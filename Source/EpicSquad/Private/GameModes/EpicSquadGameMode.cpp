// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameModes/EpicSquadGameMode.h"

#include "CommonSessionSubsystem.h"
#include "CommonUserSubsystem.h"
#include "GameMapsSettings.h"

#include "Development/LyraDeveloperSettings.h"

#include "Engine/StreamableManager.h"

#include "GameModes/LyraGameState.h"

#include "Kismet/GameplayStatics.h"

#include "Player/EpicSquadPlayerCharacter.h"
#include "Player/EpicSquadPlayerController.h"
#include "Player/PlayerState_Base.h"

#include "System/LyraGameSession.h"

#include "UI/LyraHUD.h"

AEpicSquadGameMode::AEpicSquadGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// use our custom PlayerController class
	GameStateClass = ALyraGameState::StaticClass();
	GameSessionClass = ALyraGameSession::StaticClass();
	//ReplaySpectatorPlayerControllerClass = ALyraReplayPlayerController::StaticClass();
	HUDClass = ALyraHUD::StaticClass();
	PlayerControllerClass = AEpicSquadPlayerController::StaticClass();
	DefaultPawnClass = AEpicSquadPlayerCharacter::StaticClass();
	PlayerStateClass = APlayerState_Base::StaticClass();
	// set default pawn class to our Blueprinted character /Game/Blueprints/Player/BP_PlayerMain.BP_PlayerMain
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Player/BP_PlayerMain"));
	// if (PlayerPawnBPClass.Class != nullptr)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }
	//
	// // set default controller to our Blueprinted controller /Game/Blueprints/Player/BP_PlayerController.BP_PlayerController
	// static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Blueprints/Player/BP_PlayerController"));
	// if(PlayerControllerBPClass.Class != nullptr)
	// {
	// 	PlayerControllerClass = PlayerControllerBPClass.Class;
	// }
}



