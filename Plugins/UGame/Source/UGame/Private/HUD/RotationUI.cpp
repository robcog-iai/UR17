// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "RotationUI.h"
#include "UGame.h"
#include "Engine.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SRotationUI::Construct(const FArguments& args)
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
			.Position(TAttribute<FVector2D>(this, &SRotationUI::GetActionsWidgetPos))
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
						.Text(FText::FromString("Rotate Object"))
						.OnClicked(this, &SRotationUI::Rotation)
					]
					+ SVerticalBox::Slot()
					[
						SNew(SButton)
						.Text(FText::FromString("Pick Object Up"))
						.OnClicked(this, &SRotationUI::PickUp)
					]
				]
			]
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SRotationUI::Rotation()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Rotation"));
	}

	// actually the BlueprintImplementable function of the HUD is not called; uncomment if you want to handle the OnClick via Blueprint
	//MainMenuHUD->PlayGameClicked();
	return FReply::Handled();
}

FReply SRotationUI::PickUp()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("PickUp"));
	}

	// actually the BlueprintImplementable function of the HUD is not called; uncomment if you want to handle the OnClick via Blueprint
	//MainMenuHUD->QuitGameClicked();
	return FReply::Handled();
}

FVector2D SRotationUI::GetActionsWidgetPos() const
{
	return FVector2D(100, 100);
}