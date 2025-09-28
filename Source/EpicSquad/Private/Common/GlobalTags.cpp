// Fill out your copyright notice in the Description page of Project Settings.


#include "Common/GlobalTags.h"

namespace GlobalGameplayTags
{
	//Factions
	UE_DEFINE_GAMEPLAY_TAG(Factions_ID_Alliance,"Factions.ID.Alliance");
	UE_DEFINE_GAMEPLAY_TAG(Factions_ID_Horde,"Factions.ID.Horde");
	
	
	//Input
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Sprint,"Input.Action.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Roll,"Input.Action.Roll");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_MeleeAttack,"Input.Action.MeleeAttack");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_HeavyAttack,"Input.Action.HeavyAttack");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Interact,"Input.Action.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Throw,"Input.Action.Throw");
	
}

namespace InventoryGameplayTags
{
	//Inventory.Item.Quality
	UE_DEFINE_GAMEPLAY_TAG(Inventory_Item_Quality_Poor,"Inventory.Item.Quality.Poor");
	UE_DEFINE_GAMEPLAY_TAG(Inventory_Item_Quality_Uncommon,"Inventory.Item.Quality.Uncommon");
	UE_DEFINE_GAMEPLAY_TAG(Inventory_Item_Quality_Rare,"Inventory.Item.Quality.Rare");
	UE_DEFINE_GAMEPLAY_TAG(Inventory_Item_Quality_Epic,"Inventory.Item.Quality.Epic");
	UE_DEFINE_GAMEPLAY_TAG(Inventory_Item_Quality_Legendary,"Inventory.Item.Quality.Legendary");
	UE_DEFINE_GAMEPLAY_TAG(Inventory_Item_Quality_Mythic,"Inventory.Item.Quality.Mythic");

	//Inventory.Item.Equipment
	UE_DEFINE_GAMEPLAY_TAG(Inventory_Item_Type_Sword,"Inventory.Item.Type.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Inventory_Item_Type_Shield,"Inventory.Item.Type.Shield");

	
}

namespace LocomotionTags
{
	//Global Foley
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_Handplant,"Foley.Event.Handplant");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_Jump,"Foley.Event.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_Land,"Foley.Event.Land");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_Run,"Foley.Event.Run");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_RunStrafe,"Foley.Event.RunStrafe");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_RunBackwds,"Foley.Event.RunBackwds");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_Scuff,"Foley.Event.Scuff");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_ScuffPivot,"Foley.Event.ScuffPivot");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_ScuffWall,"Foley.Event.ScuffWall");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_Tumble,"Foley.Event.Tumble");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_Walk,"Foley.Event.Walk");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_WalkBackwds,"Foley.Event.WalkBackwds");

	//MotionMatching
	UE_DEFINE_GAMEPLAY_TAG(MotionMatching_Default,"MotionMatching.Default");
	UE_DEFINE_GAMEPLAY_TAG(MotionMatching_Idle,"MotionMatching.Idle");
	UE_DEFINE_GAMEPLAY_TAG(MotionMatching_Loops,"MotionMatching.Loops");
	UE_DEFINE_GAMEPLAY_TAG(MotionMatching_Pivots,"MotionMatching.Pivots");
	UE_DEFINE_GAMEPLAY_TAG(MotionMatching_Starts,"MotionMatching.Starts");
	UE_DEFINE_GAMEPLAY_TAG(MotionMatching_Stops,"MotionMatching.Stops");
	
}

namespace CombatTags
{
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Attack_Primary_A,"Combat.Ability.Attack.Primary.A");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Attack_Primary_B,"Combat.Ability.Attack.Primary.B");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Attack_Primary_C,"Combat.Ability.Attack.Primary.C");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Attack_Primary_D,"Combat.Ability.Attack.Primary.D");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Attack_Primary_E,"Combat.Ability.Attack.Primary.E");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Attack_Primary_F,"Combat.Ability.Attack.Primary.F");
	
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Attack_Secondary_A,"Combat.Ability.Attack.Secondary.A");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Attack_Secondary_B,"Combat.Ability.Attack.Secondary.B");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Attack_Secondary_C,"Combat.Ability.Attack.Secondary.C");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Attack_Secondary_D,"Combat.Ability.Attack.Secondary.D");
	
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Combo_Primary,"Combat.Ability.Combo.Primary");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Combo_Evade,"Combat.Ability.Combo.Evade");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Combo_Parry,"Combat.Ability.Combo.Parry");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Combo_Riposte,"Combat.Ability.Combo.Riposte");
	
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Combo_Secondary,"Combat.Ability.Combo.Secondary");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Combo_Cast,"Combat.Ability.Combo.Cast");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Combo_Block,"Combat.Ability.Combo.Block");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Ability_Combo_Opportunity,"Combat.Ability.Combo.Opportunity");
	
}
