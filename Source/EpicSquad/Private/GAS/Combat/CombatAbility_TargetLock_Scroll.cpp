// CloudHu:604746493@qq.com All Rights Reserved


#include "GAS/Combat/CombatAbility_TargetLock_Scroll.h"
#include "AbilitySystemComponent.h"
#include "NinjaCombatFunctionLibrary.h"
#include "NinjaCombatSettings.h"
#include "NinjaCombatTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/Effects/CombatEffect_LockedBySource.h"
#include "AbilitySystem/Effects/CombatEffect_LockedOnTarget.h"
#include "AbilitySystem/Tasks/AbilityTask_TrackDistance.h"
#include "AbilityTasks/AbilityTask_PerformTargeting.h"
#include "Interfaces/Components/CombatDamageManagerInterface.h"
#include "Interfaces/Components/CombatTargetManagerInterface.h"
#include "TargetingSystem/TargetingPreset.h"
#include "TargetingSystem/TargetingSubsystem.h"
#include "Types/TargetingSystemTypes.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_Combat_Event_Target_Scroll_Left, "Combat.Event.Target.Scroll.Left");
UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_Combat_Event_Target_Scroll_Right, "Combat.Event.Target.Scroll.Right");

UCombatAbility_TargetLock_Scroll::UCombatAbility_TargetLock_Scroll()
{
	bTrackDistance = true;
	bPerformAsyncTargeting = true;
	bMoveToNewTargets = GetDefault<UNinjaCombatSettings>()->bShouldAcquireNextTargetsAutomatically;
	DistanceThreshold = 500.f;
	ActivationOwnedTags.AddTag(Tag_Combat_Input_Block_Camera_Yaw);
	ActivationBlockedTags.AddTag(Tag_Combat_State_Dead);
	TargetLockEffectClass = UCombatEffect_LockedOnTarget::StaticClass();
	SourceLockEffectClass = UCombatEffect_LockedBySource::StaticClass();

	FGameplayTagContainer InitialTags;
	InitialTags.AddTagFast(Tag_Combat_Ability_TargetLock);
	SetInitialTags(InitialTags);

	FAbilityTriggerData& EventTrigger = AbilityTriggers.AddDefaulted_GetRef();
	EventTrigger.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	EventTrigger.TriggerTag = Tag_Combat_Event_Target_Acquired;
}

bool UCombatAbility_TargetLock_Scroll::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                          const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
                                                          FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)
		&& IsValid(GetTargetManagerComponentFromActorInfo());
}

bool UCombatAbility_TargetLock_Scroll::HasTarget() const
{
	return GetTarget() == nullptr;
}

AActor* UCombatAbility_TargetLock_Scroll::GetTarget() const
{
	if (const UActorComponent* TargetManager = GetTargetManagerComponentFromActorInfo();
		UNinjaCombatFunctionLibrary::IsValidTargetManagerComponent(TargetManager))
	{
		return ICombatTargetManagerInterface::Execute_GetCombatTarget(TargetManager);
	}
	return nullptr;
}

void UCombatAbility_TargetLock_Scroll::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                       const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Even though it is not common to have costs/cooldowns related to Target Locks,
	// we can't assume that for all scenarios, so let's follow the GAS contract for this ability.
	//
	if (!K2_CommitAbility())
	{
		const FString& Message = FString::Printf(TEXT("Unable to activate, cost requirements not met."));
		AddDebugMessage(Message);
		K2_CancelAbility();
	}

	static constexpr bool bMatchExact = false;
	TargetEventTask = InitializeEventTask(Tag_Combat_Event_Target, bMatchExact);
	TargetEventTask->ReadyForActivation();

	// The first possible activation happens via a Gameplay Event. This allows external factors
	// to collect a target for us. Most common scenario being the Perception System for AI Agents.
	//
	// At this point we want to ensure that we received a known event and that the optional object
	// in this event contains the appropriate target. Since there are two slots for optional objects,
	// we'll separate the retrieval into another function to provide some flexibility there.
	//
	if (TriggerEventData != nullptr && TriggerEventData->EventTag == Tag_Combat_Event_Target_Acquired)
	{
		UObject* OptionalObject = const_cast<UObject*>(TriggerEventData->OptionalObject.Get());

		if (AActor* NewTarget = Cast<AActor>(OptionalObject); IsValid(NewTarget))
		{
			const FString& Message = FString::Printf(TEXT("Received target %s via Gameplay Event."), *GetNameSafe(NewTarget));
			AddDebugMessage(Message);
			HandleNewTarget(NewTarget);
		}
		else
		{
			const FString& Message = FString::Printf(TEXT("Received invalid target via Gameplay Event."));
			AddDebugMessage(Message);
			K2_CancelAbility();
		}
	}

	// If the activation did not happen from an event, then we can follow the default path, which
	// means executing the targeting preset assigned to this ability.
	//
	// We are deliberately not checking for a valid target preset in the "CanActivateAbility" function,
	// because it's expected to be absent for AI agents that will only rely on the Perception System for
	// target acquisition, and will most likely never have a targeting preset set.
	//
	else
	{
		CollectTarget(TargetingPreset, bPerformAsyncTargeting);
	}
}

