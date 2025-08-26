// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"

#include "FrontendPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class EPICSQUAD_API AFrontendPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	//~ Begin APlayerController Interface
	virtual void OnPossess(APawn* aPawn) override;
	//~ End APlayerController Interface
};
