// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraSettingKeyboardInput.h"

#include "CommonInputSubsystem.h"
#include "EnhancedInputSubsystems.h"
#include "Player/LyraLocalPlayer.h"
#include "PlayerMappableInputConfig.h"
#include "UserSettings/EnhancedInputUserSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraSettingKeyboardInput)

#define LOCTEXT_NAMESPACE "LyraSettings"

namespace Lyra::ErrorMessages
{
	static const FText UnknownMappingName = LOCTEXT("LyraErrors_UnknownMappingName", "Unknown Mapping");
}


ULyraSettingKeyboardInput::ULyraSettingKeyboardInput(): CachedDesiredInputKeyType(ECommonInputType::MouseAndKeyboard), CachedOwningInputUserSettings(nullptr),
                                                        CachedOwningKeyProfile(nullptr)
{
	bReportAnalytics = false;
}

FText ULyraSettingKeyboardInput::GetSettingDisplayName() const
{
	if (const FKeyMappingRow* Row = FindKeyMappingRow())
	{
		if (Row->HasAnyMappings())
		{
			return Row->Mappings.begin()->GetDisplayName();
		}
	}

	return Lyra::ErrorMessages::UnknownMappingName;
}

FText ULyraSettingKeyboardInput::GetSettingDisplayCategory() const
{
	if (const FKeyMappingRow* Row = FindKeyMappingRow())
	{
		if (Row->HasAnyMappings())
		{
			return Row->Mappings.begin()->GetDisplayCategory();
		}
	}

	return Lyra::ErrorMessages::UnknownMappingName;
}

const FKey& ULyraSettingKeyboardInput::GetCurrentKey(const EPlayerMappableKeySlot InSlot) const
{
	if (InitialKeyMappings.Contains(InSlot))
	{
		return *InitialKeyMappings.Find(InSlot);
	}
	FMapPlayerKeyArgs KeyArgs;
	KeyArgs.MappingName = ActionMappingName;
	KeyArgs.Slot = InSlot;
	return CachedOwningKeyProfile->FindKeyMapping(KeyArgs)->GetCurrentKey();
	
	// if (CachedOwningKeyProfile)
	// {
	// 	FPlayerMappableKeyQueryOptions QueryOptionsForSlot = QueryOptions;
	// 	QueryOptionsForSlot.SlotToMatch = InSlot;
	//
	// 	// if (const FKeyMappingRow* Row = FindKeyMappingRow())
	// 	// {
	// 	// 	for (const FPlayerKeyMapping& Mapping : Row->Mappings)
	// 	// 	{
	// 	// 		if (CachedOwningKeyProfile->DoesMappingPassQueryOptions(Mapping, QueryOptionsForSlot))
	// 	// 		{
	// 	// 			return Mapping.GetCurrentKey();
	// 	// 		}
	// 	// 	}
	// 	// }
	//
	// }
	// return EKeys::Invalid;
}

bool ULyraSettingKeyboardInput::GetIconFromKey(FSlateBrush& OutBrush, const FKey& InKey) const
{
	const UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(LocalPlayer);

	checkf(CommonInputSubsystem, TEXT("ULyraSettingKeyboardInput::GetIconFromKey Can not Get UCommonInputSubsystem"));

	const bool bHasFoundBrush = UCommonInputPlatformSettings::Get()->TryGetInputBrush(
		OutBrush,
		InKey,
		CachedDesiredInputKeyType,
		CommonInputSubsystem->GetCurrentGamepadName()
	);

	return bHasFoundBrush;
}

FText ULyraSettingKeyboardInput::GetKeyTextFromKey(const FKey& InKey)
{
	return InKey.GetDisplayName();
}

bool ULyraSettingKeyboardInput::GetIconFromCurrentKey(FSlateBrush& OutBrush, const EPlayerMappableKeySlot InSlot) const
{
	const UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(LocalPlayer);

	check(CommonInputSubsystem);

	const bool bHasFoundBrush = UCommonInputPlatformSettings::Get()->TryGetInputBrush(
		OutBrush,
		GetCurrentKey(InSlot),
		CachedDesiredInputKeyType,
		CommonInputSubsystem->GetCurrentGamepadName()
	);


	return bHasFoundBrush;
}

