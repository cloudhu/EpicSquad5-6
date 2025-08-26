// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonActivatableWidget.h"

#include "GameSettingPressAnyKey.generated.h"

#define UE_API GAMESETTINGS_API

struct FKey;

class UObject;

/**
 * @class UGameSettingPressAnyKey
 * @brief An abstract class for handling key selection in game settings, derived from UCommonActivatableWidget.
 *
 * This class provides a framework for allowing the user to press any key and then responding to that input. It includes
 * event dispatchers for when a key is selected or when the key selection is canceled. The class is designed to be used
 * in scenarios where a game setting requires the user to remap a key.
 */
UCLASS(MinimalAPI, Abstract)
class UGameSettingPressAnyKey : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UE_API UGameSettingPressAnyKey(const FObjectInitializer& Initializer);

	DECLARE_EVENT_TwoParams(UGameSettingPressAnyKey, FOnKeySelected, FKey, int32);

	FOnKeySelected OnKeySelected;

	DECLARE_EVENT(UGameSettingPressAnyKey, FOnKeySelectionCanceled);

	FOnKeySelectionCanceled OnKeySelectionCanceled;

	UE_API void SetKeyIndex(const int32 InKeyIndex);

protected:
	UE_API virtual void NativeOnActivated() override;
	UE_API virtual void NativeOnDeactivated() override;

	UE_API void HandleKeySelected(FKey InKey);
	UE_API void HandleKeySelectionCanceled();

	UE_API void Dismiss(TFunction<void()> PostDismissCallback);

private:
	bool bKeySelected = false;
	int32 KeyIndex = 0;
	TSharedPtr<class FSettingsPressAnyKeyInputPreProcessor> InputProcessor;
};

#undef UE_API
