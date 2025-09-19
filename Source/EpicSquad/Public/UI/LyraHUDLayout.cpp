// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraHUDLayout.h"
#include "GameFramework/InputSettings.h"
#include "NativeGameplayTags.h"
#include "UI/Foundation/LyraControllerDisconnectedScreen.h"
#include "UI/LyraActivatableWidget.h"

#include "CommonGame/CommonUIExtensions.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraHUDLayout)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MENU, "UI.Layer.Menu");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_ACTION_ESCAPE, "UI.Action.Escape");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_Input_PrimarlyController, "Platform.Trait.Input.PrimaryController");

ULyraHUDLayout::ULyraHUDLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void ULyraHUDLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
}

void ULyraHUDLayout::NativeDestruct()
{
	Super::NativeDestruct();
	
}
