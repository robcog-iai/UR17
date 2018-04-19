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

void AGameHUD::DrawHUD()
{
	if (CrosshairTexture)
	{
		// Find the center of our canvas.
		FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

		// Offset by half of the texture's dimensions so that the center of the texture aligns with the center of the Canvas.
		FVector2D CrossHairDrawPosition(Center.X - (CrosshairTexture->GetSurfaceWidth() * 0.5f), Center.Y - (CrosshairTexture->GetSurfaceHeight() * 0.5f));

		// Draw the crosshair at the centerpoint.
		FCanvasTileItem TileItem(CrossHairDrawPosition, CrosshairTexture->Resource, FLinearColor::White);

		TileItem.BlendMode = SE_BLEND_Translucent;

		Canvas->DrawItem(TileItem);
	}
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