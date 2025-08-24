// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameUIManagerSubsystem.h"

#include "LyraUIManagerSubsystem.generated.h"

class FSubsystemCollectionBase;
class UObject;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLayoutLoaded, const bool /*bSuccess*/);
UCLASS()
class EPICSQUAD_API ULyraUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()

public:

	ULyraUIManagerSubsystem();
	static ULyraUIManagerSubsystem* Get(const UObject* WorldContextObject);
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable)
	void RegisterCreatedPrimaryLayoutWidget(UPrimaryGameLayout* InCreatedWidget);

	FORCEINLINE virtual UPrimaryGameLayout* GetCreatedPrimaryLayout() const override { return CreatedPrimaryLayout; }
	
	void CallOrRegister_OnLayoutLoaded_HighPriority(FOnLayoutLoaded::FDelegate&& Delegate);
private:
	// bool Tick(float DeltaTime);
	// void SyncRootLayoutVisibilityToShowHUD();
	
	//FTSTicker::FDelegateHandle TickHandle;

	FOnLayoutLoaded OnPrimaryLayoutLoaded;

	UPROPERTY(Transient)
	UPrimaryGameLayout* CreatedPrimaryLayout;
};
