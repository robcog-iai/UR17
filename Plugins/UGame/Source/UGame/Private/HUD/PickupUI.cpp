// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PickupUI.h"
#include "UGame.h"
#include "Runtime/Engine/Classes/Engine/UserInterfaceSettings.h"
#include "Runtime/Engine/Classes/Engine/RendererSettings.h"
#include "Engine.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPickupUI::Construct(const FArguments& args)
{
	GameHUD = args._GameHUD;
	ButtonStyle = &FGameButtonStyles::Get().GetWidgetStyle<FGlobalStyle>("ButtonStyle");

	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Left)
		[
			SNew(SCanvas)
			+ SCanvas::Slot()
			.Position(TAttribute<FVector2D>(this, &SPickupUI::GetActionsWidgetPos))
			.Size(FVector2D(400, 400))
			[
			SAssignNew(ActionGrid, SGridPanel)
			]
		]
	];

	if (!GameHUD->GPickup->bInRotationPosition && GameHUD->GPickup->ItemInLeftHand == nullptr && GameHUD->GPickup->ItemInRightHand == nullptr) {
		ActionGrid->AddSlot(0, 0)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				SNew(SButton)
				.ButtonStyle(&ButtonStyle->MenuButtonStyle)
				.TextStyle(&ButtonStyle->MenuButtonTextStyle)
				.Text(FText::FromString("Rotate Object"))
				.OnClicked(this, &SPickupUI::Rotate)
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Center)
			[
				SNew(SButton)
				.ButtonStyle(&ButtonStyle->MenuButtonStyle)
				.TextStyle(&ButtonStyle->MenuButtonTextStyle)
				.Text(FText::FromString("Pick Object Up"))
				.OnClicked(this, &SPickupUI::PickUp)
			]
		];
	}
	else if (GameHUD->GPickup->ItemInLeftHand != nullptr || GameHUD->GPickup->ItemInRightHand != nullptr)
	{
		ActionGrid->AddSlot(0, 0)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.ButtonStyle(&ButtonStyle->MenuButtonStyle)
				.TextStyle(&ButtonStyle->MenuButtonTextStyle)
				.Text(FText::FromString("Drop Item"))
			]
		];
	}
	else
	{
		ActionGrid->AddSlot(0, 0)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.ButtonStyle(&ButtonStyle->MenuButtonStyle)
				.TextStyle(&ButtonStyle->MenuButtonTextStyle)
				.Text(FText::FromString("Put in left hand"))
				.OnClicked(this, &SPickupUI::PickUpAfterRotation, true)
			]
			+ SVerticalBox::Slot()
			[
				SNew(SButton)
				.ButtonStyle(&ButtonStyle->MenuButtonStyle)
				.TextStyle(&ButtonStyle->MenuButtonTextStyle)
				.Text(FText::FromString("Put in right hand"))
				.OnClicked(this, &SPickupUI::PickUpAfterRotation, false)
			]
		];
	}
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SPickupUI::PickUp()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("PickUp"));
	}

	// To check if the mouse is one the right or left side
	FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	FVector2D ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

	// Pickup in right or left hand. If both are used the item will be left.
	if (ViewportCenter < WidgetPosition.Get())
	{
		if (GameHUD->GPickup->ItemInRightHand == nullptr)
		{
			GameHUD->GPickup->PickUpItemAfterMenu(false);
		}
		else if (GameHUD->GPickup->ItemInLeftHand == nullptr)
		{
			GameHUD->GPickup->PickUpItemAfterMenu(true);
		}
	}
	else
	{
		if (GameHUD->GPickup->ItemInLeftHand == nullptr)
		{
			GameHUD->GPickup->PickUpItemAfterMenu(true);
		}
		else if (GameHUD->GPickup->ItemInRightHand == nullptr)
		{
			GameHUD->GPickup->PickUpItemAfterMenu(false);
		}
	}
	GameHUD->GPickup->bPickupMenuActivated = false;
	GameHUD->GPickup->bFreeMouse = false;
	GameHUD->RemoveMenu();

	return FReply::Handled();
}

FReply SPickupUI::Rotate()
{
	GameHUD->GPickup->MoveToRotationPosition();
	GameHUD->GPickup->bFreeMouse = false;

	GameHUD->RemoveMenu();

	return FReply::Handled();
}

FReply SPickupUI::PickUpAfterRotation(bool bLeftHand)
{
	if (bLeftHand)
	{
		if (GameHUD->GPickup->ItemInLeftHand == nullptr)
		{
			GameHUD->GPickup->PickUpItemAfterMenu(true);
		}
		else if (GameHUD->GPickup->ItemInRightHand == nullptr)
		{
			GameHUD->GPickup->PickUpItemAfterMenu(false);
		}
	}
	else
	{
		if (GameHUD->GPickup->ItemInRightHand == nullptr)
		{
			GameHUD->GPickup->PickUpItemAfterMenu(false);
		}
		else if (GameHUD->GPickup->ItemInLeftHand == nullptr)
		{
			GameHUD->GPickup->PickUpItemAfterMenu(true);
		}
	}

	GameHUD->GPickup->bPickupMenuActivated = false;
	GameHUD->GPickup->bFreeMouse = false;
	GameHUD->RemoveMenu();

	return FReply::Handled();
}


FVector2D SPickupUI::GetActionsWidgetPos() const
{
	// The viewport is necessary to get the excat mouseposition on the screen
	FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	float ViewportScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));

	return WidgetPosition.Get() / ViewportScale;
}