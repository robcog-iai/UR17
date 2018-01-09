// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: David Brinkmann

#include "CIHUD.h"


void ACIHUD::DrawHUD()
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

	if (StabilityTestSceneCaptureTexture != nullptr && bStabilityCheckScreenEnabled) {
		FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
		FVector2D DrawPosition(Center.X - (StabilityTestSceneCaptureTexture->GetSurfaceWidth() * 0.5f), Center.Y - (StabilityTestSceneCaptureTexture->GetSurfaceHeight() * 0.5f));
		
		// Draw the crosshair at the centerpoint.
		FCanvasTileItem TileItem(DrawPosition, StabilityTestSceneCaptureTexture->Resource, FLinearColor::White);

		Canvas->DrawItem(TileItem);
	}
}