const FKeyMappingRow* ULyraSettingKeyboardInput::FindKeyMappingRow() const
{
	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{
		return Profile->FindKeyMappingRow(ActionMappingName);
	}

	ensure(false);
	return nullptr;
}

UEnhancedPlayerMappableKeyProfile* ULyraSettingKeyboardInput::FindMappableKeyProfile() const
{
	return CachedOwningKeyProfile;
}

UEnhancedInputUserSettings* ULyraSettingKeyboardInput::GetUserSettings() const
{
	return CachedOwningInputUserSettings;
}

void ULyraSettingKeyboardInput::OnInitialized()
{
	DynamicDetails = FGetGameSettingsDetails::CreateLambda([this](ULocalPlayer&)
	{
		if (const FKeyMappingRow* Row = FindKeyMappingRow())
		{
			if (Row->HasAnyMappings())
			{
				return FText::Format(LOCTEXT("DynamicDetails_KeyboardInputAction", "Bindings for {0}"), Row->Mappings.begin()->GetDisplayName());
			}
		}
		return FText::GetEmpty();
	});

	Super::OnInitialized();
}

void ULyraSettingKeyboardInput::InitializeInputData(UEnhancedInputUserSettings* InOwningInputUserSettings, UEnhancedPlayerMappableKeyProfile* KeyProfile,
                                                    const FKeyMappingRow& MappingData,
                                                    const FPlayerMappableKeyQueryOptions& InQueryOptions)
{
	check(KeyProfile);

	//ProfileIdentifier = KeyProfile->GetProfileIdString();
	QueryOptions = InQueryOptions;
	CachedOwningInputUserSettings = InOwningInputUserSettings;
	CachedOwningKeyProfile = KeyProfile;
	for (const FPlayerKeyMapping& Mapping : MappingData.Mappings)
	{
		// Only add mappings that pass the query filters that have been provided upon creation
		if (!KeyProfile->DoesMappingPassQueryOptions(Mapping, QueryOptions))
		{
			continue;
		}
		const int SlotID=static_cast<int>(Mapping.GetSlot());

		UE_LOG(LogTemp, Log, TEXT("MappingDisplayName: %s -- SlotID: %d"),*Mapping.GetDisplayName().ToString(), SlotID);
		ActionMappingName = Mapping.GetMappingName();
		InitialKeyMappings.Add(Mapping.GetSlot(), Mapping.GetCurrentKey());

		if (const FText& MappingDisplayName = Mapping.GetDisplayName(); !MappingDisplayName.IsEmpty())
		{
			SetDisplayName(MappingDisplayName);
		}
	}
	FString NameString = TEXT("KBM_Input_") + ActionMappingName.ToString();
	if (InQueryOptions.KeyToMatch == EKeys::W)
	{
		CachedDesiredInputKeyType = ECommonInputType::MouseAndKeyboard;
	}
	else
	{
		CachedDesiredInputKeyType = ECommonInputType::Gamepad;
		NameString = TEXT("GAMEPAD_Input_") + ActionMappingName.ToString();
	}

	//EpicDebug::Print(TEXT("ULyraSettingKeyboardInput::InitializeInputData NameString :") + NameString);
	SetDevName(*NameString);
}

FText ULyraSettingKeyboardInput::GetKeyTextFromSlot(const EPlayerMappableKeySlot InSlot) const
{
	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{
		FPlayerMappableKeyQueryOptions QueryOptionsForSlot = QueryOptions;
		QueryOptionsForSlot.SlotToMatch = InSlot;

		if (const FKeyMappingRow* Row = FindKeyMappingRow())
		{
			for (const FPlayerKeyMapping& Mapping : Row->Mappings)
			{
				if (Profile->DoesMappingPassQueryOptions(Mapping, QueryOptionsForSlot))
				{
					return Mapping.GetCurrentKey().GetDisplayName();
				}
			}
		}
	}

	return EKeys::Invalid.GetDisplayName();
}

