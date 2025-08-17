// CloudHu:604746493@qq.com All Rights Reserved


#include "Player/InputHandlers/IH_PlayerMove.h"
#include "NinjaInputTags.h"
#include "Components/NinjaInputManagerComponent.h"
#include "Player/EpicSquadPlayerCharacter.h"

UIH_PlayerMove::UIH_PlayerMove()
{
	bUseWorldSpace = false;
	MinimumMagnitudeToMove = 0.3f;

	BlockMovementTags = FGameplayTagContainer::EmptyContainer;
	BlockMovementTags.AddTagFast(Tag_Input_Block_Movement);

	TriggerEvents.Add(ETriggerEvent::Triggered);
	TriggerEvents.Add(ETriggerEvent::Ongoing);

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionRef(TEXT("/Script/EnhancedInput.InputAction'/NinjaInput/Input/IA_NI_Move.IA_NI_Move'"));
	if (InputActionRef.Succeeded())
	{
		const TObjectPtr<UInputAction> InputAction = InputActionRef.Object;
		InputActions.AddUnique(InputAction);
	}
}

void UIH_PlayerMove::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value, const UInputAction* InputAction,
                                                         float ElapsedTime) const
{
	Move(Manager, Value);
}

void UIH_PlayerMove::HandleOngoingEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value, const UInputAction* InputAction,
                                                       float ElapsedTime) const
{
	Move(Manager, Value);
}

FVector2D UIH_PlayerMove::GetMovementInputScaleValue_Implementation(const UNinjaInputManagerComponent* Manager, const FVector2D& InputValue) const
{
	switch (Manager->GetAnalogStickBehavior())
	{
	case EInputAnalogStickBehavior::FixedSpeed_SingleGait:
	case EInputAnalogStickBehavior::FixedSpeed_WalkRun:
		return InputValue.GetSafeNormal();

	case EInputAnalogStickBehavior::VariableSpeed_SingleGait:
	case EInputAnalogStickBehavior::VariableSpeed_WalkRun:
		return InputValue;
	}

	return InputValue;
}

void UIH_PlayerMove::Move_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value) const
{
	if (!CanMove(Manager, Value))
	{
		return;
	}

	if (APawn* Pawn = Cast<APawn>(Manager->GetPawn()))
	{
		FVector2D InputValue = Value.Get<FVector2D>();

		if (bUseWorldSpace)
		{
			InputValue = InputValue.GetSafeNormal();

			const FVector ForwardDirection = FVector::ForwardVector;
			Pawn->AddMovementInput(ForwardDirection, InputValue[1]);

			const FVector RightDirection = FVector::RightVector;
			Pawn->AddMovementInput(RightDirection, InputValue[0]);
			// if (const AEpicSquadPlayerCharacter* Player = Cast<AEpicSquadPlayerCharacter>(Pawn))
			// {
			// 	Player->GetImmersiveComponent()->SetAxisForward(InputValue[1]);
			// 	Player->GetImmersiveComponent()->SetAxisRight(InputValue[0]);
			// }
		}
		else
		{
			InputValue = GetMovementInputScaleValue(Manager, InputValue);

			const FVector ForwardDirection = Manager->GetForwardVector();
			Pawn->AddMovementInput(ForwardDirection, InputValue[1]);

			const FVector RightDirection = Manager->GetRightVector();
			Pawn->AddMovementInput(RightDirection, InputValue[0]);

			// if (const AEpicSquadPlayerCharacter* Player = Cast<AEpicSquadPlayerCharacter>(Pawn))
			// {
			// 	Player->GetImmersiveComponent()->SetAxisForward(InputValue[1]);
			// 	Player->GetImmersiveComponent()->SetAxisRight(InputValue[0]);
			// }
		}
	}
}

bool UIH_PlayerMove::CanMove_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value) const
{
	// Ensure that we DO NOT have any of the blocking tags and the value is past the minimum threshold.
	return !HasAnyTags(Manager, BlockMovementTags) && Value.GetMagnitude() >= MinimumMagnitudeToMove;
}
