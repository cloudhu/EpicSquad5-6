// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ImmersiveComponentBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputValueChangeDelegate, float, Delta);

UCLASS(Blueprintable, ClassGroup=(Custom), DisplayName="Immersive Interaction Component", meta=(BlueprintSpawnableComponent))
class EPICSQUAD_API UImmersiveComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UImmersiveComponentBase();

	UFUNCTION(BlueprintCallable,BlueprintPure,BlueprintGetter)
	FORCEINLINE float GetAxisForward() const { return AxisForward;} 
	
	UFUNCTION(BlueprintCallable,BlueprintPure,BlueprintGetter)
	FORCEINLINE float GetAxisRight() const { return AxisRight;} 
	
	UPROPERTY(BlueprintAssignable)
	FOnInputValueChangeDelegate OnInputValueChangeAxisForward;

	UPROPERTY(BlueprintAssignable)
	FOnInputValueChangeDelegate OnInputValueChangeAxisRight;

	void SetAxisForward(float InAxisForward);
	
	void SetAxisRight(float InAxisRight);
protected:
	UPROPERTY(Transient,BlueprintReadWrite,Category=Immersive)
	float AxisForward=0.f;

	UPROPERTY(Transient,BlueprintReadWrite,Category=Immersive)
	float AxisRight=0.f;
		
};
