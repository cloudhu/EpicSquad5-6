// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "LyraActivatableWidget.h"
#include "LyraHUDLayout.generated.h"

class UCommonActivatableWidget;
class UObject;
class ULyraControllerDisconnectedScreen;

/**
 * ULyraHUDLayout
 *
 *	Widget used to lay out the player's HUD (typically specified by an Add Widgets action in the experience)
 */
UCLASS(Abstract, BlueprintType, Blueprintable, Meta = (DisplayName = "HUD Layout", Category = "HUD"))
class ULyraHUDLayout : public ULyraActivatableWidget
{
	GENERATED_BODY()

public:

	ULyraHUDLayout(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
};
