#include "SLDeveloperBossPhaseConfigs.h"

FSLPhase1Config::FSLPhase1Config()
{
	BossHealthMultiplier = 0.3f;
	BossSpawnDelay = 2.0f;
	BossSpawnOffset = FVector(300.0f, 0.0f, 0.0f);
}

FSLPhase2Config::FSLPhase2Config()
{
	RoomActivationDelay = 0.5f;
	bAutoTeleportPlayer = true;
}

FSLPhase3Config::FSLPhase3Config()
{
	AutoWallAttackInterval = 4.0f;
	InitialWallAttackDelay = 2.0f;
	bRandomWallSelection = false;
	MouseActorClass = nullptr;
}

FSLPhase4Config::FSLPhase4Config()
{
	AutoWallAttackInterval = 4.0f;
	InitialWallAttackDelay = 2.0f;
}

FSLPhase5Config::FSLPhase5Config()
{
	MaxSimultaneousWalls = 3;
	MultiWallDelayMin = 0.2f;
	MultiWallDelayMax = 1.0f;
	bEnableMultiWallAttack = true;
	WallAttackInterval = 2.0f;
	WallAttackDelay = 1.0f;
	WallResetDelay = 1.5f;
	MaxActiveWalls = 2;
	bLimitActiveWalls = true;
}