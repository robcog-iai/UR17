// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#include "UGameModeBase.h"
#include "Engine.h"
#include "UGame.h"

AUGameModeBase::AUGameModeBase()
{
 HUDClass = AGameHUD::StaticClass();

	PickupHUD = CreateDefaultSubobject<AGameHUD>(TEXT("PickupHUD"));
 PlayerController = nullptr;
}

void AUGameModeBase::DrawHudMenu()
{
	PickupHUD->DrawMenu();
}

void AUGameModeBase::RemoveMenu()
{
	PickupHUD->RemoveMenu();
}

void AUGameModeBase::DrawPickupHudMenu(float MouseX, float MouseY)
{
	PickupHUD->DrawPickUpMenu(MouseX, MouseY);
}