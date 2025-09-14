// Copyright Epic Games, Inc. All Rights Reserved.


#include "UI/CommonGame/PrimaryGameLayout.h"

#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

#include "UI/Subsystem/LyraUIManagerSubsystem.h"

#include "Widgets/CommonActivatableWidgetContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PrimaryGameLayout)

class UObject;

/*static*/ UPrimaryGameLayout* UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(const UObject* WorldContextObject)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	checkf(GameInstance,TEXT("Can not get GameInstance UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer"));
	if (const ULyraUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<ULyraUIManagerSubsystem>())
	{
		if (UPrimaryGameLayout* RootLayout = UIManager->GetCreatedPrimaryLayout())
		{
			return RootLayout;
		}
	}
	APlayerController* PlayerController = GameInstance->GetPrimaryPlayerController(false);
	return GetPrimaryGameLayout(PlayerController);
}

/*static*/ UPrimaryGameLayout* UPrimaryGameLayout::GetPrimaryGameLayout(const APlayerController* PlayerController)
{
	checkf(PlayerController,TEXT("Can not get PlayerController UPrimaryGameLayout::GetPrimaryGameLayout"));
	if (const ULyraUIManagerSubsystem* UIManager = ULyraUIManagerSubsystem::Get(PlayerController))
	{
		if (UPrimaryGameLayout* RootLayout = UIManager->GetCreatedPrimaryLayout())
		{
			return RootLayout;
		}
	}
	return PlayerController ? GetPrimaryGameLayout(Cast<ULocalPlayer>(PlayerController->Player)) : nullptr;
}

/*static*/ UPrimaryGameLayout* UPrimaryGameLayout::GetPrimaryGameLayout(const ULocalPlayer* LocalPlayer)
{
	if (LocalPlayer)
	{
		if (const UGameInstance* GameInstance = LocalPlayer->GetGameInstance())
		{
			if (const ULyraUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<ULyraUIManagerSubsystem>())
			{
				if (UPrimaryGameLayout* RootLayout = UIManager->GetCreatedPrimaryLayout())
				{
					return RootLayout;
				}
			}
		}
	}

	return nullptr;
}

UPrimaryGameLayout::UPrimaryGameLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPrimaryGameLayout::SetIsDormant(const bool Dormant)
{
	if (bIsDormant != Dormant)
	{
		const ULocalPlayer* Lp = GetOwningLocalPlayer();
		const int32 PlayerId = Lp ? Lp->GetControllerId() : -1;
		const TCHAR* OldDormancyStr = bIsDormant ? TEXT("Dormant") : TEXT("Not-Dormant");
		const TCHAR* NewDormancyStr = Dormant ? TEXT("Dormant") : TEXT("Not-Dormant");
		const TCHAR* PrimaryPlayerStr = Lp && Lp->IsPrimaryPlayer() ? TEXT("[Primary]") : TEXT("[Non-Primary]");
		UE_LOG(LogTemp, Display, TEXT("%s PrimaryGameLayout Dormancy changed for [%d] from [%s] to [%s]"), PrimaryPlayerStr, PlayerId, OldDormancyStr, NewDormancyStr);

		bIsDormant = Dormant;
		OnIsDormantChanged();
	}
}

void UPrimaryGameLayout::OnIsDormantChanged()
{
	//@TODO NDarnell Determine what to do with dormancy, in the past we treated dormancy as a way to shutoff rendering
	//and the view for the other local players when we force multiple players to use the player view of a single player.
	
	//if (UCommonLocalPlayer* LocalPlayer = GetOwningLocalPlayer<UCommonLocalPlayer>())
	//{
	//	// When the root layout is dormant, we don't want to render anything from the owner's view either
	//	LocalPlayer->SetIsPlayerViewEnabled(!bIsDormant);
	//}

	//SetVisibility(bIsDormant ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);

	//OnLayoutDormancyChanged().Broadcast(bIsDormant);
}

void UPrimaryGameLayout::RegisterLayer(FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget)
{
	if (!IsDesignTime())
	{
		LayerWidget->OnTransitioningChanged.AddUObject(this, &UPrimaryGameLayout::OnWidgetStackTransitioning);
		// TODO: Consider allowing a transition duration, we currently set it to 0, because if it's not 0, the
		//       transition effect will cause focus to not transition properly to the new widgets when using
		//       gamepad always.
		LayerWidget->SetTransitionDuration(0.0);

		Layers.Add(LayerTag, LayerWidget);
	}
}

void UPrimaryGameLayout::OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, const bool bIsTransitioning)
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

void UPrimaryGameLayout::FindAndRemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget)
{
	// We're not sure what layer the widget is on so go searching.
	for (const auto& LayerKvp : Layers)
	{
		LayerKvp.Value->RemoveWidget(*ActivatableWidget);
	}
}

UCommonActivatableWidgetContainerBase* UPrimaryGameLayout::GetLayerWidget(const FGameplayTag LayerName) const
{
	return Layers.FindRef(LayerName);
}
