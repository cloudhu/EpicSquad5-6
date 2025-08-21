// CloudHu:604746493@qq.com All Rights Reserved


#include "UI/CommonGame/Widget_ActivatableBase.h"
#include "Player/EpicSquadPlayerController.h"

AEpicSquadPlayerController* UWidget_ActivatableBase::GetOwningFrontendPlayerController()
{
	if (!CachedOwningController.IsValid())
	{
		CachedOwningController = GetOwningPlayer<AEpicSquadPlayerController>();
	}

	return CachedOwningController.IsValid()? CachedOwningController.Get() : nullptr;
}
