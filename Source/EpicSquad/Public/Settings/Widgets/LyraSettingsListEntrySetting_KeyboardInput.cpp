// Copyright Epic Games, Inc. All Rights Reserved.

#include "Settings/Widgets/LyraSettingsListEntrySetting_KeyboardInput.h"

#include "CommonUIExtensions.h"
#include "GameUIManagerSubsystem.h"
#include "NativeGameplayTags.h"
#include "PrimaryGameLayout.h"

#include "Settings/CustomSettings/LyraSettingKeyboardInput.h"

#include "UI/CommonGame/Widget_KeyRemapButtonBase.h"
#include "UI/Foundation/LyraButtonBase.h"
#include "Widgets/Misc/GameSettingPressAnyKey.h"
#include "Widgets/Misc/KeyAlreadyBoundWarning.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraSettingsListEntrySetting_KeyboardInput)

#define LOCTEXT_NAMESPACE "LyraSettings"

UE_DEFINE_GAMEPLAY_TAG_STATIC(PressAnyKeyLayer, "UI.Layer.Modal");

void ULyraSettingsListEntrySetting_KeyboardInput::SetSetting(UGameSetting* InSetting)
{
	KeyboardInputSetting = CastChecked<ULyraSettingKeyboardInput>(InSetting);

	Super::SetSetting(InSetting);

	Refresh();
}

