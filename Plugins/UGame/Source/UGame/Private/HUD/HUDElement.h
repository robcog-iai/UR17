// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#pragma once

#include "Engine.h"
#include "GameFramework/HUD.h"
#include "HUDElement.generated.h"

/**
*
*/
UCLASS()
class UGAME_API AHUDElement : public AHUD
{
	GENERATED_BODY()

protected:
	// This will be drawn at the center of the screen.
	UPROPERTY(EditDefaultsOnly)
		UTexture2D* CrosshairTexture;

public:
	// Primary draw call for the HUD.
	virtual void DrawHUD() override;
};