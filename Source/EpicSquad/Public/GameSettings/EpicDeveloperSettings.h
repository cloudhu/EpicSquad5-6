// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "Engine/DeveloperSettings.h"
#include "EpicDeveloperSettings.generated.h"

class UCommonActivatableWidget;
/**
 * 
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "UI Settings"))
class EPICSQUAD_API UEpicDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Widget Reference", meta = (ForceInlineRow, Categories = "UI.Widget"))
	TMap< FGameplayTag,TSoftClassPtr<UCommonActivatableWidget> > FrontendWidgetMap;

	UPROPERTY(Config, EditAnywhere, Category = "UI Image Reference", meta = (ForceInlineRow, Categories = "UI.Image"))
	TMap< FGameplayTag, TSoftObjectPtr<UTexture2D> > ScreenSoftImageMap;

	UPROPERTY(Config, EditAnywhere, Category = "UI Image Reference", meta = (ForceInlineRow, Categories = "UI.Image"))
	TMap< FGameplayTag, TSoftObjectPtr<UMaterialInstance> > SoftMaterialInstanceMap;
	
};
