use os;
use npc;
use uo;
include "include/attributes";
function process_combat_event(opponent)
  if((GetHp(me) * 100/GetMaxHp(me)) < flee_point)
	EraseObjProperty(me,"#flees");
    flee(opponent);
  endif
endfunction

function process_flee_event(opponent)
  opponent := opponent; // stops ecompile complaining that it's not used
  return 0;
endfunction



function post_combat()
  DisableCombatEvents();
  EnableMainEvents();
  SetWarMode(0);
  SetOpponent(0);
  sleep(1);
  look_around();
endfunction

function prepare_for_fight(opponent)
  opponent := opponent; // stops ecompile complaining that it's not used
  DisableMainEvents();
  EnableCombatEvents();
endfunction
