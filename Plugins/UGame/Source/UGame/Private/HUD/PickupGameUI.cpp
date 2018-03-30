// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PickupGameUI.h"
#include "UGame.h"
#include "Engine.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPickupGameUI::Construct(const FArguments& args)
{
	GameHUD = args._GameHUD;

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
			.Text(FText::FromString("Pick Up lLeft Hand (Left Mouse Button)"))
		.OnClicked(this, &SPickupGameUI::PickUpLeft)
		]
	+ SVerticalBox::Slot()
		[
			SNew(SButton)
			.Text(FText::FromString("Pick Up Reft Hand (Right Mouse Button)"))
		.OnClicked(this, &SPickupGameUI::PickUpRight)
		]
		]
		];

}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SPickupGameUI::PickUpLeft()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Rotation"));
	}

	// actually the BlueprintImplementable function of the HUD is not called; uncomment if you want to handle the OnClick via Blueprint
	//MainMenuHUD->PlayGameClicked();
	return FReply::Handled();
}

FReply SPickupGameUI::PickUpRight()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("PickUp"));
	}

	// actually the BlueprintImplementable function of the HUD is not called; uncomment if you want to handle the OnClick via Blueprint
	//MainMenuHUD->QuitGameClicked();
	return FReply::Handled();
}