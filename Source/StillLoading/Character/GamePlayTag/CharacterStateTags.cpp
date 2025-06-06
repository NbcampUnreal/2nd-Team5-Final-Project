#include "CharacterStateTags.h"
#include "GamePlayTag.h"

// Character
UE_DEFINE_GAMEPLAY_TAG(TAG_Character, "Character");

// Side Effect
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_PrepareLockOn, "Character.PrepareLockOn");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_LockOn, "Character.LockOn");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Invulnerable, "Character.Invulnerable");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_OnBuff, "Character.OnBuff");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Dead, "Character.Dead");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Empowered, "Character.Empowered");

// Hit Reactions
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction, "Character.HitReaction");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Weak, "Character.HitReaction.Weak");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Medium, "Character.HitReaction.Medium");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Area, "Character.HitReaction.Area");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Airborne, "Character.HitReaction.Airborne");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Groggy, "Character.HitReaction.Groggy");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Knockback, "Character.HitReaction.Knockback");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Falling, "Character.HitReaction.Falling");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Block_Break, "Character.HitReaction.Block.Break");

// Attacks
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack, "Character.Attack");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_Basic1, "Character.Attack.Basic1");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_Basic2, "Character.Attack.Basic2");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_Basic3, "Character.Attack.Basic3");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_Special1, "Character.Attack.Special1");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_Special2, "Character.Attack.Special2");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_Special3, "Character.Attack.Special3");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_Air1, "Character.Attack.Air1");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_Air2, "Character.Attack.Air2");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_Air3, "Character.Attack.Air3");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_ExecutionA, "Character.Attack.ExecutionA");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_ExecutionB, "Character.Attack.ExecutionB");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_ExecutionC, "Character.Attack.ExecutionC");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_Airborne, "Character.Attack.Airborne");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_Airup, "Character.Attack.Airup");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_Airdown, "Character.Attack.Airdown");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_Blast, "Character.Attack.Blast");

// Charge
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Charge, "Character.Charge");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Charge_Basic, "Character.Charge.Basic");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Charge_Special, "Character.Charge.Special");

// Defense
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Defense, "Character.Defense");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Defense_Block, "Character.Defense.Block");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Defense_Parry, "Character.Defense.Parry");

// Movement
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Movement, "Character.Movement");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Movement_Idle, "Character.Movement.Idle");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Movement_Forward, "Character.Movement.Forward");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Movement_Backward, "Character.Movement.Backward");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Movement_Dash, "Character.Movement.Dash");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Movement_Jump, "Character.Movement.Jump");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Movement_Run, "Character.Movement.Run");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Movement_Walk, "Character.Movement.Walk");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Movement_Dodge, "Character.Movement.Dodge");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Movement_OnAir, "Character.Movement.OnAir");

// AI
UE_DEFINE_GAMEPLAY_TAG(TAG_AI, "AI");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_Hit, "AI.Hit");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_Hit_Air, "AI.Hit.Air");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_Dead, "AI.Dead");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_IsAttacking, "AI.IsAttacking");

// AI State
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_Idle, "AI.Idle");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_AbleToAttack, "AI.AbleToAttack");

// 전략 - 개별 행동
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_STRATEGY_SINGLE_WANDER, "AI.Strategy.Single.Wander"); // 방향 없이 배회하며 접근
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_STRATEGY_SINGLE_AGGRESSIVE, "AI.Strategy.Single.Aggressive"); // 시야 확보 시 바로 돌진 공격
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_STRATEGY_SINGLE_SNEAKY, "AI.Strategy.Single.Sneaky"); // 은밀하게 접근 후 공격

// 전략 - 그룹 행동
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_STRATEGY_GROUPED_SWARM, "AI.Strategy.Grouped.Swarm"); // 무리 지어 몰려다니며 공격 (난잡한 형태)
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_STRATEGY_GROUPED_FORMATION, "AI.Strategy.Grouped.Formation"); // 대열을 갖춘 정돈된 전술적 움직임
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_STRATEGY_GROUPED_FLANK, "AI.Strategy.Grouped.Flank"); // 측면 또는 후방에서 공격
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_STRATEGY_GROUPED_PATROL, "AI.Strategy.Grouped.Patrol"); // 일정 구역을 분산 정찰

// 전략 - 조직적 행동
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_STRATEGY_ORGANIZED_SQUAD, "AI.Strategy.Organized.Squad"); // 소규모 팀 단위로 움직이며 역할 분담
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_STRATEGY_ORGANIZED_HOLDPOSITION, "AI.Strategy.Organized.HoldPosition"); // 지정 위치 방어 유지
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_STRATEGY_ORGANIZED_ATTACK, "AI.Strategy.Organized.Attack");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_STRATEGY_ORGANIZED_MOVEPOSITION, "AI.Strategy.Organized.MovePosition");