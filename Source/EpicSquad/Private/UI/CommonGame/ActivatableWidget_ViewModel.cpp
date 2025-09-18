// CloudHu:604746493@qq.com All Rights Reserved


#include "UI/CommonGame/ActivatableWidget_ViewModel.h"

#include "AbilitySystemGlobals.h"
#include "MVVMSubsystem.h"

#include "UI/Interfaces/CombatViewModelInterface.h"

#include "View/MVVMView.h"

UActivatableWidget_ViewModel::UActivatableWidget_ViewModel(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	bShowWidgetWithCombatActor = true;
	bCollapseWidgetWithoutCombatActor = true;
	DefaultVisibility = ESlateVisibility::HitTestInvisible;
}

void UActivatableWidget_ViewModel::NativeConstruct()
{
	Super::NativeConstruct();
	DefaultVisibility = GetVisibility();
	
	RefreshVisibility();
	RefreshViewModels();
}

void UActivatableWidget_ViewModel::NativeDestruct()
{
	ClearViewModels();
	Super::NativeDestruct();
	
}

void UActivatableWidget_ViewModel::SetCombatActor_Implementation(AActor* NewCombatActor)
{
	CombatActor = NewCombatActor;

	RefreshVisibility();
	RefreshViewModels();
}

AActor* UActivatableWidget_ViewModel::GetCombatActor() const
{
	return IsValid(CombatActor) ? CombatActor : GetOwningPlayerPawn(); 
}

UAbilitySystemComponent* UActivatableWidget_ViewModel::GetAbilityComponent() const
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetCombatActor());
}

void UActivatableWidget_ViewModel::RefreshVisibility()
{
	if (IsValid(CombatActor) && bShowWidgetWithCombatActor)
	{
		SetVisibility(DefaultVisibility);
	}
	else if (!IsValid(CombatActor) && bCollapseWidgetWithoutCombatActor)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UActivatableWidget_ViewModel::RefreshViewModels()
{
	const UMVVMView* View = UMVVMSubsystem::GetViewFromUserWidget(this);
	if (!IsValid(View) || !View->AreSourcesInitialized())
	{
		return;
	}

	for (const TArrayView<const FMVVMView_Source> Sources = View->GetSources(); const FMVVMView_Source& ViewSource : Sources)
	{
		if (UObject* Source = ViewSource.Source; Source != this)
		{
			SetWidgetDataToViewModel(Source);	
		}
	}
}

void UActivatableWidget_ViewModel::SetWidgetDataToViewModel(UObject* Source)
{
	if (!IsValid(Source))
	{
		return;
	}

	if (Source->Implements<UCombatViewModelInterface>())
	{
		// Set this first, since it might be required by the next interfaces!
		ICombatViewModelInterface::Execute_SetWidgetOwner(Source, GetOwningPlayer());
	}
	
	if (Source->Implements<UCombatActorAwareInterface>())
	{
		Execute_SetCombatActor(Source, CombatActor);
	}
}

void UActivatableWidget_ViewModel::ClearViewModels()
{
	const UMVVMView* View = UMVVMSubsystem::GetViewFromUserWidget(this);
	if (!IsValid(View) || !View->AreSourcesInitialized())
	{
		return;
	}

	for (const TArrayView<const FMVVMView_Source> Sources = View->GetSources(); const FMVVMView_Source& ViewSource : Sources)
	{
		UObject* Source = ViewSource.Source; 
		ClearWidgetDataFromViewModel(Source);
	}
}

void UActivatableWidget_ViewModel::ClearWidgetDataFromViewModel(UObject* Source)
{
	if (!IsValid(Source))
	{
		return;
	}
	
	if (Source->Implements<UCombatActorAwareInterface>())
	{
		Execute_SetCombatActor(Source, nullptr);
	}

	if (Source->Implements<UCombatViewModelInterface>())
	{
		ICombatViewModelInterface::Execute_ClearWidgetOwner(Source);
	}
}
