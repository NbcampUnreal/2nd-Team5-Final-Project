#include "SLMovementComponentBase.h"

#include "Character/SLPlayerCharacter.h"
#include "Character/BattleComponent/BattleComponent.h"
#include "Character/CombatHandlerComponent/CombatHandlerComponent.h"
#include "Character/DynamicIMCComponent/SLDynamicIMCComponent.h"
#include "Character/Interaction/SLInteractionComponent.h"
#include "Character/MontageComponent/AnimationMontageComponent.h"
#include "Character/PlayerState/SLBattlePlayerState.h"
#include "Character/RadarComponent/CollisionRadarComponent.h"
#include "Controller/SLBasePlayerController.h"
#include "Interactable/SLInteractableObjectBase.h"
#include "SubSystem/SLSoundSubsystem.h"
#include "UI/HUD/SLBaseHUD.h"
#include "UI/HUD/SLInGameHUD.h"

USLMovementComponentBase::USLMovementComponentBase()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLMovementComponentBase::BeginPlay()
{
	Super::BeginPlay();

	if (auto* IMC = GetOwner()->FindComponentByClass<UDynamicIMCComponent>())
	{
		IMC->OnActionTriggered.AddDynamic(this, &USLMovementComponentBase::OnActionTriggered);
		IMC->OnActionStarted.AddDynamic(this, &USLMovementComponentBase::OnActionStarted);
		IMC->OnActionCompleted.AddDynamic(this, &USLMovementComponentBase::OnActionCompleted);
	}

	OwnerCharacter = Cast<ASLPlayerCharacter>(GetOwner());

	if (OwnerCharacter)
	{
		CachedMontageComponent = OwnerCharacter->FindComponentByClass<UAnimationMontageComponent>();
		CachedCombatComponent = OwnerCharacter->FindComponentByClass<UCombatHandlerComponent>();
		CachedBattleComponent = OwnerCharacter->FindComponentByClass<UBattleComponent>();
		CachedRadarComponent = OwnerCharacter->FindComponentByClass<UCollisionRadarComponent>();
		CachedInteractionComponent = OwnerCharacter->FindComponentByClass<USLInteractionComponent>();
		
		CachedBattleSoundSubsystem = GetBattleSoundSubSystem();

		CachedSkeletalMesh = OwnerCharacter->GetMesh();
	}

	if (CachedBattleComponent)
	{
		CachedBattleComponent->OnCharacterHited.AddDynamic(this, &USLMovementComponentBase::OnHitReceived);
	}
}

USLSoundSubsystem* USLMovementComponentBase::GetBattleSoundSubSystem() const
{
	if (const UWorld* World = GetWorld())
	{
		if (const UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<USLSoundSubsystem>();
		}
	}

	return nullptr;
}

void USLMovementComponentBase::OpenMenu() const
{
	if (const ASLBasePlayerController* PlayerController = Cast<ASLBasePlayerController>(OwnerCharacter->GetController()))
	{
		if (ASLBaseHUD* CurrentHUD = Cast<ASLBaseHUD>(PlayerController->GetHUD()))
		{
			CurrentHUD->OnPause();
		}
	}
}

void USLMovementComponentBase::Interact() const
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_InputBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("USLMovementComponentBase: Input Blocked"));
		return;
	}
	
	if (ASLInteractableObjectBase* InteractableObject = CachedInteractionComponent->GetInteractableObject())
	{
		if (ASLPlayerCharacterBase* PlayerCharacter = Cast<ASLPlayerCharacterBase>(GetOwner()))
		{
			InteractableObject->TriggerReact(PlayerCharacter, ESLReactiveTriggerType::ERT_InteractKey);
		}
	}
}

void USLMovementComponentBase::PresentGoal() const
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_UIBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("USLMovementComponentBase: UI Blocked"));
		return;
	}

	if (const APlayerController* Controller = Cast<APlayerController>(OwnerCharacter->GetController()))
	{
		if (ASLInGameHUD* InGameHUD = Cast<ASLInGameHUD>(Controller->GetHUD()))
		{
			InGameHUD->ApplyObjective();
		}
	}
}

void USLMovementComponentBase::DanceTime() const
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_InputBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("USLMovementComponentBase: Input Blocked"));
		return;
	}
	
	//OwnerCharacter->ChangeVisibilityWeapons(false);
	CachedMontageComponent->PlayTrickMontage(FName("Dance"));
}

void USLMovementComponentBase::OnActionTriggered_Implementation(EInputActionType ActionType, FInputActionValue Value)
{
	
}

void USLMovementComponentBase::OnActionStarted_Implementation(EInputActionType ActionType)
{
	switch (ActionType)
	{
	case EInputActionType::EIAT_PresentGoal:
		PresentGoal();
		break;
	case EInputActionType::EIAT_Interaction:
		Interact();
		break;
	case EInputActionType::EIAT_Menu:
		OpenMenu();
		break;
	case EInputActionType::EIAT_Special:
		DanceTime();
		break;

	default:
		break;
	}
}

void USLMovementComponentBase::OnActionCompleted_Implementation(EInputActionType ActionType)
{
}

void USLMovementComponentBase::OnHitReceived_Implementation(AActor* Causer, float Damage, const FHitResult& HitResult,
	EHitAnimType AnimType)
{
	if (OwnerCharacter->IsConditionBlocked(EQueryType::EQT_HitBlock))
	{
		//UE_LOG(LogTemp, Warning, TEXT("USLMovementComponentBase: Hit Blocked"));
		return;
	}
	
	if (OwnerCharacter)
	{
		if (const APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController()))
		{
			if (ASLBattlePlayerState* BasePlayerState = PlayerController->GetPlayerState<ASLBattlePlayerState>())
			{
				BasePlayerState->DecreaseHealth(Damage);
			}
		}
	}
}

