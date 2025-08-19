// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/CommonGame/LyraUIMessaging.h"
#include "UI/Messaging/CommonGameDialog.h"
#include "UI/CommonGame/FrontendManagerSubsystem.h"
#include "UI/CommonGame/PrimaryGameLayout.h"
#include "NativeGameplayTags.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraUIMessaging)

class FSubsystemCollectionBase;
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MODAL, "UI.Layer.Modal");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_Widget_ConfirmScreen, "UI.Widget.ConfirmScreen");

void ULyraUIMessaging::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ConfirmationDialogClassPtr = UCommonUIExtensions::GetFrontendSoftWidgetClassByTag(TAG_UI_Widget_ConfirmScreen).LoadSynchronous();
}

void ULyraUIMessaging::ShowConfirmation(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
{
	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer<ULocalPlayer>())
	{
		if (const UFrontendManagerSubsystem* UIManager = LocalPlayer->GetGameInstance()->GetSubsystem<UFrontendManagerSubsystem>())
		{
			if (UPrimaryGameLayout* RootLayout = UIManager->GetCreatedPrimaryLayout())
			{
				RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(TAG_UI_LAYER_MODAL, ConfirmationDialogClassPtr, [DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog) {
					Dialog.SetupDialog(DialogDescriptor, ResultCallback);
				});
			}
		}
	}
}

void ULyraUIMessaging::ShowError(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
{
	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer<ULocalPlayer>())
	{
		if (const UFrontendManagerSubsystem* UIManager = LocalPlayer->GetGameInstance()->GetSubsystem<UFrontendManagerSubsystem>())
		{
			if (UPrimaryGameLayout* RootLayout = UIManager->GetCreatedPrimaryLayout())
			{
				RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(TAG_UI_LAYER_MODAL, ConfirmationDialogClassPtr, [DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog) {
					Dialog.SetupDialog(DialogDescriptor, ResultCallback);
				});
			}
		}
	}
}
