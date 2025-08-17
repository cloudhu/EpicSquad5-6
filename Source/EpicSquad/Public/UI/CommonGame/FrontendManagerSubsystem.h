// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FrontendManagerSubsystem.generated.h"

class UPrimaryGameLayout;
/**
* This manager is intended to be replaced by whatever your game needs to actually create, so this class is abstract to prevent it from being created.
If you just need the basic functionality you will start by sublcassing this subsystem in your own game.
 */
UCLASS()
class EPICSQUAD_API UFrontendManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	 static UFrontendManagerSubsystem* Get(const UObject* WorldContextObject);

	//~ Begin USubsystem Interface
	 virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~ End USubsystem Interface

	UFUNCTION(BlueprintCallable)
	 void RegisterCreatedPrimaryLayoutWidget(UPrimaryGameLayout* InCreatedWidget);

	 FORCEINLINE UPrimaryGameLayout* GetCreatedPrimaryLayout() const { return CreatedPrimaryLayout; }

private:
	UPROPERTY(Transient)
	UPrimaryGameLayout* CreatedPrimaryLayout;
};
