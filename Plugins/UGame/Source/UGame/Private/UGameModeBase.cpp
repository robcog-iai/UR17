// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#include "UGameModeBase.h"
#include "UGame.h"
#include "HUD/GameHUD.h"

AUGameModeBase::AUGameModeBase()
{
	HUDClass = AGameHUD::StaticClass();
}