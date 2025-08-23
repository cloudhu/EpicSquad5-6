// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Messaging/CommonMessagingSubsystem.h"
#include "Templates/SubclassOf.h"

#include "LyraUIMessaging.generated.h"

class UCommonActivatableWidget;
class FSubsystemCollectionBase;
class UCommonGameDialog;
class UCommonGameDialogDescriptor;
class UObject;

/**
 * 
 */
UCLASS()
class EPICSQUAD_API ULyraUIMessaging : public UCommonMessagingSubsystem
{
	GENERATED_BODY()

public:
	ULyraUIMessaging() { }

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void ShowConfirmation(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback = FCommonMessagingResultDelegate()) override;
	virtual void ShowError(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback = FCommonMessagingResultDelegate()) override;

private:
	
	UPROPERTY()
	TSubclassOf<UCommonGameDialog> ConfirmationDialogClassPtr;

	UPROPERTY()
	TSubclassOf<UCommonGameDialog> ErrorDialogClassPtr;

	// UPROPERTY(config)
	// TSoftClassPtr<UCommonActivatableWidget> ConfirmationDialogClass;
	// Can not find where to config 
	// UPROPERTY(config)
	// TSoftClassPtr<UCommonGameDialog> ErrorDialogClass;
};
