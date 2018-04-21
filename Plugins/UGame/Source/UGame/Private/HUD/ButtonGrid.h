// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "PickupUI.h"
#include "SlateExtras.h"

class AGameHUD;

// Grid of actions something can perform
class SButtonGrid : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SButtonGrid)
	{}

	SLATE_ARGUMENT(TWeakObjectPtr<AGameHUD>, GameHUD)

	SLATE_END_ARGS()

	/** needed for every widget */
	void Construct(const FArguments& InArgs);

	/** 3 x 3 grid of action buttons */
	//TArray< TSharedPtr<FActionButtonInfo> > ActionButtons;

	/** Shared pointer to action grid panel */
	TSharedPtr<SGridPanel> ActionGrid;

	TSharedPtr<SPickupUI> Pickup;

	// Stores a weak reference to the HUD controlling this class.
	TWeakObjectPtr<class AGameHUD> GameHUD;

	FVector2D GetActionsWidgetPos() const;
};