void UCombatAbility_TargetLock_Scroll::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	FinishLatentTasks({TargetingTask, DistanceTask, TargetEventTask});
	DismissCurrentTarget();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCombatAbility_TargetLock_Scroll::HandleEventReceived_Implementation(FGameplayEventData Payload)
{
	if (Payload.EventTag == Tag_Combat_Event_Target_Scan)
	{
		// Collect a new target. Also handles cancelling the current one, if no new target was found.
		// Also handles the scenario of re-collecting the same target, which means no changes.
		CollectTarget(TargetingPreset, bPerformAsyncTargeting);
	}
	else if (Payload.EventTag == Tag_Combat_Event_Target_Dismissed)
	{
		K2_EndAbility();
	}
	else if (Payload.EventTag == Tag_Combat_Event_Target_Scroll_Left)
	{
		TargetPickPolicy = ETargetPickPolicy::ScrollLeft;
		CollectTarget(TargetingPreset, bPerformAsyncTargeting);
	}
	else if (Payload.EventTag == Tag_Combat_Event_Target_Scroll_Right)
	{
		TargetPickPolicy = ETargetPickPolicy::ScrollRight;
		CollectTarget(TargetingPreset, bPerformAsyncTargeting);
	}
}

void UCombatAbility_TargetLock_Scroll::CollectTarget(UTargetingPreset* Preset, bool bExecuteAsync)
{
	if (IsValid(Preset))
	{
		TargetingTask = UAbilityTask_PerformTargeting::PerformTargetingRequest(this, Preset, bExecuteAsync);
		TargetingTask->OnTargetReady.AddUniqueDynamic(this, &ThisClass::HandleTargetReady);
		TargetingTask->ReadyForActivation();
	}
	else
	{
		// Nothing to do here, without a Target Preset.
		K2_CancelAbility();
	}
}

void UCombatAbility_TargetLock_Scroll::DismissCurrentTarget()
{
	if (const AActor* CurrentTarget = GetTarget(); IsValid(CurrentTarget))
	{
		ClearTargetFromOwnerComponent();
		UnbindFromDeathDelegate(CurrentTarget);
		RemoveTargetLockEffect();
		StopTrackingDistance();

		CurrentTarget = nullptr;

		const FString& Message = FString::Printf(TEXT("Unlocked from Target."));
		AddDebugMessage(Message);
	}
}

