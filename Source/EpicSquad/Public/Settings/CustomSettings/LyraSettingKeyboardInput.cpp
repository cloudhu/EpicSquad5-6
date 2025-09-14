// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraSettingKeyboardInput.h"

#include "CommonInputSubsystem.h"
#include "EnhancedInputSubsystems.h"

#include "Input/Settings/NinjaInputUserSettings.h"

#include "UI/Messaging/CommonGameDialog.h"
#include "UI/Subsystem/LyraUIMessaging.h"

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
						return Mapping.GetCurrentKey();
					}
				}
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Can not find key from MappingName: %s -- SlotID:%d ,Try to find by FMapPlayerKeyArgs"), *ActionMappingName.ToString(), InSlot)
	FMapPlayerKeyArgs KeyArgs;
	KeyArgs.MappingName = ActionMappingName;
	KeyArgs.Slot = InSlot;
	return CachedOwningKeyProfile->FindKeyMapping(KeyArgs)->GetCurrentKey();
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
	if (CachedOwningKeyProfile)
	{
		return CachedOwningKeyProfile->FindKeyMappingRow(ActionMappingName);
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

void ULyraSettingKeyboardInput::InitializeInputData(UNinjaInputUserSettings* InOwningInputUserSettings, UEnhancedPlayerMappableKeyProfile* KeyProfile,
							 const FPlayerKeyMapping& Mapping, const FPlayerMappableKeyQueryOptions& InQueryOptions)
{
	
	check(KeyProfile);
	CachedOwningInputUserSettings=InOwningInputUserSettings;
	CachedOwningKeyProfile=KeyProfile;
	//把数据ID和查询刷选条件缓存起来
	ProfileIdentifier = KeyProfile->GetProfileIdString();
	QueryOptions = InQueryOptions;
	//拿到按键映射名称，也缓存起来，把槽位和按键保存到初始映射集合中待用
	ActionMappingName = Mapping.GetMappingName();
	InitialKeyMappings.Add(Mapping.GetSlot(), Mapping.GetCurrentKey());
	//设置显示名
	if (const FText& MappingDisplayName = Mapping.GetDisplayName(); !MappingDisplayName.IsEmpty())
	{
		SetDisplayName(MappingDisplayName);
	}
	//如果匹配的按键是W，那么就是键盘输入，否则就是手柄输入，并根据硬件添加不同的前缀名称，可以避免重名
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
	//打印出来看看数据是否有问题
	UE_LOG(LogTemp, Warning, TEXT("NameString: %s"), *NameString);
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
	if (CachedOwningInputUserSettings)
	{
		FMapPlayerKeyArgs Args = {};
		Args.MappingName = ActionMappingName;

		FGameplayTagContainer FailureReason;
		CachedOwningInputUserSettings->ResetAllPlayerKeysInRow(Args, FailureReason);

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
					if (CachedDesiredInputKeyType == ECommonInputType::MouseAndKeyboard && Mapping.GetCurrentKey().IsGamepadKey())
					{
						continue;
					}

					if (CachedDesiredInputKeyType == ECommonInputType::Gamepad && !Mapping.GetCurrentKey().IsGamepadKey())
					{
						continue;
					}
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
	bool CanBind = false;
	FText WarningTitle = LOCTEXT("WarningKeyChangeBinding_Title", "Please use the Mouse And Keyboard to input!");

	if (CachedDesiredInputKeyType == ECommonInputType::MouseAndKeyboard)
	{
		//当前需要的是键盘输入，不接受手柄输入
		CanBind = !NewKey.IsGamepadKey();
	}
	else if (CachedDesiredInputKeyType == ECommonInputType::Gamepad)
	{
		CanBind = NewKey.IsGamepadKey();
		WarningTitle = LOCTEXT("WarningKeyChangeBinding_Title", "Please use the Gamepad to input!");
	}

	if (CanBind)
	{
		FMapPlayerKeyArgs Args = {};
		Args.MappingName = ActionMappingName;
		Args.Slot = static_cast<EPlayerMappableKeySlot>(InKeyBindSlot);
		Args.NewKey = NewKey;
		// If you want to, you can additionally specify this mapping to only be applied to a certain hardware device or key profile
		//Args.ProfileId =
		//Args.HardwareDeviceId =

		if (UEnhancedInputUserSettings* Settings = GetUserSettings())
		{
			FGameplayTagContainer FailureReason;
			Settings->MapPlayerKey(Args, FailureReason);
			StoreInitial();
			NotifySettingChanged(EGameSettingChangeReason::Change);
		}

		return true;
	}
	if (ULyraUIMessaging* Messaging = LocalPlayer->GetSubsystem<ULyraUIMessaging>())
	{
		Messaging->ShowConfirmation(
			UCommonGameDialogDescriptor::CreateConfirmationOk(
				WarningTitle,
				LOCTEXT("WarningKeyChangeBinding_Message", "You will need to use the corresponding input device to change binding.")
			)
		);
	}
	return CanBind;

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
