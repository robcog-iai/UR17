// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ButtonGrid.h"
#include "UGame.h"
#include "GameHUD.h"
#include "Engine/Console.h"

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
			SNew(SCanvas)
			+SCanvas::Slot()
			.Position(TAttribute<FVector2D>(this, &SButtonGrid::GetActionsWidgetPos))
			.Size(FVector2D(600, 800))
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
			]			
		]
	];
}

FVector2D SButtonGrid::GetActionsWidgetPos() const
{
	return FVector2D(100,100);
}