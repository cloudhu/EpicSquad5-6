// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/EpicSquadPlayerController.h"
#include "EnhancedInputSubsystems.h"

#include "Components/NinjaInputManagerComponent.h"


AEpicSquadPlayerController::AEpicSquadPlayerController()
{
	static const FName InputManagerName = FName("InputManager");
	InputManager = CreateDefaultSubobject<UNinjaInputManagerComponent>(InputManagerName);

}


