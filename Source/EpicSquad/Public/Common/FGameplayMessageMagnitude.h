// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FGameplayMessageMagnitude.generated.h"

/**
 * Payload for Gameplay Message.
 */
USTRUCT(BlueprintType)
struct FGameplayMessageMagnitude
{
	
	GENERATED_BODY()

	
	/** Time when the loot was created. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayMessage")
	float Magnitude = 0.f;

	
	FGameplayMessageMagnitude();
	
};
