// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: David Brinkmann

#pragma once


#include "Engine.h"
#include "GameFramework/HUD.h"
#include "CIHUD.generated.h"

/**
 *
 */
UCLASS()
class CLICKINTERACTION_API ACIHUD : public AHUD
{
	GENERATED_BODY()

protected:
	// This will be drawn at the center of the screen.
	UPROPERTY(EditDefaultsOnly)
		UTexture2D* CrosshairTexture;

	UPROPERTY(EditAnywhere)
		UTextureRenderTarget2D* StabilityTestSceneCaptureTexture;
	

public:
	// Primary draw call for the HUD.
	virtual void DrawHUD() override;

	bool bStabilityCheckScreenEnabled;

};
