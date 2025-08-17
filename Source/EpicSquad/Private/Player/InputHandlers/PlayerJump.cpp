// CloudHu:604746493@qq.com All Rights Reserved


#include "Player/InputHandlers/PlayerJump.h"

#include "NinjaInputTags.h"

#include "Components/NinjaInputManagerComponent.h"

#include "GameFramework/Character.h"

#include "Interfaces/TraversalMovementInputInterface.h"

UPlayerJump::UPlayerJump()
{
	MinimumMagnitudeToJump = 0.1f;
	
	BlockJumpTags = FGameplayTagContainer::EmptyContainer;
	BlockJumpTags.AddTagFast(Tag_Input_Block_Movement);

	TriggerEvents.Add(ETriggerEvent::Started);
	TriggerEvents.Add(ETriggerEvent::Triggered);
	TriggerEvents.Add(ETriggerEvent::Completed);

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionRef(TEXT("/Script/EnhancedInput.InputAction'/NinjaInput/Input/IA_NI_Jump.IA_NI_Jump'"));
	if (InputActionRef.Succeeded())
	{
		const TObjectPtr<UInputAction> InputAction = InputActionRef.Object;
		InputActions.AddUnique(InputAction);
	}
}

void UPlayerJump::HandleStartedEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value, const UInputAction* InputAction) const
{
	if (!CanJump(Manager, Value))
	{
		return;
	}

	TryTraversalJumpAction(Manager, Value, InputAction, ETriggerEvent::Started);
}

void UPlayerJump::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value, const UInputAction* InputAction,
	float ElapsedTime) const
{
	if (!CanJump(Manager, Value))
	{
		return;
	}

	if (TryTraversalJumpAction(Manager, Value, InputAction, ETriggerEvent::Triggered))
	{
		return;
	}

	if (ACharacter* OwningCharacter = Cast<ACharacter>(Manager->GetPawn()); IsValid(OwningCharacter))
	{
		OwningCharacter->Jump();
	}
}

void UPlayerJump::HandleCompletedEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value, const UInputAction* InputAction,
	float ElapsedTime) const
{
	if (ACharacter* OwningCharacter = Cast<ACharacter>(Manager->GetPawn()); IsValid(OwningCharacter))
	{
		OwningCharacter->StopJumping();
	}
}

bool UPlayerJump::TryTraversalJumpAction_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value, const UInputAction* InputAction,
                                                        ETriggerEvent TriggerEvent) const
{
	if (APawn* OwningPawn = Manager->GetPawn(); IsValid(OwningPawn) && OwningPawn->Implements<UTraversalMovementInputInterface>())
	{
		return ITraversalMovementInputInterface::Execute_TryTraversalJumpAction(
			OwningPawn, Value, InputAction, ETriggerEvent::Triggered);
	}

	return false;
}

bool UPlayerJump::CanJump_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value) const
{
	// Ensure that we DO NOT have any of the blocking tags and the value is past the minimum threshold.
	return !HasAnyTags(Manager, BlockJumpTags) && Value.GetMagnitude() >= MinimumMagnitudeToJump;
}
