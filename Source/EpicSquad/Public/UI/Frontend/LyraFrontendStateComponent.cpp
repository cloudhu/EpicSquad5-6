// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraFrontendStateComponent.h"

#include "CommonGameInstance.h"
#include "CommonSessionSubsystem.h"
#include "CommonUserSubsystem.h"
#include "ControlFlowManager.h"
#include "Kismet/GameplayStatics.h"
#include "NativeGameplayTags.h"
#include "PrimaryGameLayout.h"

#include "UI/Subsystem/LyraUIManagerSubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraFrontendStateComponent)

namespace FrontendTags
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_PLATFORM_TRAIT_SINGLEONLINEUSER, "Platform.Trait.SingleOnlineUser");
	UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MENU, "UI.Layer.Menu");
}

ULyraFrontendStateComponent::ULyraFrontendStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ULyraFrontendStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for the experience load to complete
	//AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	// ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
	// check(ExperienceComponent);
	//
	// // This delegate is on a component with the same lifetime as this one, so no need to unhook it in 
	// ExperienceComponent->CallOrRegister_OnExperienceLoaded_HighPriority(FOnLyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	const AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();

	if (ULyraUIManagerSubsystem* UIManager = GameState->GetGameInstance()->GetSubsystem<ULyraUIManagerSubsystem>())
	{
		UIManager->CallOrRegister_OnLayoutLoaded_HighPriority(FOnLayoutLoaded::FDelegate::CreateUObject(this, &ThisClass::OnFrontendLayoutLoaded));
	}
	//OnFrontendLayoutLoaded(true);
}

void ULyraFrontendStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

bool ULyraFrontendStateComponent::ShouldShowLoadingScreen(FString& OutReason) const
{
	if (bShouldShowLoadingScreen)
	{
		OutReason = TEXT("Frontend Flow Pending...");

		if (FrontEndFlow.IsValid())
		{
			if (const TOptional<FString> StepDebugName = FrontEndFlow->GetCurrentStepDebugName(); StepDebugName.IsSet())
			{
				OutReason = StepDebugName.GetValue();
			}
		}

		return true;
	}

	return false;
}

void ULyraFrontendStateComponent::OnFrontendLayoutLoaded(const bool bSuccess)
{
	FControlFlow& Flow = FControlFlowStatics::Create(this, TEXT("FrontendFlow"))
	                     .QueueStep(TEXT("Wait For User Initialization"), this, &ThisClass::FlowStep_WaitForUserInitialization)
	                     .QueueStep(TEXT("Try Show Press Start Screen"), this, &ThisClass::FlowStep_TryShowPressStartScreen)
	                     .QueueStep(TEXT("Try Join Requested Session"), this, &ThisClass::FlowStep_TryJoinRequestedSession)
	                     .QueueStep(TEXT("Try Show Main Screen"), this, &ThisClass::FlowStep_TryShowMainScreen);

	Flow.ExecuteFlow();

	FrontEndFlow = Flow.AsShared();
}

//不要采用优化提示的const 或 static，会导致编译不过 
void ULyraFrontendStateComponent::FlowStep_WaitForUserInitialization(FControlFlowNodeRef SubFlow)
{
	// If this was a hard disconnect, explicitly destroy all user and session state
	// TODO: Refactor the engine disconnect flow so it is more explicit about why it happened
	bool bWasHardDisconnect = false;
	const AGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AGameModeBase>();
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	//EpicDebug::Print(TEXT("FlowStep_WaitForUserInitialization"));
	if (ensure(GameMode) && UGameplayStatics::HasOption(GameMode->OptionsString, TEXT("closed")))
	{
		//EpicDebug::Print(TEXT("bWasHardDisconnect true"));
		bWasHardDisconnect = true;
	}

	// Only reset users on hard disconnect
	if (UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>(); ensure(UserSubsystem) && bWasHardDisconnect)
	{
		//EpicDebug::Print(TEXT("ResetUserState"));
		UserSubsystem->ResetUserState();
	}

	// Always reset sessions
	if (UCommonSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UCommonSessionSubsystem>(); ensure(SessionSubsystem))
	{
		//EpicDebug::Print(TEXT("SessionSubsystem->CleanUpSessions()"));
		SessionSubsystem->CleanUpSessions();
	}

	SubFlow->ContinueFlow();
}

