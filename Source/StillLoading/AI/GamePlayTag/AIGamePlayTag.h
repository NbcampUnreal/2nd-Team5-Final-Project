#pragma once

#include "NativeGameplayTags.h"

namespace SLAIGameplayTags
{
	// 종류 -> 이름 -> 스킬 , 상태,
	
	// 동료 AI 스킬 태그
	STILLLOADING_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Companion_Skill_Attack01);
	STILLLOADING_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Companion_Skill_Attack02);
	STILLLOADING_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Companion_Skill_Attack03);
	
	// 동료 AI 쿨다운 태그
	STILLLOADING_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Companion_Cooldown_Attack);
	
	// 동료 AI 상태 태그
	STILLLOADING_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Companion_State_Attacking);
	STILLLOADING_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Companion_State_Casting);
}