// CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonUIExtensions.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Widget_ActivatableBase.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "PrimaryGameLayout.generated.h"

/**
 * The state of an async load operation for the UI.
 */
enum class EAsyncWidgetLayerState : uint8
{
	Canceled,
	Initialize,
	AfterPush
};

/**
 * The primary game UI layout of your game. This widget class represents how to layout, push and display all layers of the UI for a single player. Each player in a split-screen game will receive their own primary game layout.
 */
UCLASS(MinimalAPI, Abstract, meta = (DisableNativeTick))
class UPrimaryGameLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	static EPICSQUAD_API UPrimaryGameLayout* GetPrimaryGameLayoutForPrimaryPlayer(const UObject* WorldContextObject);
	static EPICSQUAD_API UPrimaryGameLayout* GetPrimaryGameLayout(const APlayerController* PlayerController);
	static EPICSQUAD_API UPrimaryGameLayout* GetPrimaryGameLayout(const ULocalPlayer* LocalPlayer);

public:
	EPICSQUAD_API UPrimaryGameLayout(const FObjectInitializer& ObjectInitializer);

	/** A dormant root layout is collapsed and responds only to persistent actions registered by the owning player */
	EPICSQUAD_API void SetIsDormant(bool InDormant);

	EPICSQUAD_API bool IsDormant() const { return bIsDormant; }

public:
	template <typename ActivatableWidgetT = UWidget_ActivatableBase>
	TSharedPtr<FStreamableHandle> PushWidgetToLayerStackAsync(FGameplayTag LayerName, bool bSuspendInputUntilComplete,
	                                                          TSoftClassPtr<UWidget_ActivatableBase> ActivatableWidgetClass)
	{
		return PushWidgetToLayerStackAsync<ActivatableWidgetT>(LayerName, bSuspendInputUntilComplete, ActivatableWidgetClass,
		                                                       [](EAsyncWidgetLayerState, ActivatableWidgetT*)
		                                                       {
		                                                       });
	}

	template <typename ActivatableWidgetT = UWidget_ActivatableBase>
	TSharedPtr<FStreamableHandle> PushWidgetToLayerStackAsync(FGameplayTag LayerName, bool bSuspendInputUntilComplete,
	                                                          TSoftClassPtr<UWidget_ActivatableBase> ActivatableWidgetClass,
	                                                          TFunction<void(EAsyncWidgetLayerState, ActivatableWidgetT*)> StateFunc)
	{
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UWidget_ActivatableBase>::IsDerived, "Only Widget_ActivatableBase can be used here");

		static FName Name_PushingWidgetToLayer("PushingWidgetToLayer");
		const FName SuspendInputToken = bSuspendInputUntilComplete
			                                ? UCommonUIExtensions::SuspendInputForPlayer(GetOwningPlayer(), Name_PushingWidgetToLayer)
			                                : NAME_None;

		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(
			ActivatableWidgetClass.ToSoftObjectPath(),
			FStreamableDelegate::CreateWeakLambda(
				this,
				[this, LayerName, ActivatableWidgetClass, StateFunc,
					SuspendInputToken]()
				{
					UCommonUIExtensions::ResumeInputForPlayer(
						GetOwningPlayer(), SuspendInputToken);

					ActivatableWidgetT* Widget = PushWidgetToLayerStack<
						ActivatableWidgetT>(
						LayerName, ActivatableWidgetClass.Get(),
						[StateFunc](ActivatableWidgetT& WidgetToInit)
						{
							StateFunc(EAsyncWidgetLayerState::Initialize,
							          &WidgetToInit);
						});

					StateFunc(EAsyncWidgetLayerState::AfterPush, Widget);
				})
		);

		// Set up a cancel delegate so that we can resume input if this handler is canceled.
		StreamingHandle->BindCancelDelegate(FStreamableDelegate::CreateWeakLambda(
				this,
				[this, StateFunc, SuspendInputToken]()
				{
					UCommonUIExtensions::ResumeInputForPlayer(
						GetOwningPlayer(), SuspendInputToken);
					StateFunc(EAsyncWidgetLayerState::Canceled, nullptr);
				})
		);

		return StreamingHandle;
	}

	template <typename ActivatableWidgetT = UWidget_ActivatableBase>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerName, UClass* ActivatableWidgetClass)
	{
		return PushWidgetToLayerStack<ActivatableWidgetT>(LayerName, ActivatableWidgetClass, [](ActivatableWidgetT&)
		{
		});
	}

	template <typename ActivatableWidgetT = UWidget_ActivatableBase>
	ActivatableWidgetT* PushWidgetToLayerStack(FGameplayTag LayerName, UClass* ActivatableWidgetClass, TFunctionRef<void(ActivatableWidgetT&)> InitInstanceFunc)
	{
		static_assert(TIsDerivedFrom<ActivatableWidgetT, UWidget_ActivatableBase>::IsDerived, "Only CommonActivatableWidgets can be used here");

		if (UCommonActivatableWidgetContainerBase* Layer = GetLayerWidget(LayerName))
		{
			return Layer->AddWidget<ActivatableWidgetT>(ActivatableWidgetClass, InitInstanceFunc);
		}

		return nullptr;
	}

	// Find the widget if it exists on any of the layers and remove it from the layer.
	EPICSQUAD_API void FindAndRemoveWidgetFromLayer(UWidget_ActivatableBase* ActivatableWidget);

	// Get the layer widget for the given layer tag.
	EPICSQUAD_API UCommonActivatableWidgetContainerBase* GetLayerWidget(FGameplayTag LayerName) const;

protected:
	/** Register a layer that widgets can be pushed onto. */
	UFUNCTION(BlueprintCallable, Category="Layer")
	EPICSQUAD_API void RegisterLayer(UPARAM(meta = (Categories = "UI.Layer"))
	                                 FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget);

	EPICSQUAD_API virtual void OnIsDormantChanged();

	EPICSQUAD_API void OnWidgetStackTransitioning(UCommonActivatableWidgetContainerBase* Widget, bool bIsTransitioning);

private:
	bool bIsDormant = false;

	// Lets us keep track of all suspended input tokens so that multiple async UIs can be loading and we correctly suspend
	// for the duration of all of them.
	TArray<FName> SuspendInputTokens;

	// The registered layers for the primary layout.
	UPROPERTY(Transient, meta = (Categories = "UI.Layer"))
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;
};
