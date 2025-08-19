// CloudHu:604746493@qq.com All Rights Reserved


#include "UI/CommonGame/PrimaryGameLayout.h"

#include "Kismet/GameplayStatics.h"

#include "UI/CommonGame/FrontendManagerSubsystem.h"

UPrimaryGameLayout* UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	const APlayerController* PlayerController = GameInstance->GetPrimaryPlayerController(false);
	return GetPrimaryGameLayout(PlayerController);
}

UPrimaryGameLayout* UPrimaryGameLayout::GetPrimaryGameLayout(const APlayerController* PlayerController)
{
	return PlayerController ? GetPrimaryGameLayout(Cast<ULocalPlayer>(PlayerController->Player)) : nullptr;
}

UPrimaryGameLayout* UPrimaryGameLayout::GetPrimaryGameLayout(const ULocalPlayer* LocalPlayer)
{
	if (LocalPlayer)
	{
		if (const UFrontendManagerSubsystem* UIManager = LocalPlayer->GetGameInstance()->GetSubsystem<UFrontendManagerSubsystem>())
		{
			if (UPrimaryGameLayout* RootLayout = UIManager->GetCreatedPrimaryLayout())
			{
				return RootLayout;
			}
		}
	}

	return nullptr;
}

UPrimaryGameLayout::UPrimaryGameLayout(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void UPrimaryGameLayout::SetIsDormant(bool InDormant)
{
	if (bIsDormant != InDormant)
	{
		const ULocalPlayer* Lp = GetOwningLocalPlayer();
		const int32 PlayerId = Lp ? Lp->GetControllerId() : -1;
		const TCHAR* OldDormancyStr = bIsDormant ? TEXT("Dormant") : TEXT("Not-Dormant");
		const TCHAR* NewDormancyStr = InDormant ? TEXT("Dormant") : TEXT("Not-Dormant");
		const TCHAR* PrimaryPlayerStr = Lp && Lp->IsPrimaryPlayer() ? TEXT("[Primary]") : TEXT("[Non-Primary]");
		UE_LOG(LogTemp, Display, TEXT("%s PrimaryGameLayout Dormancy changed for [%d] from [%s] to [%s]"), PrimaryPlayerStr, PlayerId, OldDormancyStr, NewDormancyStr);

		bIsDormant = InDormant;
		OnIsDormantChanged();
	}
}

void UPrimaryGameLayout::FindAndRemoveWidgetFromLayer(UWidget_ActivatableBase* ActivatableWidget)
{
	// We're not sure what layer the widget is on so go searching.
	for (const auto& LayerKvp : Layers)
	{
		LayerKvp.Value->RemoveWidget(*ActivatableWidget);
	}
}

UCommonActivatableWidgetContainerBase* UPrimaryGameLayout::GetLayerWidget(FGameplayTag LayerName) const
{
	return Layers.FindRef(LayerName);
}

void UPrimaryGameLayout::RegisterLayer(FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget)
{
	if (!IsDesignTime())
	{
		LayerWidget->OnTransitioningChanged.AddUObject(this, &UPrimaryGameLayout::OnWidgetStackTransitioning);
		// TODO: Consider allowing a transition duration, we currently set it to 0, because if it's not 0, the
		//       transition effect will cause focus to not transition properly to the new widgets when using
		//       gamepad always.
		//LayerWidget->SetTransitionDuration(0.2);

		Layers.Add(LayerTag, LayerWidget);
	}
}

void UPrimaryGameLayout::OnIsDormantChanged()
{
}

void UPrimaryGameLayout::OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning)
{
	if (bIsTransitioning)
	{
		const FName SuspendToken = UCommonUIExtensions::SuspendInputForPlayer(GetOwningLocalPlayer(), TEXT("GlobalStackTransion"));
		SuspendInputTokens.Add(SuspendToken);
	}
	else
	{
		if (ensure(SuspendInputTokens.Num() > 0))
		{
			const FName SuspendToken = SuspendInputTokens.Pop();
			UCommonUIExtensions::ResumeInputForPlayer(GetOwningLocalPlayer(), SuspendToken);
		}
	}
}
