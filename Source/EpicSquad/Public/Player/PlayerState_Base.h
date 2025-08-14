// // CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/NinjaGASPlayerState.h"

#include "Interfaces/InventorySystemInterface.h"

#include "PlayerState_Base.generated.h"

/**
 * 
 */
UCLASS()
class EPICSQUAD_API APlayerState_Base : public ANinjaGASPlayerState, public IInventorySystemInterface
{
	GENERATED_BODY()
public:
	APlayerState_Base(const FObjectInitializer& ObjectInitializer);

	// -- Begin InventorySystem implementation
	virtual UNinjaInventoryManagerComponent* GetInventoryManager_Implementation() const override;
	// -- End InventorySystem implementation
private:

	/** Inventory Manager component. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNinjaInventoryManagerComponent> InventoryManager;
};
