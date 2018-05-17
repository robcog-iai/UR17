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
			[
				SAssignNew(ActionGrid, SGridPanel)
			]
		]
	];

	if (true) {
		ActionGrid->AddSlot(0,0)
		[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
   .VAlign(VAlign_Top)
   .HAlign(HAlign_Center)
   .AutoHeight()
			[
				SNew(SButton)
				.Text(FText::FromString("Rotate Object"))
				.OnClicked(this, &SPickupUI::PickUp)
			]
		+ SVerticalBox::Slot()
   .VAlign(VAlign_Top)
   .HAlign(HAlign_Center)
			[
				SNew(SButton)
				.Text(FText::FromString("Pick Object Up"))
				.OnClicked(this, &SPickupUI::PickUp)
			]
		];
	}
	else {
		ActionGrid->AddSlot(0, 0)
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

FReply SPickupUI::PickUp()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("PickUp"));
	}

 // To check if the mouse is one the right or left side
 FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
 FVector2D ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

 // Pickup in right or left hand
 if (ViewportCenter < WidgetPosition.Get())
 {
  GameHUD->GPickup->PickUpItemAfterMenu(false);
 } 
 else
 {
  GameHUD->GPickup->PickUpItemAfterMenu(true);
 }

	return FReply::Handled();
}

FVector2D SPickupUI::GetActionsWidgetPos() const
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Mouseposition returned."));

 // The viewport is necessary to get the excat mouseposition on the screen
 FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
 float ViewportScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));

	return WidgetPosition.Get() / ViewportScale;
}