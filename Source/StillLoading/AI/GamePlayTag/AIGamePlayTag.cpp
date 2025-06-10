#include "AIGamePlayTag.h"

namespace SLAIGameplayTags
{
// Companion 공통 행동 패턴
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_None, "Companion.Pattern.None");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_Attack_Plunge, "Companion.Pattern.Attack.Plunge");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_Attack_Air, "Companion.Pattern.Attack.Air");
	
	// Battle Mage 콤보 패턴
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_MeleeCombo1, "Companion.Pattern.BM.MeleeCombo1");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_MeleeCombo2, "Companion.Pattern.BM.MeleeCombo2");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_MeleeCombo3, "Companion.Pattern.BM.MeleeCombo3");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_QuickCombo1, "Companion.Pattern.BM.QuickCombo1");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_HeavyCombo1, "Companion.Pattern.BM.HeavyCombo1");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_FinisherCombo1, "Companion.Pattern.BM.FinisherCombo1");
	
	// Battle Mage 개별 공격 패턴
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack01, "Companion.Pattern.BM.Attack01");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack02, "Companion.Pattern.BM.Attack02");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack03, "Companion.Pattern.BM.Attack03");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack04, "Companion.Pattern.BM.Attack04");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack05, "Companion.Pattern.BM.Attack05");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack06, "Companion.Pattern.BM.Attack06");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack07, "Companion.Pattern.BM.Attack07");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack08, "Companion.Pattern.BM.Attack08");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack09, "Companion.Pattern.BM.Attack09");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack10, "Companion.Pattern.BM.Attack10");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack11, "Companion.Pattern.BM.Attack11");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack12, "Companion.Pattern.BM.Attack12");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack13, "Companion.Pattern.BM.Attack13");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack14, "Companion.Pattern.BM.Attack14");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack15, "Companion.Pattern.BM.Attack15");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack16, "Companion.Pattern.BM.Attack16");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack17, "Companion.Pattern.BM.Attack17");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_BM_Attack18, "Companion.Pattern.BM.Attack18");
	
	// Wizard 콤보 패턴
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_MeleeCombo1, "Companion.Pattern.WZ.MeleeCombo1");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_MeleeCombo2, "Companion.Pattern.WZ.MeleeCombo2");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_RangeCombo1, "Companion.Pattern.WZ.RangeCombo1");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_RangeCombo2, "Companion.Pattern.WZ.RangeCombo2");
	
	// Wizard 개별 공격 패턴
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack01, "Companion.Pattern.WZ.Attack01");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack02, "Companion.Pattern.WZ.Attack02");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack03, "Companion.Pattern.WZ.Attack03");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack04, "Companion.Pattern.WZ.Attack04");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack05, "Companion.Pattern.WZ.Attack05");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack06, "Companion.Pattern.WZ.Attack06");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack07, "Companion.Pattern.WZ.Attack07");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack08, "Companion.Pattern.WZ.Attack08");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack09, "Companion.Pattern.WZ.Attack09");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack10, "Companion.Pattern.WZ.Attack10");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack11, "Companion.Pattern.WZ.Attack11");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack12, "Companion.Pattern.WZ.Attack12");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack13, "Companion.Pattern.WZ.Attack13");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack14, "Companion.Pattern.WZ.Attack14");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack15, "Companion.Pattern.WZ.Attack15");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack16, "Companion.Pattern.WZ.Attack16");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack17, "Companion.Pattern.WZ.Attack17");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack18, "Companion.Pattern.WZ.Attack18");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack19, "Companion.Pattern.WZ.Attack19");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack20, "Companion.Pattern.WZ.Attack20");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack21, "Companion.Pattern.WZ.Attack21");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack22, "Companion.Pattern.WZ.Attack22");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Attack23, "Companion.Pattern.WZ.Attack23");

	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Special_Patterns1, "Companion.Pattern.WZ.Special.Patterns1");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Special_Patterns2, "Companion.Pattern.WZ.Special.Patterns2");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Special_Patterns3, "Companion.Pattern.WZ.Special.Patterns3");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Special_Patterns4, "Companion.Pattern.WZ.Special.Patterns4");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Special_Patterns5, "Companion.Pattern.WZ.Special.Patterns5");
	
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Aim_Attack01, "Companion.Pattern.WZ.Aim.Attack01");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Aim_Attack02, "Companion.Pattern.WZ.Aim.Attack02");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Aim_Attack03, "Companion.Pattern.WZ.Aim.Attack03");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Aim_Attack04, "Companion.Pattern.WZ.Aim.Attack04");

	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Loop_Aim_Attack01, "Companion.Pattern.WZ.Loop.Aim.Attack01");
	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Loop_Aim_Attack02, "Companion.Pattern.WZ.Loop.Aim.Attack02");

	UE_DEFINE_GAMEPLAY_TAG(Companion_Pattern_WZ_Loop_Attack04, "Companion.Pattern.WZ.Loop.Attack04");


	// Doppelganger 행동 패턴
	UE_DEFINE_GAMEPLAY_TAG(Doppelganger_Pattern_None, "Doppelganger.Pattern.None");

	// Doppelganger 콤보 공격
	UE_DEFINE_GAMEPLAY_TAG(Doppelganger_Pattern_Combo_Attack_01, "Doppelganger.Pattern.Combo.Attack01");
	UE_DEFINE_GAMEPLAY_TAG(Doppelganger_Pattern_Combo_Attack_02, "Doppelganger.Pattern.Combo.Attack02");
	UE_DEFINE_GAMEPLAY_TAG(Doppelganger_Pattern_Combo_Attack_03, "Doppelganger.Pattern.Combo.Attack03");
	UE_DEFINE_GAMEPLAY_TAG(Doppelganger_Pattern_Combo_Attack_04, "Doppelganger.Pattern.Combo.Attack04");
	UE_DEFINE_GAMEPLAY_TAG(Doppelganger_Pattern_Combo_Attack_Air, "Doppelganger.Pattern.Combo.AttackAir");

	// Doppelganger 특수 공격
	UE_DEFINE_GAMEPLAY_TAG(Doppelganger_Pattern_Attack_Up_01, "Doppelganger.Pattern.Attack.Up01");
	UE_DEFINE_GAMEPLAY_TAG(Doppelganger_Pattern_Attack_Up_Floor_To_Air_02, "Doppelganger.Pattern.Attack.UpFloorToAir02");
	UE_DEFINE_GAMEPLAY_TAG(Doppelganger_Pattern_Run_Attack_02, "Doppelganger.Pattern.Attack.Run02");
	UE_DEFINE_GAMEPLAY_TAG(Doppelganger_Pattern_Dash_Air_Attack, "Doppelganger.Pattern.Attack.DashAir");

	// Doppelganger 방어 및 지원
	UE_DEFINE_GAMEPLAY_TAG(Doppelganger_Pattern_Guard, "Doppelganger.Pattern.Defense.Guard");
	UE_DEFINE_GAMEPLAY_TAG(Doppelganger_Pattern_Parry, "Doppelganger.Pattern.Defense.Parry");
	UE_DEFINE_GAMEPLAY_TAG(Doppelganger_Pattern_Buff_Cast, "Doppelganger.Pattern.Support.BuffCast");

}
