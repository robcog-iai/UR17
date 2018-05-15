// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

/**
* SPickupGameUI.h – Provides an implementation of the Slate UI representing the game menu.
*/

#pragma once

#include "SlateBasics.h"
#include "../Character/Components/GPickup.h"
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

	// Click handler for the Play Game! button – Calls MenuHUD’s PlayGameClicked() event.
	FReply PickUpLeft();

	// Click handler for the Quit Game button – Calls MenuHUD’s QuitGameClicked() event.
	FReply PickUpRight();

	// Stores a weak reference to the HUD controlling this class.
	TWeakObjectPtr<class AGameHUD> GameHUD;

	TSharedPtr<SGridPanel> ActionGrid;

 // Position on screen
 TAttribute<FVector2D> WidgetPosition;

 UGPickup GPickup;

private:

	// Get the position where to draw the menu
	FVector2D GetActionsWidgetPos() const;
};
