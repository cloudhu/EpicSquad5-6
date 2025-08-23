// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraReplicationGraphSettings.h"
#include "System/LyraReplicationGraph.h"

ULyraReplicationGraphSettings::ULyraReplicationGraphSettings()
{
	CategoryName = TEXT("Game");
	DefaultReplicationGraphClass = ULyraReplicationGraph::StaticClass();
}