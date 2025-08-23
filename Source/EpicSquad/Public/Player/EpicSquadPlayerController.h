// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/NinjaFactionPlayerController.h"
#include "EpicSquadPlayerController.generated.h"


class UNinjaInputManagerComponent;
/**
 *  Player controller for a top-down perspective game.
 *  Implements point and click based controls
 */
UCLASS(abstract)
class EPICSQUAD_API AEpicSquadPlayerController : public ANinjaFactionPlayerController
{
	GENERATED_BODY()


public:

	/** Constructor */
	AEpicSquadPlayerController();

	void ResetTeam() const;
private:

	/** Manages player input, using Enhanced Input/Ninja Input. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess=true))
	TObjectPtr<UNinjaInputManagerComponent> InputManager;

};


