// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "NinjaInputHandler.h"
#include "PlayerJump.generated.h"

/**
* Replicates the original Jump setup for Character owners.
Avoid mixing default Character actions, such as this one, when you are using the Gameplay Ability System as it provides a Jump Gameplay Ability that can be used instead.
 */
UCLASS(meta = (DisplayName = "Player: Jump"))
class EPICSQUAD_API UPlayerJump : public UNinjaInputHandler
{
	GENERATED_BODY()

	UPlayerJump();

protected:

	/**
	 * Minimum magnitude required for the jump to be accepted.
	 * This is only relevant for analog input from a gamepad.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump")
	float MinimumMagnitudeToJump;

	/** Gameplay tags that, if present, will block the ability of jumping. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump")
	FGameplayTagContainer BlockJumpTags;

	/**
	 * Checks if the character is allowed to jump.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Input Handler|Jump")	
	bool CanJump(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value) const;

	/**
	 * Tries to execute a traversal jump action, based on the provided values.
	 * A successful attempt blocks the default jump logic.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Input Handler|Jump")
	bool TryTraversalJumpAction(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction, ETriggerEvent TriggerEvent) const;
	
	virtual void HandleStartedEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction) const override;	
	
	virtual void HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction, float ElapsedTime) const override;

	virtual void HandleCompletedEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
		const UInputAction* InputAction, float ElapsedTime) const override;
};
