// CloudHu:604746493@qq.com All Rights Reserved


#include "UI/CommonGame/Widget_ActivatableBase.h"
#include "Player/EpicSquadPlayerController.h"

TOptional<FUIInputConfig> UWidget_ActivatableBase::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case ELyraWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case ELyraWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case ELyraWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	case ELyraWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}

AEpicSquadPlayerController* UWidget_ActivatableBase::GetOwningFrontendPlayerController()
{
	if (!CachedOwningController.IsValid())
	{
		CachedOwningController = GetOwningPlayer<AEpicSquadPlayerController>();
	}

	return CachedOwningController.IsValid()? CachedOwningController.Get() : nullptr;
}