void UCombatAbility_TargetLock_Scroll::HandleTargetReady(FTargetingRequestHandle TargetingRequestHandle)
{
	const UTargetingSubsystem* TargetingSubsystem = UTargetingSubsystem::Get(GetWorld());
	check(IsValid(TargetingSubsystem));

	TArray<AActor*> TargetsFound;
	TargetingSubsystem->GetTargetingResultsActors(TargetingRequestHandle, TargetsFound);
	if (TargetsFound.IsEmpty())
	{
		const FString& Message = FString::Printf(TEXT("No targets found!"));
		AddDebugMessage(Message);
	}
	AActor* NewTarget = TargetsFound.IsValidIndex(0) ? TargetsFound[0] : nullptr;
	//Pick Target based on TargetPickPolicy
	if (TargetsFound.Num() > 1)
	{
		int CurrentTargetIndex =TargetsFound.IndexOfByKey(GetTarget()) ;
		switch (TargetPickPolicy)
		{
		case ETargetPickPolicy::NearestOne:
			//Targeting Preset will pick the nearest one
			break;
		case ETargetPickPolicy::ScrollLeft:
			CurrentTargetIndex = TargetsFound.IsValidIndex(CurrentTargetIndex+1)?CurrentTargetIndex+1:CurrentTargetIndex+2;
			break;
		case ETargetPickPolicy::ScrollRight:
			CurrentTargetIndex = TargetsFound.IsValidIndex(CurrentTargetIndex-1)?CurrentTargetIndex-1:1;
			break;
		}
		NewTarget = TargetsFound.IsValidIndex(CurrentTargetIndex) ? TargetsFound[CurrentTargetIndex] : TargetsFound.Last();
		TargetPickPolicy = ETargetPickPolicy::NearestOne;
	}
	if (IsValid(NewTarget) && NewTarget != GetTarget())
	{
		HandleNewTarget(NewTarget);
	}
	else if (!IsValid(NewTarget))
	{
		ClearTargetFromOwnerComponent();
		K2_CancelAbility();
	}
}

void UCombatAbility_TargetLock_Scroll::ApplyTargetLockEffect(AActor* Target)
{
	if (IsValid(TargetLockEffectClass))
	{
		const FGameplayEffectSpecHandle TargetLockSpecHandle = MakeOutgoingGameplayEffectSpec(TargetLockEffectClass);
		ActiveTargetLockEffectHandle = K2_ApplyGameplayEffectSpecToOwner(TargetLockSpecHandle);
	}

	if (IsValid(SourceLockEffectClass))
	{
		const FGameplayEffectSpecHandle LockedBySourceSpecHandle = MakeOutgoingGameplayEffectSpec(SourceLockEffectClass);
		const FGameplayAbilityTargetingLocationInfo TargetingLocation;
		const FGameplayAbilityTargetDataHandle TargetDataHandle = TargetingLocation.MakeTargetDataHandleFromActors({Target});

		if (const TArray<FActiveGameplayEffectHandle>& Handles = K2_ApplyGameplayEffectSpecToTarget(LockedBySourceSpecHandle, TargetDataHandle); !Handles.
			IsEmpty())
		{
			ActiveSourceLockEffectHandles.Empty();
			ActiveSourceLockEffectHandles.Append(Handles);
		}
	}
}

void UCombatAbility_TargetLock_Scroll::RemoveTargetLockEffect()
{
	if (ActiveTargetLockEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(ActiveTargetLockEffectHandle);
		ActiveTargetLockEffectHandle.Invalidate();
	}

	if (!ActiveSourceLockEffectHandles.IsEmpty())
	{
		for (FActiveGameplayEffectHandle& Handle : ActiveSourceLockEffectHandles)
		{
			if (UAbilitySystemComponent* TargetAbilityComponent = Handle.GetOwningAbilitySystemComponent(); IsValid(TargetAbilityComponent) && Handle.IsValid())
			{
				// We need to check for the target ASC since we might be cancelling this ability
				// because the target has died and therefore the ASC might have been destroyed.
				TargetAbilityComponent->RemoveActiveGameplayEffect(Handle);
			}
			Handle.Invalidate();
		}

		ActiveSourceLockEffectHandles.Empty();
	}
}

void UCombatAbility_TargetLock_Scroll::StartTrackingDistance()
{
	AActor* CurrentTarget = GetTarget();
	if (!IsValid(CurrentTarget))
	{
		return;
	}

	if (bTrackDistance && DistanceThreshold > 0.f)
	{
		if (IsValid(DistanceTask))
		{
			// Just replace the target in the current task. No need to re-instantiate it.
			DistanceTask->SetTarget(CurrentTarget);
		}
		else
		{
			DistanceTask = UAbilityTask_TrackDistance::CreateTask(this, CurrentTarget, DistanceThreshold);
			DistanceTask->OnDistanceExceeded.AddUniqueDynamic(this, &ThisClass::HandleDistanceExceeded);
			DistanceTask->ReadyForActivation();
		}
	}
}