void ULyraSettingsListEntrySetting_KeyboardInput::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Button_Clear->OnClicked().AddUObject(this, &ThisClass::HandleClearClicked);
	Button_ResetToDefault->OnClicked().AddUObject(this, &ThisClass::HandleResetToDefaultClicked);
	Buttons.Add(Button_PrimaryKey);
	Buttons.Add(Button_SecondaryKey);
	Buttons.Add(Button_ThirdKey);
	Buttons.Add(Button_FourthKey);
	Button_PrimaryKey->OnKeyRemapClicked.AddUObject(this, &ThisClass::HandleRemapKeyClicked);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleRemapKeyClicked(int32 SlotID)
{
	if (const UGameUIManagerSubsystem* UIManager = GetOwningLocalPlayer()->GetGameInstance()->GetSubsystem<UGameUIManagerSubsystem>())
	{
		if (UPrimaryGameLayout* Layout =UIManager->GetCreatedPrimaryLayout())
		{
			UGameSettingPressAnyKey* PressAnyKeyPanel =Layout->PushWidgetToLayerStack<UGameSettingPressAnyKey>(PressAnyKeyLayer, PressAnyKeyPanelClass,[this, SlotID](UGameSettingPressAnyKey& PressAnyKeyPanel)
			{
				PressAnyKeyPanel.SetKeyIndex(SlotID);
			});
			PressAnyKeyPanel->OnKeySelected.AddUObject(this, &ThisClass::HandleRemapKeySelected, PressAnyKeyPanel);
			PressAnyKeyPanel->OnKeySelectionCanceled.AddUObject(this, &ThisClass::HandleKeySelectionCanceled, PressAnyKeyPanel);
		}
	}
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleRemapKeySelected(FKey InKey, int32 SlotID, UGameSettingPressAnyKey* PressAnyKeyPanel)
{
	PressAnyKeyPanel->OnKeySelected.RemoveAll(this);
	ChangeBinding(SlotID, InKey);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleDuplicateKeySelected(FKey InKey, int32 SlotID, UKeyAlreadyBoundWarning* DuplicateKeyPressAnyKeyPanel) const
{
	DuplicateKeyPressAnyKeyPanel->OnKeySelected.RemoveAll(this);
	KeyboardInputSetting->ChangeBinding(SlotID, OriginalKeyToBind);
}

void ULyraSettingsListEntrySetting_KeyboardInput::ChangeBinding(const int32 InKeyBindSlot, const FKey& InKey)
{
	OriginalKeyToBind = InKey;
	TArray<FName> ActionsForKey;
	KeyboardInputSetting->GetAllMappedActionsFromKey(InKeyBindSlot, InKey, ActionsForKey);
	if (!ActionsForKey.IsEmpty())
	{
		UKeyAlreadyBoundWarning* KeyAlreadyBoundWarning = CastChecked<UKeyAlreadyBoundWarning>(
			UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(), PressAnyKeyLayer, KeyAlreadyBoundWarningPanelClass));

		FString ActionNames;
		for (FName ActionName : ActionsForKey)
		{
			ActionNames += ActionName.ToString() += ", ";
		}

		FFormatNamedArguments Args;
		Args.Add(TEXT("InKey"), InKey.GetDisplayName());
		Args.Add(TEXT("ActionNames"), FText::FromString(ActionNames));

		KeyAlreadyBoundWarning->SetWarningText(
			FText::Format(LOCTEXT("WarningText", "{InKey} is already bound to {ActionNames} are you sure you want to rebind it?"), Args));
		KeyAlreadyBoundWarning->SetCancelText(
			FText::Format(LOCTEXT("CancelText", "Press escape to cancel, or press {InKey} again to confirm rebinding."), Args));

		KeyAlreadyBoundWarning->OnKeySelected.AddUObject(this, &ThisClass::HandleDuplicateKeySelected, KeyAlreadyBoundWarning);

		KeyAlreadyBoundWarning->OnKeySelectionCanceled.AddUObject(this, &ThisClass::HandleKeySelectionCanceled, KeyAlreadyBoundWarning);
	}
	else
	{
		KeyboardInputSetting->ChangeBinding(InKeyBindSlot, InKey);
	}
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleKeySelectionCanceled(UGameSettingPressAnyKey* PressAnyKeyPanel) const
{
	PressAnyKeyPanel->OnKeySelectionCanceled.RemoveAll(this);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleKeySelectionCanceled(UKeyAlreadyBoundWarning* PressAnyKeyPanel) const
{
	PressAnyKeyPanel->OnKeySelectionCanceled.RemoveAll(this);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleClearClicked() const
{
	KeyboardInputSetting->ChangeBinding(0, EKeys::Invalid);
	KeyboardInputSetting->ChangeBinding(1, EKeys::Invalid);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleResetToDefaultClicked() const
{
	KeyboardInputSetting->ResetToDefault();
}

void ULyraSettingsListEntrySetting_KeyboardInput::OnSettingChanged()
{
	Refresh();
}

void ULyraSettingsListEntrySetting_KeyboardInput::Refresh() const
{
	if (ensure(KeyboardInputSetting))
	{
		Button_PrimaryKey->SetVisibility(ESlateVisibility::Collapsed);
		Button_SecondaryKey->SetVisibility(ESlateVisibility::Collapsed);
		Button_ThirdKey->SetVisibility(ESlateVisibility::Collapsed);
		Button_FourthKey->SetVisibility(ESlateVisibility::Collapsed);
		int Index = 0;
		FSlateBrush FoundBrush;
		for (TPair Pair : KeyboardInputSetting->GetKeyMappings())
		{
			if (Pair.Value.IsValid())
			{
				if (const FKey& Key=KeyboardInputSetting->GetCurrentKey(Pair.Key); KeyboardInputSetting->GetIconFromKey(FoundBrush, Key))
				{
					Buttons[Index]->SetButtonDisplayImage(FoundBrush);
				}
				else
				{
					Buttons[Index]->SetButtonText(Key.GetDisplayName());
				}
				Buttons[Index]->SetKeyIndex(static_cast<int32>(Pair.Key));
				Buttons[Index]->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				Buttons[Index]->SetVisibility(ESlateVisibility::Hidden);
			}
			Index++;
			if (Index > Buttons.Num()-1)
			{
				Index = 0;
			}
		}

		// Only display the reset to default button if a mapping is customized
		if (ensure(Button_ResetToDefault))
		{
			if (KeyboardInputSetting->IsMappingCustomized())
			{
				Button_ResetToDefault->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				Button_ResetToDefault->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void ULyraSettingsListEntrySetting_KeyboardInput::NativeOnEntryReleased()
{
	Super::NativeOnEntryReleased();

	KeyboardInputSetting = nullptr;
}

#undef LOCTEXT_NAMESPACE
