use os;
use npc;
use math;
use uo;
var lastface;
var nexttry;

function CloseDistance(opponent)
      case (Distance(me, opponent))
        1:
        0:          return 1;
        default:    Opendoors(me);
                 
                    if(RunToward(opponent))
                        return 0;
                    endif
                    return 1;
    endcase

    return 0;

endfunction

function Opendoors(me)
  foreach door in ListItemsNearLocation(me.x, me.y, me.z, 1)
    if((door.isa(POLCLASS_DOOR)) and (!door.locked))
      if(!GetObjProperty(door, "WhenOpened" ))
        start_script(":doors:openDoor", door);
      endif
      EraseObjProperty(door, "#inuse");
    endif
  endforeach
endfunction

function NavigateTo(destination);
  var dX := 0;
  var dY := 0;
  var maxdist := 30;
  for X:=1 to 24
    sleepms(1);
    if (CheckLOSat(me, me.x + X - 12, me.y, me.z) and CheckLOSat(destination, me.x + X - 12, me.y, me.z))
      if(coordist(destination.x , destination.y , me.x+X-12 , me.y) < maxdist)
        dX := me.x+X-12;
        dY := me.y;
        maxdist := coordist(destination.x , destination.y , me.x+X-12 , me.y );
      endif
    else
      for Y:=1 to 24
        if(CheckLOSat(me, me.x + X - 12, me.y + Y - 12, me.z) and CheckLOSat(destination, me.x+X-12, me.y+Y-12, me.z))
          if(coordist(destination.x , destination.y , me.x+X-12 , me.y+Y-12 ) < maxdist)
            dX := me.x+X-12;
            dY := me.y+Y-12;
            maxdist := coordist(destination.x , destination.y , me.x + X - 12 , me.y);
          endif
        endif
      endfor
    endif
  endfor
  if(dX)
    return CloseDistancetoLocation(dX, dY);
  endif
  return 1;
endfunction

function CloseDistancetoLocation(x, y)
  var chk := 0;
  repeat
    RunTowardLocation(x, y);
    RunTowardLocation(x, y);
    RunTowardLocation(x, y);
    var face := me.facing;
    if(ABS(lastface - face) == 4)
      lastface := face;
      chk := chk + 1;
    endif
    lastface := face;
  until((coordist( me.x , me.y , x , y ) <= 0) or (chk > 1));
  if(chk > 1)
    return 1;
  endif
  return 0;
endfunction

function NavigateAway(targ)
  var face;
  var runaway := 0;
  repeat
    face := me.facing;
    WalkToward(targ);
    WalkToward(targ);
    WalkToward(targ);
    if(ABS(me.facing - face) = 4)
      runaway := runaway + 1;
    endif
  until ((Distance(me, targ) <= 1) or (runaway > 2) or (Distance(me, targ) > 20));
endfunction
