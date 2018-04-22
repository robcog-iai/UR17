// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PickupUI.h"
#include "UGame.h"
#include "Engine.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPickupUI::Construct(const FArguments& args)
{
	GameHUD = args._GameHUD;

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SCanvas)
			+ SCanvas::Slot()
			.Position(TAttribute<FVector2D>(this, &SPickupUI::GetActionsWidgetPos))
			.Size(FVector2D(600, 800))
			[
				SAssignNew(ActionGrid, SGridPanel)
			]
		]
	];

	if (true) {
		ActionGrid->AddSlot(1,1)
		[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.Text(FText::FromString("Rotate Object"))
				.OnClicked(this, &SPickupUI::PickUpLeft)
			]
		+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.Text(FText::FromString("Pick Object Up"))
				.OnClicked(this, &SPickupUI::PickUpRight)
			]
		];
	}
	else {
		ActionGrid->AddSlot(1, 1)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.Text(FText::FromString("Rotate Object"))
				//.OnClicked(this, &SRotationUI::Rotation)
			]
			+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.Text(FText::FromString("Pick Object Up"))
				//.OnClicked(this, &SRotationUI::PickUp)
			]
		];
	}
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SPickupUI::PickUpLeft()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Rotation"));
	}

	// actually the BlueprintImplementable function of the HUD is not called; uncomment if you want to handle the OnClick via Blueprint
	//MainMenuHUD->PlayGameClicked();
	return FReply::Handled();
}

FReply SPickupUI::PickUpRight()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("PickUp"));
	}

	// actually the BlueprintImplementable function of the HUD is not called; uncomment if you want to handle the OnClick via Blueprint
	//MainMenuHUD->QuitGameClicked();
	return FReply::Handled();
}

FVector2D SPickupUI::GetActionsWidgetPos() const
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Mouseposition returned."));
	return WidgetPosition;
}