// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ButtonGrid.h"
#include "UGame.h"
#include "GameHUD.h"

void SButtonGrid::Construct(const FArguments& InArgs)
{
	GameHUD = InArgs._GameHUD;

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.Text(FText::FromString("Pick Up Left Hand (Left Mouse Button)"))
			]
			+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.Text(FText::FromString("Pick Up Reft Hand (Right Mouse Button)"))
			]
		]
	];
}