void UCombatAbility_TargetLock_Scroll::StopTrackingDistance()
{
	FinishLatentTask(DistanceTask);
}

void UCombatAbility_TargetLock_Scroll::SetTargetOnOwnerComponent(AActor* Target) const
{
	UActorComponent* TargetManager = GetTargetManagerComponentFromActorInfo();
	check(IsValid(TargetManager));
	ICombatTargetManagerInterface::Execute_SetCombatTarget(TargetManager, Target);
}

void UCombatAbility_TargetLock_Scroll::ClearTargetFromOwnerComponent() const
{
	if (UActorComponent* TargetManager = GetTargetManagerComponentFromActorInfo(); IsValid(TargetManager))
	{
		ICombatTargetManagerInterface::Execute_SetCombatTarget(TargetManager, nullptr);

		const FString& Message = FString::Printf(TEXT("Cleared target."));
		AddDebugMessage(Message);
	}
}

void UCombatAbility_TargetLock_Scroll::BindToDeathDelegate(const AActor* Target)
{
	if (UActorComponent* DamageManager = UNinjaCombatFunctionLibrary::GetDamageManagerComponent(Target); IsValid(DamageManager))
	{
		FOwnerDiedDelegate Delegate;
		Delegate.BindDynamic(this, &ThisClass::HandleTargetDeath);
		ICombatDamageManagerInterface::Execute_BindToOwnerDiedDelegate(DamageManager, Delegate);
	}
}

void UCombatAbility_TargetLock_Scroll::UnbindFromDeathDelegate(const AActor* Target) const
{
	if (UActorComponent* DamageManager = UNinjaCombatFunctionLibrary::GetDamageManagerComponent(Target); IsValid(DamageManager))
	{
		ICombatDamageManagerInterface::Execute_UnbindFromOwnerDiedDelegate(DamageManager, this);
	}
}

void UCombatAbility_TargetLock_Scroll::HandleTargetDeath_Implementation(AActor* Actor)
{
	if (IsValid(Actor))
	{
		UnbindFromDeathDelegate(Actor);
		ClearTargetFromOwnerComponent();
	}

	if (bMoveToNewTargets)
	{
		UTargetingPreset* Preset = IsValid(NextTargetingPreset) ? NextTargetingPreset : TargetingPreset;
		CollectTarget(Preset, bPerformAsyncTargeting);
	}
	else
	{
		K2_EndAbility();
	}
}

void UCombatAbility_TargetLock_Scroll::HandleDistanceExceeded_Implementation(const AActor* Target, float Distance)
{
	const FString& Message = FString::Printf(TEXT("Target %s Exceeded distance %f."), *GetNameSafe(Target), Distance);
	AddDebugMessage(Message);
	if (bFindNewTargetOnDistanceExceeded)
	{
		// Are we need to do this before find New Taget ?
		// FinishLatentTasks({ TargetingTask,DistanceTask, TargetEventTask });
		// DismissCurrentTarget();
		CollectTarget(TargetingPreset, bPerformAsyncTargeting);
	}
	else
	{
		K2_EndAbility();
	}
}

void UCombatAbility_TargetLock_Scroll::HandleNewTarget_Implementation(AActor* NewTarget)
{
	check(IsValid(NewTarget));

	// Only remove effects that are relevant to the previous target.
	//
	// We want to reuse our current Tasks and Target Lock Actor, so we'll keep those
	// active and just update the current target next.
	//
	AActor* CurrentTarget = GetTarget();
	if (IsValid(CurrentTarget) && CurrentTarget != NewTarget)
	{
		ClearTargetFromOwnerComponent();
		UnbindFromDeathDelegate(CurrentTarget);
	}

	CurrentTarget = NewTarget;
	SetTargetOnOwnerComponent(CurrentTarget);
	BindToDeathDelegate(CurrentTarget);
	ApplyTargetLockEffect(CurrentTarget);
	StartTrackingDistance();

	const FString& Message = FString::Printf(TEXT("Locked on Target %s."), *GetNameSafe(CurrentTarget));
	AddDebugMessage(Message);
}
