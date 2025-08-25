// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingValue.h"
#include "UserSettings/EnhancedInputUserSettings.h"

#include "LyraSettingKeyboardInput.generated.h"

enum class ECommonInputType : uint8;
class UObject;

//--------------------------------------
// ULyraSettingKeyboardInput
//--------------------------------------

UCLASS()
class EPICSQUAD_API ULyraSettingKeyboardInput : public UGameSettingValue
{
	GENERATED_BODY()

public:
	ULyraSettingKeyboardInput();

	void InitializeInputData(UEnhancedInputUserSettings* InOwningInputUserSettings, UEnhancedPlayerMappableKeyProfile* KeyProfile,
	                         const FKeyMappingRow& MappingData, const FPlayerMappableKeyQueryOptions& InQueryOptions);

	FText GetKeyTextFromSlot(const EPlayerMappableKeySlot InSlot) const;

	bool IsSlotValid(const EPlayerMappableKeySlot InSlot) const;

	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	bool ChangeBinding(int32 InKeyBindSlot, const FKey& NewKey);
	void GetAllMappedActionsFromKey(int32 InKeyBindSlot, const FKey& Key, TArray<FName>& OutActionNames) const;

	/** Returns true if mappings on this setting have been customized */
	bool IsMappingCustomized() const;

	FText GetSettingDisplayName() const;
	FText GetSettingDisplayCategory() const;
	const FKey& GetCurrentKey(const EPlayerMappableKeySlot InSlot) const;

	bool GetIconFromKey(FSlateBrush& OutBrush, const FKey& InKey) const;
	static FText GetKeyTextFromKey(const FKey& InKey);
	auto GetIconFromCurrentKey(FSlateBrush& OutBrush, const EPlayerMappableKeySlot InSlot) const -> bool;

	const FKeyMappingRow* FindKeyMappingRow() const;
	UEnhancedPlayerMappableKeyProfile* FindMappableKeyProfile() const;
	UEnhancedInputUserSettings* GetUserSettings() const;

	FORCEINLINE ECommonInputType GetDesiredInputKeyType() const { return CachedDesiredInputKeyType; }

	TMap<EPlayerMappableKeySlot, FKey> GetKeyMappings() const { return InitialKeyMappings; }

protected:
	/** ULyraSetting */
	virtual void OnInitialized() override;

	/** The name of this action's mappings */
	FName ActionMappingName;

	/** The query options to filter down keys on this setting for */
	FPlayerMappableKeyQueryOptions QueryOptions;

	/** The profile identifier that this key setting is from */
	FString ProfileIdentifier;

	/** Store the initial key mappings that are set on this for each slot */
	TMap<EPlayerMappableKeySlot, FKey> InitialKeyMappings;

	ECommonInputType CachedDesiredInputKeyType;

	UPROPERTY(Transient)
	UEnhancedInputUserSettings* CachedOwningInputUserSettings;

	UPROPERTY(Transient)
	UEnhancedPlayerMappableKeyProfile* CachedOwningKeyProfile;
};
