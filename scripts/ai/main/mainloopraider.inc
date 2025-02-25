//FIXME: because there are hundreds of these fellas, the ai might be a tad to complex, ie, dont run on a slow server..
// These npcs dont resopnd well to .restarts, so dont restart em
//v0.7c
use vitals;
use cfgfile;
use util;
use os;
use npc;
use polsys;
use basic;
use math;
use uo;

include "include/zuluhotel/raidconsts";
include "include/attributes";
include "include/objtype";

var dontloot := 0;
var waypointcounter := 1; //where we are in terms of waypoints
var waypointcount := 0;// how many waypoints?
var goalx, goaly;
var los_tries := 0;


var lastd := 0; //lastdistance
var last_check_pathfinding := ReadGameClock()+20; //used for checking for obsticles

var raidid:=GetProperty("#townraider");
var settings := { };
var raidupdateinfo := { };

function main_AI_loop()

	me.disable( "invul" );
	RevokePrivilege( me, "invul" );
	me.setMurderer(1); //why the hell dont this work?
	//need to create a master to assign npc too.
	SetName(me, me.name + " [" + GetGlobalProperty("raidname"+raidid) + " raider]"); //doesent restart to well

    EnableMainEvents();
	settings := GetGlobalProperty("raidsettings"+raidid);

	if ((!settings[SET_XGOAL]) || (!settings[SET_YGOAL]) || !GetGlobalProperty("raidname"+raidid) || !raidid)
		KillME();
	endif

	var waypoints := GetGlobalProperty("raidwaypoints"+raidid);
	if (waypoints[1])
		goalx := waypoints[1];
		goaly := waypoints[2];
		waypointcount := len(waypoints)/2;
	else
		waypointcounter := 0;
		goalx := settings[SET_XGOAL];
		goaly := settings[SET_YGOAL];
	endif
	waypoints := 0;

	raidupdateinfo := GetGlobalProperty("raidupdateinfo"+raidid);

	if ((settings[SET_YOINKAMOUNT]==0)|| (raidupdateinfo[UPD_GOLDLEFT] < settings[SET_YOINKAMOUNT]) || (GetProperty("noloot")))
		dontloot := 1;
	endif

	//Theres no real way of stopping guards from owning these fellas, unless the guards are modified, or turned off
	SetProperty("guardignore",1);
	SetProperty("called", 1);

	SetAnchor( settings[SET_XGOAL], settings[SET_YGOAL], DistanceToLocation( settings[SET_XGOAL], settings[SET_YGOAL] )+10, 5 );

	if (DistanceToLocation( settings[SET_XGOAL], settings[SET_YGOAL] ) < 12)
		EnableEnteredArea();
	else
		SetDexterityMod(me, GetDexterityMod(me) + DEX_UPGD);
	endif


    var last_loot:= ReadGameClock();
    var next_check := ReadGameClock() + 10;
    var ev;
	var waittime := 0;



	//FIXME: clean this up a bit

    while (1)
		if (last_loot)
			//Were not near the gold
			if (!FindPatch())
					waittime := 0;
					sleepms(SLEEP_TIME);
			else
			//Were Yoinking
				if (!dontloot)
					if (last_loot < ReadGameClock())
						last_loot := ReadGameClock() + 20;

						raidupdateinfo := GetGlobalProperty("raidupdateinfo"+raidid);
						if (raidupdateinfo[UPD_GOLDLEFT] >= settings[SET_YOINKAMOUNT])
							raidupdateinfo[UPD_GOLDLEFT] := raidupdateinfo[UPD_GOLDLEFT] - CInt(settings[SET_YOINKAMOUNT]);
							SetGlobalProperty("raidupdateinfo"+raidid, raidupdateinfo);
							say("*Yoink*");
							waittime := 2;
							sleep(2);
						else
							if (settings[SET_STOPNOGOLD]) //Stop the raid coz theres no gold left
								say("We have the gold!");
								StopRaid();
								next_check := 0;
							else
								say("We have all da loot!");
								next_check := 0;
							endif
							last_loot := 0;
							waittime := 1;
							SetAnchor( settings[SET_XGOAL], settings[SET_YGOAL], 9, 100 );
							EnableEnteredArea();
						endif
					else
						waittime := 1;
						sleepms(500);
					endif
				else
					SetAnchor( settings[SET_XGOAL], settings[SET_YGOAL], 6, 100 );
					last_loot := 0;
				endif
				waittime := 0;
			endif
			ev := os::wait_for_event( waittime );
		else
			ev := os::wait_for_event( 1 );
		endif
	
		if ((ev) && (ev.source))
			case (ev.type)
				SYSEVENT_ENTEREDAREA: //this is clogging up when there are alot on npcs in one area
					if (settings[SET_RAIDERWAR])
						if (GetObjProperty(ev.source, "#townraider")!=raidid)
							say("Die!");
							Fight( ev.source );
						endif
					endif

					if ( ev.source.script!="raidkillpcs" )
						if (RandomInt(10)<8)
							if ( (((ev.source.script=="raiddefendpcs") || (ev.source.script=="hireling") || (ev.source.script=="tamed")) || (!ev.source.npctemplate)))
								Fight( ev.source );
 							endif
				 		endif
					endif
				SYSEVENT_ENGAGED:
					if (RandomInt(3)==1)
	    				get_help( ev.source );
					endif
	    			Fight(ev.source);
				SYSEVENT_DAMAGED:
	    			Fight(ev.source);
				EVID_ALL_ATTACK_CMD:
					if (RandomInt(2)==1)
						Fight(ev.source);//dont gang up
					endif
			endcase
		endif


		if (next_check < ReadGameClock())

			if (!GetGlobalProperty("raidname"+raidid)) //check for finished raid (note if they are fighting, they dun stop)
				KillME();
				return;
			endif

			if (!last_loot)
				wander();
			endif

			next_check := ReadGameClock() + 8;

			look_around();
		endif

   endwhile     