bool ULyraSettingKeyboardInput::IsSlotValid(const EPlayerMappableKeySlot InSlot) const
{
	if (InitialKeyMappings.Contains(InSlot))
	{
		return InitialKeyMappings.Find(InSlot)->IsValid();
	}
	if (CachedOwningKeyProfile)
	{
		FPlayerMappableKeyQueryOptions QueryOptionsForSlot = QueryOptions;
		QueryOptionsForSlot.SlotToMatch = InSlot;

		if (const FKeyMappingRow* Row = FindKeyMappingRow())
		{
			for (const FPlayerKeyMapping& Mapping : Row->Mappings)
			{
				if (CachedOwningKeyProfile->DoesMappingPassQueryOptions(Mapping, QueryOptionsForSlot))
				{
					return Mapping.IsValid();
				}
			}
		}
	}
	return false;
}

void ULyraSettingKeyboardInput::ResetToDefault()
{
	if (UEnhancedInputUserSettings* Settings = GetUserSettings())
	{
		FMapPlayerKeyArgs Args = {};
		Args.MappingName = ActionMappingName;

		FGameplayTagContainer FailureReason;
		Settings->ResetAllPlayerKeysInRow(Args, FailureReason);

		NotifySettingChanged(EGameSettingChangeReason::Change);
	}
}

void ULyraSettingKeyboardInput::StoreInitial()
{
	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{
		if (const FKeyMappingRow* Row = FindKeyMappingRow())
		{
			for (const FPlayerKeyMapping& Mapping : Row->Mappings)
			{
				if (Profile->DoesMappingPassQueryOptions(Mapping, QueryOptions))
				{
					ActionMappingName = Mapping.GetMappingName();
					InitialKeyMappings.Add(Mapping.GetSlot(), Mapping.GetCurrentKey());
				}
			}
		}
	}
}

void ULyraSettingKeyboardInput::RestoreToInitial()
{
	for (TPair Pair : InitialKeyMappings)
	{
		ChangeBinding(static_cast<int32>(Pair.Key), Pair.Value);
	}
}

bool ULyraSettingKeyboardInput::ChangeBinding(const int32 InKeyBindSlot, const FKey& NewKey)
{
	if (!NewKey.IsGamepadKey())
	{
		FMapPlayerKeyArgs Args = {};
		Args.MappingName = ActionMappingName;
		Args.Slot = static_cast<EPlayerMappableKeySlot>(static_cast<uint8>(InKeyBindSlot));
		Args.NewKey = NewKey;
		// If you want to, you can additionally specify this mapping to only be applied to a certain hardware device or key profile
		//Args.ProfileId =
		//Args.HardwareDeviceId =

		if (UEnhancedInputUserSettings* Settings = GetUserSettings())
		{
			FGameplayTagContainer FailureReason;
			Settings->MapPlayerKey(Args, FailureReason);
			NotifySettingChanged(EGameSettingChangeReason::Change);
		}

		return true;
	}

	return false;
}

void ULyraSettingKeyboardInput::GetAllMappedActionsFromKey(int32 InKeyBindSlot, const FKey& Key, TArray<FName>& OutActionNames) const
{
	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{
		Profile->GetMappingNamesForKey(Key, OutActionNames);
	}
}

bool ULyraSettingKeyboardInput::IsMappingCustomized() const
{
	bool bResult = false;

	if (const UEnhancedPlayerMappableKeyProfile* Profile = FindMappableKeyProfile())
	{
		const FPlayerMappableKeyQueryOptions QueryOptionsForSlot = QueryOptions;

		if (const FKeyMappingRow* Row = FindKeyMappingRow())
		{
			for (const FPlayerKeyMapping& Mapping : Row->Mappings)
			{
				if (Profile->DoesMappingPassQueryOptions(Mapping, QueryOptionsForSlot))
				{
					bResult |= Mapping.IsCustomized();
				}
			}
		}
	}

	return bResult;
}

#undef LOCTEXT_NAMESPACE
