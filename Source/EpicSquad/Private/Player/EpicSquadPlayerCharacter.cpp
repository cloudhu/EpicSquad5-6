// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/EpicSquadPlayerCharacter.h"
#include "NinjaCombatTags.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ImmersiveComponentBase.h"
#include "Components/NinjaCombatCharacterMovementComponent.h"
#include "Components/NinjaCombatEquipmentAdapterComponent.h"
#include "Components/NinjaCombatManagerComponent.h"
#include "Components/NinjaEquipmentManagerComponent.h"
#include "Components/NinjaFactionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEpicSquadPlayerCharacter::AEpicSquadPlayerCharacter(const FObjectInitializer& ObjectInitializer): Super(
	ObjectInitializer.SetDefaultSubobjectClass<UNinjaCombatCharacterMovementComponent>(CharacterMovementComponentName))
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	CombatManager = CreateDefaultSubobject<UNinjaCombatManagerComponent>("CombatManager");

	ForwardReference = CreateDefaultSubobject<UArrowComponent>("ForwardReference");
	ForwardReference->ComponentTags.Add(Tag_Combat_Component_ForwardReference.GetTag().GetTagName());
	ForwardReference->SetVisibleFlag(false);
	ForwardReference->SetUsingAbsoluteRotation(true);
	ForwardReference->SetWorldRotation(FRotator::ZeroRotator);
	ForwardReference->SetArrowColor(FLinearColor::Green);
	ForwardReference->SetupAttachment(GetRootComponent());

	/** Equipment Manager component. */
	EquipmentManager = CreateDefaultSubobject<UNinjaEquipmentManagerComponent>(TEXT("EquipmentManager"));
	EquipmentWeaponManager = CreateDefaultSubobject<UNinjaCombatEquipmentAdapterComponent>(TEXT("EquipmentWeaponManager"));

	static const FName FactionComponentName = TEXT("FactionManager");
	FactionManager = CreateOptionalDefaultSubobject<UNinjaFactionComponent>(FactionComponentName);
	// ImmersiveInteractionComponent = CreateOptionalDefaultSubobject<UImmersiveComponentBase>(TEXT("ImmersiveInteractionManager"));
}


UNinjaCombatManagerComponent* AEpicSquadPlayerCharacter::GetCombatManager_Implementation() const
{
	return CombatManager;
}

USceneComponent* AEpicSquadPlayerCharacter::GetCombatForwardReference_Implementation() const
{
	return ForwardReference;
}

USkeletalMeshComponent* AEpicSquadPlayerCharacter::GetCombatMesh_Implementation() const
{
	return GetMesh();
}

UAnimInstance* AEpicSquadPlayerCharacter::GetCombatAnimInstance_Implementation() const
{
	return GetMesh()->GetAnimInstance();
}

UActorComponent* AEpicSquadPlayerCharacter::GetWeaponManagerComponent_Implementation() const
{
	return EquipmentWeaponManager;
}

TArray<UNinjaInputSetupDataAsset*> AEpicSquadPlayerCharacter::GetInputSetups_Implementation() const
{
	return CharacterInputs;
}

UNinjaEquipmentManagerComponent* AEpicSquadPlayerCharacter::GetEquipmentManager_Implementation() const
{
	return EquipmentManager;
}

UNinjaFactionComponent* AEpicSquadPlayerCharacter::GetFactionComponent_Implementation() const
{
	return FactionManager;
}