endfunction      


function FindPatch() //Finds way along path to goal and check los if we stay in the same point for too long

	var lastwaypoint := 0;
	var dist_to_walk := DistanceToLocation( goalx, goaly );

	if (dist_to_walk<YOINK_DIST) //check if we are nearing a waypoint

		if ((!waypointcounter) || (waypointcounter > waypointcount))
			var settings := GetGlobalProperty("raidsettings"+raidid);
			goalx := settings[SET_XGOAL];
			goaly := settings[SET_YGOAL];
			settings := 0;
			lastwaypoint := 0;
			dist_to_walk := DistanceToLocation( goalx, goaly ); //reset
			if ((dontloot) && (dist_to_walk < YOINK_DIST+6))
				return 1; //we can start yoinking
			elseif (dist_to_walk < YOINK_DIST)
				return 1; //we can start yoinking
			endif
		else
			var waypoints := GetGlobalProperty("raidwaypoints"+raidid);//dunno if this is slower if not a global

			if (waypointcounter==1)
				goalx := waypoints[1];
				goaly := waypoints[2];
			else
				goalx := waypoints[(waypointcounter*2)-1];
				goaly := waypoints[waypointcounter*2];
			endif
			dist_to_walk := DistanceToLocation( goalx, goaly ); //reset

			if (inpoint(goalx,goaly,2))
				waypointcounter:=waypointcounter+1;
			endif

		endif

	endif


	if (dist_to_walk < 10)
		if (lastwaypoint)
			WalkTowardLocation(goalx, goaly);
		else
			RunTowardLocation(goalx, goaly);
		endif
	elseif (dist_to_walk<16)
		if (lastwaypoint)
			RemoveDEXUpgd();
			WalkTowardLocation(goalx, goaly);
			WalkTowardLocation(goalx, goaly);
			EnableEnteredArea();
		else
			RunTowardLocation(goalx, goaly);
		endif
	else
		RunTowardLocation(goalx, goaly);
		case(RandomInt(10)) //so its not a big line
			1: Move( me.facing+1 ); Move( me.facing );
			2: Move( me.facing-1 ); Move( me.facing );
		endcase
		RunTowardLocation(goalx, goaly);
	endif

	if (last_check_pathfinding < ReadGameClock())

		if (close(lastd,dist_to_walk,2)) //do we have a block?

			if(CheckForStuff()) //incase the players have casted a wall, cast a dispell
				los_tries := 0;
			else
				me.facing:=me.facing+1;
				Move(me.facing);
				Move(me.facing);
				Move(me.facing);
				los_tries := los_tries + 1;
				if (los_tries > 4)
					KillME();
				endif
			endif

			last_check_pathfinding := ReadGameClock() + 8;
		else
			last_check_pathfinding := ReadGameClock() + 3;
		endif

		lastd := dist_to_walk;
	endif

endfunction

