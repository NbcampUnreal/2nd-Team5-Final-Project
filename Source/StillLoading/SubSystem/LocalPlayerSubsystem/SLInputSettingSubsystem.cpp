#include "SLInputSettingSubsystem.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void USLInputSettingSubsystem::MapKey(UInputMappingContext* Context, const UInputAction* Action, const FKey NewKey) const
{
	if (!Context || !Action) return;

	Context->UnmapKey(Action, NewKey);
	Context->MapKey(Action, NewKey);

	if (APlayerController* PC = GetLocalPlayer()->GetPlayerController(GetWorld()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			InputSub->RemoveMappingContext(Context);
			InputSub->AddMappingContext(Context, 0);
		}
	}
}

TObjectPtr<UInputMappingContext> USLInputSettingSubsystem::GetOrCreateIMC(const FName ContextName)
{
	if (MappingContexts.Contains(ContextName))
		return MappingContexts[ContextName];

	UInputMappingContext* NewIMC = NewObject<UInputMappingContext>(this);
	MappingContexts.Add(ContextName, NewIMC);
	return NewIMC;
}