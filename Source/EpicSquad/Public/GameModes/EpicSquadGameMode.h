// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/NinjaCombatGameMode.h"

#include "EpicSquadGameMode.generated.h"

/**
 *  Simple Game Mode for a top-down perspective game
 *  Sets the default gameplay framework classes
 *  Check the Blueprint derived class for the set values
 */
UCLASS(abstract)
class EPICSQUAD_API AEpicSquadGameMode : public ANinjaCombatGameMode
{
	GENERATED_BODY()

public:

	/** Constructor */
	AEpicSquadGameMode();
};



