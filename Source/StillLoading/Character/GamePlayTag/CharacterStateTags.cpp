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
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_EnterCinematic, "Character.EnterCinematic");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_BlockBreak, "Character.BlockBreak");

// Hit Reactions
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction, "Character.HitReaction");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Weak, "Character.HitReaction.Weak");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Medium, "Character.HitReaction.Medium");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Area, "Character.HitReaction.Area");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Airborne, "Character.HitReaction.Airborne");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Groggy, "Character.HitReaction.Groggy");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Knockback, "Character.HitReaction.Knockback");
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_HitReaction_Falling, "Character.HitReaction.Falling");

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
UE_DEFINE_GAMEPLAY_TAG(TAG_Character_Attack_SpawnSword, "Character.Attack.SpawnSword");

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

//Position
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_Leader, "AI.Leader");

// AI State
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_Idle, "AI.Idle");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_AbleToAttack, "AI.AbleToAttack");
UE_DEFINE_GAMEPLAY_TAG(TAG_AI_IsPlayingMontage, "AI.IsPlayingMontage");

// 잡몹 행동 패턴
UE_DEFINE_GAMEPLAY_TAG(TAG_JOBMOB_ATTACK, "JobMob.Attack");
UE_DEFINE_GAMEPLAY_TAG(TAG_JOBMOB_MOVE, "JobMob.Move");
UE_DEFINE_GAMEPLAY_TAG(TAG_JOBMOB_HOLDPOSITION, "JobMob.HoldPosition");