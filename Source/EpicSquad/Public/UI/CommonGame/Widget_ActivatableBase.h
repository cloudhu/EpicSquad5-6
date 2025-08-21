// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UI/LyraActivatableWidget.h"
#include "Widget_ActivatableBase.generated.h"

class AEpicSquadPlayerController;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class EPICSQUAD_API UWidget_ActivatableBase : public ULyraActivatableWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintPure)
	AEpicSquadPlayerController* GetOwningFrontendPlayerController();

private:
	TWeakObjectPtr<AEpicSquadPlayerController> CachedOwningController;
};
