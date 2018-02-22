// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "GameHUD.h"
#include "UGame.h"
#include "GameUI.h"
#include "Engine.h"

void AGameHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SAssignNew(GameUI, SGameUI).GameHUD(this);

	if (GEngine->IsValidLowLevel())
	{
		GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(GameUI.ToSharedRef()));
	}
}