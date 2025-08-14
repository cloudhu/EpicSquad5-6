// // CloudHu:604746493@qq.com All Rights Reserved

#pragma once

#include "NativeGameplayTags.h"

namespace GlobalGameplayTags
{
	//Factions
	EPICSQUAD_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Factions_ID_Alliance);
	EPICSQUAD_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Factions_ID_Horde);


}

namespace InventoryGameplayTags
{
	//Inventory.Item.Quality
	EPICSQUAD_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Item_Quality_Poor);
	EPICSQUAD_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Item_Quality_Uncommon);
	EPICSQUAD_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Item_Quality_Rare);
	EPICSQUAD_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Item_Quality_Epic);
	EPICSQUAD_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Item_Quality_Legendary);
	EPICSQUAD_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Item_Quality_Mythic);

	//Inventory.Item.Equipment
	EPICSQUAD_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Inventory_Item_Weapon_Sword);
}