function CheckForStuff()

	var itemdesc := ReadConfigFile( ":doors:itemdesc" );
	var doorcount;

	foreach wall in ListItemsNearLocation( me.x,me.y,me.z,2 ) //make this check for walls infront, not all around
		if ((wall.objtype == UOBJ_STONEWALL_NS) || (wall.objtype == UOBJ_STONEWALL_EW))
			if (GetObjProperty(wall, "f") == "1")

				if ((me.script == "raidspellkillpcs") || (me.script == "raidcritterhealer"))
					WalkAwayFrom(wall);
					TurnToward(wall);
					sleepms(500);
					say("An Grav");
					PerformAction( me, 0x0010 );
					sleep(2);
					PlaySoundEffect( me, 0x0202 );
    				foreach item in ListItemsNearLocation(me.x, me.y, me.z, 3)
						if (GetObjProperty(item, "f") == "1")
							PlayStationaryEffect( item.x, item.y, item.z, 0x3728, 0xa, 0xa );
                			DestroyItem(item);
						endif
					endforeach
					return 1;
				else
					RunTowardLocation(wall.x, wall.y);
					sleep(1);
					if (wall)
						PerformAction( me, 0x0009 );
						PlaySoundEffect(me, 0x0233);
						say("*SMASH*");
						sleep(2);
					else
						return 1;
					endif
					if (wall)
						PerformAction( me, 0x0009 );
						PlaySoundEffect(me, 0x0233);
						say("*BAM*");
						sleep(2);
					else
						return 1;
					endif

					if (wall)
						sleepms(500);
						PlaySoundEffect(me, 0x020e);
           				DestroyItem(wall);
					endif
					return 1;
				endif

			endif
		endif

		if (itemdesc[wall.objtype].doortype && (wall.locked==0))
			start_script(":doors:open_door",wall);
			RunTowardLocation(wall.x, wall.y);
			doorcount := doorcount + 1;
			if (doorcount > 1)
				return 1;
			endif
		endif

	endforeach

endfunction


/** Utilitys **/

function look_around()
	if (!settings[SET_PROVOKE])
		foreach critter in ListHostiles(me,3)
			if (RandomInt(6)<5)
    			get_help( critter );
				Fight( critter );
				return;
			endif
		endforeach
	endif

endfunction


function EnableMainEvents()
    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED);
    EnableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, HALT_THRESHOLD );
endfunction


function DisableMainEvents()
    DisableEvents( SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA);
endfunction

function get_help( opponent )

	var inev;
	var ev array;
	ev.+type := EVID_ALL_ATTACK_CMD;
	ev.+source := opponent;

	foreach critter in ListMobilesNearLocation( me.x, me.y, me.z, 2 )
		if ( ( (critter.script["raid"]) && (critter.script!="raiddefendpcs") ) && (!critter.warmode) && (critter.serial != me.serial) )
			SendEvent(critter, ev);
		endif
		inev := wait_for_event(0);
		if (inev.type == EVID_ALL_ATTACK_CMD)
			return;
		endif
	endforeach

endfunction

function DistanceToLocation( locx, locy )
	var xdiff := Abs(me.x - locx);
	var ydiff := Abs(me.y - locy);

	if (xdiff > ydiff)
		return xdiff;
	else
		return ydiff;
	endif
endfunction

function inpoint(yoink_x,yoink_y,range:=YOINK_DIST)
	if ((Abs(me.x - yoink_x) < range) && (Abs(me.y - yoink_y) < range))
		return 1;
	endif
endfunction

function close(val1, val2, range:=1)
	if (Abs(val1 - val2) < range)
		return 1;
	endif
endfunction

function cleareventque()
	var ev;
	repeat
	until (! (ev := os::wait_for_event(0)) );
endfunction

function KillME()
	DisableMainEvents();
	MoveObjectToLocation(me,5288, 1176, 0, me.realm,MOVEOBJECT_FORCELOCATION);
	SetObjProperty(me,"guardkill",1);
	ApplyRawDamage( me, GetMaxHp(me) + 8 );
endfunction

function RemoveDEXUpgd()
	los_tries:=0;
	if (GetDexterityMod(me) >= DEX_UPGD)
		SetDexterityMod(me, GetDexterityMod(me) - DEX_UPGD);
	endif
endfunction

function StopRaid()
	EraseGlobalProperty("raidname"+raidid);
endfunction

function EnableEnteredArea()
	if (!settings[SET_PROVOKE])
		EnableEvents(SYSEVENT_ENTEREDAREA,HALT_THRESHOLD);
	endif
endfunction
