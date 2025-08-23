// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraUIMessaging.h"

#include "Messaging/CommonGameDialog.h"
#include "NativeGameplayTags.h"
#include "CommonLocalPlayer.h"
#include "PrimaryGameLayout.h"
#include "System/LyraDevelopmentStatics.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraUIMessaging)

class FSubsystemCollectionBase;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MODAL, "UI.Layer.Modal");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_WIDGET_CONFIRMDIALOG, "UI.Widget.ConfirmationDialog");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_WIDGET_ERRORDIALOG, "UI.Widget.ErrorDialog");

void ULyraUIMessaging::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ConfirmationDialogClassPtr = ULyraDevelopmentStatics::GetSoftWidgetClassByTag(TAG_UI_WIDGET_CONFIRMDIALOG).LoadSynchronous();
	
	ErrorDialogClassPtr = ULyraDevelopmentStatics::GetSoftWidgetClassByTag(TAG_UI_WIDGET_ERRORDIALOG).LoadSynchronous();
	
}

void ULyraUIMessaging::ShowConfirmation(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
{
	checkf(ConfirmationDialogClassPtr.Get(),TEXT("Can not load ConfirmationDialogClassPtr in ULyraUIMessaging::Initialize"));
	if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(GetLocalPlayer()))
	{
		//EpicDebug::Print(ConfirmationDialogClassPtr.Get()?"True":"False");
		RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(
			TAG_UI_LAYER_MODAL, ConfirmationDialogClassPtr,
			[DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog)
			{
				Dialog.SetupDialog(DialogDescriptor, ResultCallback);
			});
	}
	else if (const UCommonLocalPlayer* LocalPlayer = GetLocalPlayer<UCommonLocalPlayer>())
	{
		if (UPrimaryGameLayout* Layout = LocalPlayer->GetRootUILayout())
		{
			Layout->PushWidgetToLayerStack<UCommonGameDialog>(
				TAG_UI_LAYER_MODAL, ConfirmationDialogClassPtr,
				[DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog)
				{
					Dialog.SetupDialog(DialogDescriptor, ResultCallback);
				});
		}
	}
}

void ULyraUIMessaging::ShowError(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback)
{
	checkf(ErrorDialogClassPtr.Get(),TEXT("Can not load ErrorDialogClassPtr in ULyraUIMessaging::Initialize"));
	if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(GetLocalPlayer()))
	{
		//EpicDebug::Print(ConfirmationDialogClassPtr.Get()?"True":"False");
		RootLayout->PushWidgetToLayerStack<UCommonGameDialog>(
			TAG_UI_LAYER_MODAL, ErrorDialogClassPtr,
			[DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog)
			{
				Dialog.SetupDialog(DialogDescriptor, ResultCallback);
			});
	}
	else  if (const UCommonLocalPlayer* LocalPlayer = GetLocalPlayer<UCommonLocalPlayer>())
	{
		if (UPrimaryGameLayout* Layout = LocalPlayer->GetRootUILayout())
		{
			Layout->PushWidgetToLayerStack<UCommonGameDialog>(
				TAG_UI_LAYER_MODAL, ErrorDialogClassPtr,
				[DialogDescriptor, ResultCallback](UCommonGameDialog& Dialog)
				{
					Dialog.SetupDialog(DialogDescriptor, ResultCallback);
				});
		}
	}
}