void ULyraFrontendStateComponent::FlowStep_TryShowPressStartScreen(FControlFlowNodeRef SubFlow)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>();
	//EpicDebug::Print(TEXT("Try To Show Press Start Screen"));
	// Check to see if the first player is already logged in, if they are, we can skip the press start screen.
	if (const UCommonUserInfo* FirstUser = UserSubsystem->GetUserInfoForLocalPlayerIndex(0))
	{
		if (FirstUser->InitializationState == ECommonUserInitializationState::LoggedInLocalOnly ||
			FirstUser->InitializationState == ECommonUserInitializationState::LoggedInOnline)
		{
			//EpicDebug::Print(TEXT("the first player is already logged in,skip the press start screen."));
			SubFlow->ContinueFlow();
			return;
		}
	}

	// Check to see if the platform actually requires a 'Press Start' screen.  This is only
	// required on platforms where there can be multiple online users where depending on what player's
	// controller presses 'Start' establishes the player to actually log in to the game with.
	if (!UserSubsystem->ShouldWaitForStartInput())
	{
		// Start the auto login process, this should finish quickly and will use the default input device id
		InProgressPressStartScreen = SubFlow;
		UserSubsystem->OnUserInitializeComplete.AddDynamic(this, &ULyraFrontendStateComponent::OnUserInitialized);
		UserSubsystem->TryToInitializeForLocalPlay(0, FInputDeviceId(), false);
		//EpicDebug::Print(TEXT("Start the auto login process,skip the press start screen."));
		return;
	}

	// Add the Press Start screen, move to the next flow when it deactivates.
	const AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();

	if (const ULyraUIManagerSubsystem* UIManager = GameState->GetGameInstance()->GetSubsystem<ULyraUIManagerSubsystem>())
	{
		if (UPrimaryGameLayout* RootLayout = UIManager->GetCreatedPrimaryLayout())
		{
			constexpr bool bSuspendInputUntilComplete = true;
			RootLayout->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(
				FrontendTags::TAG_UI_LAYER_MENU, bSuspendInputUntilComplete, PressStartScreenClass,
				[this, SubFlow](const EAsyncWidgetLayerState State, const UCommonActivatableWidget* Screen)
				{
					switch (State)
					{
					case EAsyncWidgetLayerState::AfterPush:
						bShouldShowLoadingScreen = false;
						Screen->OnDeactivated().AddWeakLambda(this, [this, SubFlow]()
						{
							SubFlow->ContinueFlow();
						});
						break;
					case EAsyncWidgetLayerState::Canceled:
						bShouldShowLoadingScreen = false;
						SubFlow->ContinueFlow();
					default: ;
					}
				});
		}
	}
}

void ULyraFrontendStateComponent::OnUserInitialized(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege,
                                                    ECommonUserOnlineContext OnlineContext)
{
	const FControlFlowNodePtr FlowToContinue = InProgressPressStartScreen;
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);

	if (UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>(); ensure(FlowToContinue.IsValid() && UserSubsystem))
	{
		UserSubsystem->OnUserInitializeComplete.RemoveDynamic(this, &ULyraFrontendStateComponent::OnUserInitialized);
		InProgressPressStartScreen.Reset();

		if (bSuccess)
		{
			// On success continue flow normally
			FlowToContinue->ContinueFlow();
		}
		else
		{
			// TODO: Just continue for now, could go to some sort of error screen
			FlowToContinue->ContinueFlow();
		}
	}
}

void ULyraFrontendStateComponent::FlowStep_TryJoinRequestedSession(FControlFlowNodeRef SubFlow)
{
	if (UCommonGameInstance* GameInstance = Cast<UCommonGameInstance>(UGameplayStatics::GetGameInstance(this)); GameInstance->GetRequestedSession() != nullptr
		&& GameInstance->CanJoinRequestedSession())
	{
		//EpicDebug::Print("FlowStep_TryJoinRequestedSession");
		if (UCommonSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UCommonSessionSubsystem>(); ensure(SessionSubsystem))
		{
			// Bind to session join completion to continue or cancel the flow
			// TODO:  Need to ensure that after session join completes, the server travel completes.
			OnJoinSessionCompleteEventHandle = SessionSubsystem->OnJoinSessionCompleteEvent.AddWeakLambda(
				this, [this, SubFlow, SessionSubsystem](const FOnlineResultInformation& Result)
				{
					// Unbind delegate. SessionSubsystem is the object triggering this event, so it must still be valid.
					SessionSubsystem->OnJoinSessionCompleteEvent.Remove(OnJoinSessionCompleteEventHandle);
					OnJoinSessionCompleteEventHandle.Reset();

					if (Result.bWasSuccessful)
					{
						//EpicDebug::Print("No longer transitioning to the main menu!");
						// No longer transitioning to the main menu
						SubFlow->CancelFlow();
					}
					else
					{
						// Proceed to the main menu
						SubFlow->ContinueFlow();
					}
				});
			GameInstance->JoinRequestedSession();
			return;
		}
	}
	// Skip this step if we didn't start requesting a session join
	SubFlow->ContinueFlow();
}

void ULyraFrontendStateComponent::FlowStep_TryShowMainScreen(FControlFlowNodeRef SubFlow)
{
	const AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();

	if (const ULyraUIManagerSubsystem* UIManager = GameState->GetGameInstance()->GetSubsystem<ULyraUIManagerSubsystem>())
	{
		if (UPrimaryGameLayout* RootLayout = UIManager->GetCreatedPrimaryLayout())
		{
			//EpicDebug::Print("FlowStep_TryShowMainScreen");
			RootLayout->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(
				FrontendTags::TAG_UI_LAYER_MENU, true, MainScreenClass,
				[this, SubFlow](const EAsyncWidgetLayerState State, UCommonActivatableWidget* Screen)
				{
					switch (State)
					{
					case EAsyncWidgetLayerState::AfterPush:
						//EpicDebug::Print("FlowStep_TryShowMainScreen AfterPush");
						bShouldShowLoadingScreen = false;
						SubFlow->ContinueFlow();
						return;
					case EAsyncWidgetLayerState::Canceled:
						//EpicDebug::Print("FlowStep_TryShowMainScreen Canceled");
						bShouldShowLoadingScreen = false;
						SubFlow->ContinueFlow();
					default: ;
					}
				});
		}
	}
}
