// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Actions/AsyncAction_PushContentToLayerForPlayer.h"

#include "Engine/Engine.h"
#include "UI/CommonGame/PrimaryGameLayout.h"
#include "UObject/Stack.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_PushContentToLayerForPlayer)

UAsyncAction_PushContentToLayerForPlayer::UAsyncAction_PushContentToLayerForPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAsyncAction_PushContentToLayerForPlayer* UAsyncAction_PushContentToLayerForPlayer::PushContentToLayerForPlayer(
	APlayerController* InOwningPlayer, TSoftClassPtr<UWidget_ActivatableBase> InWidgetClass, FGameplayTag InLayerName, bool bSuspendInputUntilComplete)
{
	if (InWidgetClass.IsNull())
	{
		FFrame::KismetExecutionMessage(TEXT("PushContentToLayerForPlayer was passed a null WidgetClass"), ELogVerbosity::Error);
		return nullptr;
	}

	if (UWorld* World = GEngine->GetWorldFromContextObject(InOwningPlayer, EGetWorldErrorMode::LogAndReturnNull))
	{
		UAsyncAction_PushContentToLayerForPlayer* Action = NewObject<UAsyncAction_PushContentToLayerForPlayer>();
		Action->WidgetClass = InWidgetClass;
		Action->OwningPlayerPtr = InOwningPlayer;
		Action->LayerName = InLayerName;
		Action->bSuspendInputUntilComplete = bSuspendInputUntilComplete;
		Action->RegisterWithGameInstance(World);

		return Action;
	}

	return nullptr;
}

void UAsyncAction_PushContentToLayerForPlayer::Cancel()
{
	Super::Cancel();

	if (StreamingHandle.IsValid())
	{
		StreamingHandle->CancelHandle();
		StreamingHandle.Reset();
	}
}

void UAsyncAction_PushContentToLayerForPlayer::Activate()
{
	if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayout(OwningPlayerPtr.Get()))
	{
		TWeakObjectPtr<UAsyncAction_PushContentToLayerForPlayer> WeakThis = this;
		StreamingHandle = RootLayout->PushWidgetToLayerStackAsync<UWidget_ActivatableBase>(
			LayerName, bSuspendInputUntilComplete, WidgetClass,
			[this, WeakThis](
			const EAsyncWidgetLayerState State, UWidget_ActivatableBase* Widget)
			{
				if (WeakThis.IsValid())
				{
					switch (State)
					{
					case EAsyncWidgetLayerState::Initialize:
						BeforePush.Broadcast(Widget);
						break;
					case EAsyncWidgetLayerState::AfterPush:
						if (UWidget* WidgetToFocus = Widget->
							GetDesiredFocusTarget())
						{
							WidgetToFocus->SetFocus();
						}
						AfterPush.Broadcast(Widget);
						SetReadyToDestroy();
						break;
					case EAsyncWidgetLayerState::Canceled:
						SetReadyToDestroy();
						break;
					}
				}
				SetReadyToDestroy();
			});
	}
	else
	{
		SetReadyToDestroy();
	}
}
