#include "AIGamePlayTag.h"

namespace SLAIGameplayTags
{
	// 동료 AI 스킬 태그
	UE_DEFINE_GAMEPLAY_TAG(AI_Companion_Skill_Attack01, "AI.Companion.Skill.Attack01");
	UE_DEFINE_GAMEPLAY_TAG(AI_Companion_Skill_Attack02, "AI.Companion.Skill.Attack02");
	UE_DEFINE_GAMEPLAY_TAG(AI_Companion_Skill_Attack03, "AI.Companion.Skill.Attack03");
	
	// 동료 AI 쿨다운 태그
	UE_DEFINE_GAMEPLAY_TAG(AI_Companion_Cooldown_Attack, "AI.Companion.Cooldown.Attack");
	
	// 동료 AI 상태 태그
	UE_DEFINE_GAMEPLAY_TAG(AI_Companion_State_Attacking, "AI.Companion.State.Attacking");
	UE_DEFINE_GAMEPLAY_TAG(AI_Companion_State_Casting, "AI.Companion.State.Casting");
}
