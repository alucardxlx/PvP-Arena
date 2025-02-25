var vampire_drain := 10;

///////////////////
//  special setup for vampires
///////////////////

function VampireSetup ()
	var MyTemplateElem := GetNpcTemplateElem (me.npctemplate);
	vampire_drain := MyTemplateElem.vampire_drain;
	if (!vampire_drain)
		vampire_drain := 10;
	endif
endfunction




///////////////////
//  looks around the area for other NPCs that are 'friendly' with the NPC being attacked, and asks
//  them to attack
///////////////////

function GetHelpFromAllies (byref opponent)

	var ev := array {};
	ev.+type := EVID_ALL_ATTACK_CMD;
	ev.+source := opponent;

	foreach mobile in ListMobilesNearLocation ( me.x, me.y, me.z, 8 )
		if ( IsAnUndeadCreature (mobile.npctemplate) and !mobile.warmode and mobile != me )
			SendEvent (mobile, ev);
		endif
	endforeach

endfunction



