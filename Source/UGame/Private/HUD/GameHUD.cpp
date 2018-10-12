// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "GameHUD.h"
#include "UGame.h"
#include "../Character/GameController.h"
#include "PickupUI.h"
#include "Engine.h"

AGameHUD::AGameHUD()
{
    GPickup = nullptr;
}

void AGameHUD::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

void AGameHUD::RemoveMenu()
{
    GPickup->bFreeMouse = false;
    GPickup->bRightMouse = false;
    GPickup->bCallMenu = false;

    GEngine->GameViewport->RemoveAllViewportWidgets();
}

void AGameHUD::DrawPickUpMenu(float MouseX, float MouseY)
{
    SAssignNew(PickupUI, SPickupUI).GameHUD(this);
    PickupUI->WidgetPosition = FVector2D(MouseX, MouseY);

    if (GEngine->IsValidLowLevel())
    {
        GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(PickupUI.ToSharedRef()));
    }
}
