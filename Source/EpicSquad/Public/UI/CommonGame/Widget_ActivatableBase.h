// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Widget_ActivatableBase.generated.h"

class AEpicSquadPlayerController;
/**
 * 
 */
UCLASS()
class EPICSQUAD_API UWidget_ActivatableBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintPure)
	AEpicSquadPlayerController* GetOwningFrontendPlayerController();

private:
	TWeakObjectPtr<AEpicSquadPlayerController> CachedOwningController;
	
};
