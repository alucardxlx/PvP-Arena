use os;
use npc;
use vitals;
use basic;
use util;
use math;
use uo;
include "include/attributes";
function process_combat_event(opponent)
  if(Distance(me,opponent) < 2)
    var dmg := RandomInt(CInt(GetIntelligence(opponent) / 10));
    if(dmg > 12)
      dmg := 12;
    elseif(dmg < 2)
      dmg := 2;
    endif
	ApplyRawDamage(opponent, dmg);
  endif
endfunction

function prepare_for_fight(opponent)

	DisableMainEvents();
	EnableCombatEvents();

endfunction

function post_combat()

	DisableCombatEvents();
	EnableMainEvents();

	SetWarMode( 0 );
	SetOpponent( 0 );

	sleep(1);
	look_around();

endfunction
