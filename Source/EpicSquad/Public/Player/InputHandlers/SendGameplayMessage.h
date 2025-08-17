// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "NinjaInputHandler.h"
#include "SendGameplayMessage.generated.h"

/**
 * 
 */
UCLASS()
class EPICSQUAD_API USendGameplayMessage : public UNinjaInputHandler
{
	GENERATED_BODY()

public:
	USendGameplayMessage();

	virtual void HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager,
	const FInputActionValue& Value, const UInputAction* InputAction, float ElapsedTime) const override;

	/** Value to override the default magnitude. By default, the Input Value magnitude is used. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Event")
	float Magnitude=0.f;
protected:

	/** Gameplay Tag representing the event to broadcast. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Event")
	FGameplayTag EventTag;
};
