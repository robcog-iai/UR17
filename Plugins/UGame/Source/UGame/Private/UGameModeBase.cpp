// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#include "UGameModeBase.h"
#include "Engine.h"
#include "UGame.h"

AUGameModeBase::AUGameModeBase()
{
    HUDClass = AGameHUD::StaticClass();
}
