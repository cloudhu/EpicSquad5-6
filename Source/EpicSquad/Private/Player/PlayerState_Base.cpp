// // CloudHu:604746493@qq.com All Rights Reserved


#include "Player/PlayerState_Base.h"

#include "Components/NinjaInventoryManagerComponent.h"

#include "Player/EpicSquadPlayerController.h"

APlayerState_Base::APlayerState_Base(const FObjectInitializer& ObjectInitializer)
{
	InventoryManager = CreateDefaultSubobject<UNinjaInventoryManagerComponent>(TEXT("InventoryManager"));

}

UNinjaInventoryManagerComponent* APlayerState_Base::GetInventoryManager_Implementation() const
{
	return InventoryManager;
}

AEpicSquadPlayerController* APlayerState_Base::GetEpicSquadPC() const
{
	return Cast<AEpicSquadPlayerController>(GetPlayerController());;
}
