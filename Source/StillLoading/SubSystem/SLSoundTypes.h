// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLSoundTypes.generated.h"

UENUM(BlueprintType)
enum class ESLBgmSoundType : uint8
{
	EBS_None = 0,
	EBS_Intro,
	EBS_Title,
	EBS_Battle,
	EBS_Mini1,
	EBS_Mini2,
	EBS_Mini3,
	EBS_Mini4,
	EBS_Mini5,
	EBS_Mini6,
	EBS_RidingGame,
	EBS_DebugRoom,
	EBS_BossBattle,
	EBS_Ending,
	EBS_Mini1Battle
};

UENUM(BlueprintType)
enum class ESLUISoundType : uint8
{
	EUS_None = 0,
	EUS_Click,
	EUS_Hover,
	EUS_Open,
	EUS_Close,
	EUS_Notify,
	EUS_Talk
};

UENUM(BlueprintType)
enum class EBattleSoundType : uint8
{
	// Character
	BST_CharacterAttack1 UMETA(DisplayName = "Character Attack1"),
	BST_CharacterAttack2 UMETA(DisplayName = "Character Attack2"),
	BST_CharacterAttack3 UMETA(DisplayName = "Character Attack3"),
	BST_CharacterSpecialAttack1 UMETA(DisplayName = "Character Special Attack1"),
	BST_CharacterSpecialAttack2 UMETA(DisplayName = "Character Special Attack2"),
	BST_CharacterSpecialAttack3 UMETA(DisplayName = "Character Special Attack3"),
	BST_CharacterAirAttack1 UMETA(DisplayName = "Character Air Attack1"),
	BST_CharacterAirAttack2 UMETA(DisplayName = "Character Air Attack2"),
	BST_CharacterAirAttack3 UMETA(DisplayName = "Character Air Attack3"),
	BST_CharacterHit UMETA(DisplayName = "Character Hit"),
	BST_CharacterWalk UMETA(DisplayName = "Character Walk"),
	BST_CharacterDodge UMETA(DisplayName = "Character Dodge"),
	BST_CharacterSpirit UMETA(DisplayName = "Character Spirit"),
	BST_CharacterGuard UMETA(DisplayName = "Character Guard"),
	BST_CharacterGuardBreak UMETA(DisplayName = "Character GuardBreak"),
	BST_CharacterSkill UMETA(DisplayName = "Character Skill"),
	BST_CharacterDie UMETA(DisplayName = "Character Die"),
	BST_CharacterOnGround UMETA(DisplayName = "Character On Ground"),
	BST_CharacterBeginDefence UMETA(DisplayName = "Character Begin Defence"),
	BST_CharacterEndDefence UMETA(DisplayName = "Character End Defence"),

	// Monster
	BST_MonsterAttack UMETA(DisplayName = "Monster Attack"),
	BST_MonsterHit UMETA(DisplayName = "Monster Hit"),
	BST_MonsterDie UMETA(DisplayName = "Monster Die"),

	// AI
	BST_AIBossAttack UMETA(DisplayName = "AI Boss Attack"),
	BST_AIBossJumpAttack UMETA(DisplayName = "AI Boss Jump Attack"),
	BST_AIBossGroundSlamAttack UMETA(DisplayName = "AI Boss GroundSlam Attack"),
	BST_AIBossSlashAttack UMETA(DisplayName = "AI Boss Slash Attack"),
	BST_AIBossHit UMETA(DisplayName = "AI Boss Hit"),
	
	BST_AICompanionAttack01 UMETA(DisplayName = "AI Companion Attack01"),
	BST_AICompanionAttack02 UMETA(DisplayName = "AI Companion Attack02"),
	BST_AICompanionHit UMETA(DisplayName = "AI Companion Hit"),

	FootstepDefault UMETA(DisplayName = "Footstep Default"),
	FootstepRock UMETA(DisplayName = "Footstep Rock"),
	FootstepSoil UMETA(DisplayName = "Footstep Soil"),
};

UCLASS()
class STILLLOADING_API USLSoundTypes : public UObject
{
	GENERATED_BODY()
};
