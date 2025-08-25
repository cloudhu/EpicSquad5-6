// Copyright Epic Games, Inc. All Rights Reserved.

#include "Settings/Widgets/LyraSettingsListEntrySetting_KeyboardInput.h"

#include "CommonUIExtensions.h"
#include "NativeGameplayTags.h"
#include "Settings/CustomSettings/LyraSettingKeyboardInput.h"
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

	Button_PrimaryKey->OnClicked().AddUObject(this, &ThisClass::HandlePrimaryKeyClicked);
	Button_PrimaryKey->SetVisibility(ESlateVisibility::Collapsed);
	Button_SecondaryKey->OnClicked().AddUObject(this, &ThisClass::HandleSecondaryKeyClicked);
	Button_SecondaryKey->SetVisibility(ESlateVisibility::Collapsed);
	Button_ThirdKey->OnClicked().AddUObject(this, &ThisClass::HandleThirdKeyClicked);
	Button_ThirdKey->SetVisibility(ESlateVisibility::Collapsed);
	Button_FourthKey->OnClicked().AddUObject(this, &ThisClass::HandleFourthKeyClicked);
	Button_FourthKey->SetVisibility(ESlateVisibility::Collapsed);
	Button_Clear->OnClicked().AddUObject(this, &ThisClass::HandleClearClicked);
	Button_ResetToDefault->OnClicked().AddUObject(this, &ThisClass::HandleResetToDefaultClicked);
	Buttons.Add(Button_PrimaryKey);
	Buttons.Add(Button_SecondaryKey);
	Buttons.Add(Button_ThirdKey);
	Buttons.Add(Button_FourthKey);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandlePrimaryKeyClicked()
{
	UGameSettingPressAnyKey* PressAnyKeyPanel = CastChecked<UGameSettingPressAnyKey>(
		UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(), PressAnyKeyLayer, PressAnyKeyPanelClass));
	PressAnyKeyPanel->OnKeySelected.AddUObject(this, &ThisClass::HandlePrimaryKeySelected, PressAnyKeyPanel);
	PressAnyKeyPanel->OnKeySelectionCanceled.AddUObject(this, &ThisClass::HandleKeySelectionCanceled, PressAnyKeyPanel);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleSecondaryKeyClicked()
{
	UGameSettingPressAnyKey* PressAnyKeyPanel = CastChecked<UGameSettingPressAnyKey>(
		UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(), PressAnyKeyLayer, PressAnyKeyPanelClass));
	PressAnyKeyPanel->OnKeySelected.AddUObject(this, &ThisClass::HandleSecondaryKeySelected, PressAnyKeyPanel);
	PressAnyKeyPanel->OnKeySelectionCanceled.AddUObject(this, &ThisClass::HandleKeySelectionCanceled, PressAnyKeyPanel);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleThirdKeyClicked()
{
	UGameSettingPressAnyKey* PressAnyKeyPanel = CastChecked<UGameSettingPressAnyKey>(
		UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(), PressAnyKeyLayer, PressAnyKeyPanelClass));
	PressAnyKeyPanel->OnKeySelected.AddUObject(this, &ThisClass::HandleThirdKeySelected, PressAnyKeyPanel);
	PressAnyKeyPanel->OnKeySelectionCanceled.AddUObject(this, &ThisClass::HandleKeySelectionCanceled, PressAnyKeyPanel);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleFourthKeyClicked()
{
	UGameSettingPressAnyKey* PressAnyKeyPanel = CastChecked<UGameSettingPressAnyKey>(
		UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(), PressAnyKeyLayer, PressAnyKeyPanelClass));
	PressAnyKeyPanel->OnKeySelected.AddUObject(this, &ThisClass::HandleFourthKeySelected, PressAnyKeyPanel);
	PressAnyKeyPanel->OnKeySelectionCanceled.AddUObject(this, &ThisClass::HandleKeySelectionCanceled, PressAnyKeyPanel);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandlePrimaryKeySelected(FKey InKey, UGameSettingPressAnyKey* PressAnyKeyPanel)
{
	PressAnyKeyPanel->OnKeySelected.RemoveAll(this);
	ChangeBinding(0, InKey);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleSecondaryKeySelected(FKey InKey, UGameSettingPressAnyKey* PressAnyKeyPanel)
{
	PressAnyKeyPanel->OnKeySelected.RemoveAll(this);
	ChangeBinding(1, InKey);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleThirdKeySelected(FKey InKey, UGameSettingPressAnyKey* PressAnyKeyPanel)
{
	PressAnyKeyPanel->OnKeySelected.RemoveAll(this);
	ChangeBinding(2, InKey);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleFourthKeySelected(FKey InKey, UGameSettingPressAnyKey* PressAnyKeyPanel)
{
	PressAnyKeyPanel->OnKeySelected.RemoveAll(this);
	ChangeBinding(3, InKey);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandlePrimaryDuplicateKeySelected(FKey InKey, UKeyAlreadyBoundWarning* DuplicateKeyPressAnyKeyPanel) const
{
	DuplicateKeyPressAnyKeyPanel->OnKeySelected.RemoveAll(this);
	KeyboardInputSetting->ChangeBinding(0, OriginalKeyToBind);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleSecondaryDuplicateKeySelected(FKey InKey, UKeyAlreadyBoundWarning* DuplicateKeyPressAnyKeyPanel) const
{
	DuplicateKeyPressAnyKeyPanel->OnKeySelected.RemoveAll(this);
	KeyboardInputSetting->ChangeBinding(1, OriginalKeyToBind);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleThirdDuplicateKeySelected(FKey InKey, UKeyAlreadyBoundWarning* DuplicateKeyPressAnyKeyPanel) const
{
	DuplicateKeyPressAnyKeyPanel->OnKeySelected.RemoveAll(this);
	KeyboardInputSetting->ChangeBinding(2, OriginalKeyToBind);
}

void ULyraSettingsListEntrySetting_KeyboardInput::HandleFourthDuplicateKeySelected(FKey InKey, UKeyAlreadyBoundWarning* DuplicateKeyPressAnyKeyPanel) const
{
	DuplicateKeyPressAnyKeyPanel->OnKeySelected.RemoveAll(this);
	KeyboardInputSetting->ChangeBinding(3, OriginalKeyToBind);
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

		switch (InKeyBindSlot)
		{
		case 0:
			KeyAlreadyBoundWarning->OnKeySelected.AddUObject(this, &ThisClass::HandlePrimaryDuplicateKeySelected, KeyAlreadyBoundWarning);
			break;
		case 1:
			KeyAlreadyBoundWarning->OnKeySelected.AddUObject(this, &ThisClass::HandleSecondaryDuplicateKeySelected, KeyAlreadyBoundWarning);
			break;
		case 2:
			KeyAlreadyBoundWarning->OnKeySelected.AddUObject(this, &ThisClass::HandleThirdDuplicateKeySelected, KeyAlreadyBoundWarning);
			break;
		case 3:
			KeyAlreadyBoundWarning->OnKeySelected.AddUObject(this, &ThisClass::HandleFourthDuplicateKeySelected, KeyAlreadyBoundWarning);
			break;
		default: ;
		}
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
				if (KeyboardInputSetting->GetIconFromKey(FoundBrush, Pair.Value))
				{
					Buttons[Index]->SetButtonDisplayImage(FoundBrush);
				}
				else
				{
					Buttons[Index]->SetButtonText(Pair.Value.GetDisplayName());
				}
				Buttons[Index]->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				Buttons[Index]->SetVisibility(ESlateVisibility::Collapsed);
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
