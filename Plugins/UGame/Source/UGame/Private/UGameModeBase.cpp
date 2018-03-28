// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#include "UGameModeBase.h"
#include "UGame.h"

AUGameModeBase::AUGameModeBase()
{
	HUDClass = AGameHUD::StaticClass();
	PickupHUD = CreateDefaultSubobject<AGameHUD>(TEXT("PickupHUD"));
}



void AUGameModeBase::DrawHudMenu()
{
	PickupHUD->DrawMenu();
}

void AUGameModeBase::RemoveMenu()
{
	PickupHUD->RemoveMenu();
}

void AUGameModeBase::DrawPickupHudMenu()
{
	PickupHUD->DrawPickUpMenu();
}