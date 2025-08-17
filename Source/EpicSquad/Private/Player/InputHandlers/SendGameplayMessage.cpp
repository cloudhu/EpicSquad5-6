// CloudHu:604746493@qq.com All Rights Reserved


#include "Player/InputHandlers/SendGameplayMessage.h"

#include "Common/FGameplayMessageMagnitude.h"

#include "Components/NinjaInputManagerComponent.h"

#include "GameFramework/GameplayMessageSubsystem.h"

USendGameplayMessage::USendGameplayMessage()
{
	EventTag = FGameplayTag::EmptyTag;
	TriggerEvents.Add(ETriggerEvent::Triggered);
}

void USendGameplayMessage::HandleTriggeredEvent_Implementation(UNinjaInputManagerComponent* Manager, const FInputActionValue& Value,
                                                               const UInputAction* InputAction, float ElapsedTime) const
{
	FGameplayMessageMagnitude MessageMagnitude;
	MessageMagnitude.Magnitude = Magnitude;

	UGameplayMessageSubsystem::Get(GetWorld()).BroadcastMessage(EventTag,MessageMagnitude);
}
