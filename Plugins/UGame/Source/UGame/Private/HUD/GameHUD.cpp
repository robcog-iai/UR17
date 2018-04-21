// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "GameHUD.h"
#include "UGame.h"
#include "../Character/GameController.h"
#include "RotationUI.h"
#include "PickupUI.h"
#include "Engine.h"

AGameHUD::AGameHUD()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> FoundTexture(TEXT("Texture2D'/Game/Images/crosshair.crosshair'"));
	if (FoundTexture.Succeeded())
	{
		CrosshairTexture = FoundTexture.Object;
	}
}

void AGameHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();	
}

void AGameHUD::DrawMenu()
{
	SAssignNew(RotationUI, SRotationUI).GameHUD(this);

	if (GEngine->IsValidLowLevel())
	{
		GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(RotationUI.ToSharedRef()));
	}
}

void AGameHUD::RemoveMenu()
{
	GEngine->GameViewport->RemoveAllViewportWidgets();
}

void AGameHUD::DrawPickUpMenu()
{
	SAssignNew(PickupUI, SPickupUI).GameHUD(this);

	if (GEngine->IsValidLowLevel())
	{
		GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(PickupUI.ToSharedRef()));
	}
}

void AGameHUD::SetActionGridActor(AActor* InSelectedActor)
{
	SelectedActor.Reset();
	SelectedActor = MakeWeakObjectPtr(InSelectedActor);
}

AGameController * AGameHUD::GetPlayerController() const
{
	return Cast<AGameController>(PlayerOwner);
}