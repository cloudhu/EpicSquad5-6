// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraUIManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "PrimaryGameLayout.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraUIManagerSubsystem)

class FSubsystemCollectionBase;

ULyraUIManagerSubsystem::ULyraUIManagerSubsystem(): CreatedPrimaryLayout(nullptr)
{
}

ULyraUIManagerSubsystem* ULyraUIManagerSubsystem::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);

		return UGameInstance::GetSubsystem<ULyraUIManagerSubsystem>(World->GetGameInstance());
	}

	return nullptr;
}

void ULyraUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ULyraUIManagerSubsystem::Tick), 0.0f);
}

void ULyraUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	//FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);

	if (OnPrimaryLayoutLoaded.IsBound())
	{
		OnPrimaryLayoutLoaded.Broadcast(false);
		OnPrimaryLayoutLoaded.Clear();
	}
}

void ULyraUIManagerSubsystem::RegisterCreatedPrimaryLayoutWidget(UPrimaryGameLayout* InCreatedWidget)
{
	check(InCreatedWidget)
	CreatedPrimaryLayout = InCreatedWidget;
	if (OnPrimaryLayoutLoaded.IsBound())
	{
		OnPrimaryLayoutLoaded.Broadcast(true);
		OnPrimaryLayoutLoaded.Clear();
	}
}

void ULyraUIManagerSubsystem::CallOrRegister_OnLayoutLoaded_HighPriority(FOnLayoutLoaded::FDelegate&& Delegate)
{
	if (CreatedPrimaryLayout)
	{
		Delegate.Execute(true);
	}
	else
	{
		OnPrimaryLayoutLoaded.Add(MoveTemp(Delegate));
	}
}

// bool ULyraUIManagerSubsystem::Tick(float DeltaTime)
// {
// 	SyncRootLayoutVisibilityToShowHUD();
//
// 	return true;
// }
//
// void ULyraUIManagerSubsystem::SyncRootLayoutVisibilityToShowHUD()
// {
// 	if (const UGameUIPolicy* Policy = GetCurrentUIPolicy())
// 	{
// 		for (const ULocalPlayer* LocalPlayer : GetGameInstance()->GetLocalPlayers())
// 		{
// 			bool bShouldShowUI = true;
//
// 			if (const APlayerController* PC = LocalPlayer->GetPlayerController(GetWorld()))
// 			{
// 				if (const AHUD* HUD = PC->GetHUD(); HUD && !HUD->bShowHUD)
// 				{
// 					bShouldShowUI = false;
// 				}
// 			}
//
// 			if (UPrimaryGameLayout* RootLayout = Policy->GetRootLayout(CastChecked<UCommonLocalPlayer>(LocalPlayer)))
// 			{
// 				if (const ESlateVisibility DesiredVisibility = bShouldShowUI ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;
// 					DesiredVisibility != RootLayout->GetVisibility())
// 				{
// 					RootLayout->SetVisibility(DesiredVisibility);
// 				}
// 			}
// 		}
// 	}
// }
