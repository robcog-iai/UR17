// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

/**
* SPickupGameUI.h – Provides an implementation of the Slate UI representing the game menu.
*/

#pragma once

#include "SlateBasics.h"
#include "GameHUD.h"

// Lays out and controls the Main Menu UI for our tutorial.
class UGAME_API SPickupUI : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SPickupUI)
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<class AGameHUD>, GameHUD)
	SLATE_END_ARGS()

	// Constructs and lays out the Main Menu UI Widget.
	// args Arguments structure that contains widget-specific setup information.
	void Construct(const FArguments& args);

	// Click handler for the pickup button.
	FReply PickUp();

 // Click handler for the rotation button .
 FReply Rotate();

 // Click handler for the pickup after rotation button.
 FReply PickUpAfterRotation(bool bLeftHand);

	// Stores a weak reference to the HUD controlling this class.
	TWeakObjectPtr<class AGameHUD> GameHUD;

	TSharedPtr<SGridPanel> ActionGrid;

 // Position on screen
 TAttribute<FVector2D> WidgetPosition;

 bool bInRotationMode;

private:

 FVector2D ButtonPosition;

	// Get the position where to draw the menu
	FVector2D GetActionsWidgetPos() const;
};
