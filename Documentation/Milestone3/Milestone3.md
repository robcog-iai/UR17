# Milestone 3 - Clickable objects with menu and rotation

## Menu files

* The GameHUD class is the controller of the the three different menus that are currently available. To choose the right menu GameHUD has an instance UGPickup and for the correct position it receives the mouse position for drawing the menu. The menu is drawn by assigning the PickupUI and giving it the current mouse position. Over the GEngine PickupUI gets called and added to the viewport.  

```
void AGameHUD::DrawPickUpMenu(float MouseX, float MouseY)
{
	SAssignNew(PickupUI, SPickupUI).GameHUD(this);
	PickupUI->WidgetPosition = FVector2D(MouseX, MouseY);

	if (GEngine->IsValidLowLevel())
	{
		GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(PickupUI.ToSharedRef()));
	}
}
```  

The buttons are then removed with a call to GEngine.

```
void AGameHUD::RemoveMenu()
{
	GEngine->GameViewport->RemoveAllViewportWidgets();
}
```

* In PickupUI the actuell buttons are than drawn. The header file defines the button functions and holds the necessary variables to draw the buttons with a certain style and at the correct poition. For the drawing of the button it ist necessary to create a ChildSlot with an SOverlay and a SCanvas, to able to draw at position on the screen. This setup is slate specific. The buttons are then defined as ActionGrid. To get the correct button the UI needs to know in which state the pick up is currently, this is done with GameHUD which has referenc to GPickup. The following code shows the construction of the Rotate and PickUp buttons. Following this is an "else if" which asks if an object is hold for the Drop button and an "else" for the pick up after the rotation.

```
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
```

* The pick up and rotation buttons are setting falgs in the GPickup object, to indicate if the object should be rotated or picked up. One special thing about the pick up is that it is checked if the object is on the right or left sied of the chracter and is picked up in the closer hand. 

```
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
```

* The ButtonStyle files just handle the look of the buttons and don't include relevant code.

## Character
* The character handles the falgs it receives from GPickup. On right mouse click the character stops and becomes possible to move around with the mouse. If an object is close enough it can be clicked on and a menu pops up at the position of the mouse.

```
	// Stop movment when menu is active 
	if (PickupComponent->bFreeMouse && !bIsMovementLocked)
	{
		SetPlayerMovable(false);
		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;
		PickupComponent->bRotationStarted = false;
	}

	if (PickupComponent->bFreeMouse && !PickupComponent->bRightMouse && PickupComponent->bPickupMenuActivated && PickupComponent->bOverItem)
	{
		XMousePosition = .0f;
		YMousePosition = .0f;

		float XMouse;
		float YMouse;

		PlayerController->GetMousePosition(XMouse, YMouse);
		PickupHUD->DrawPickUpMenu(XMouse, YMouse);
	}
	else if (!PickupComponent->bFreeMouse && bIsMovementLocked)
	{
		SetPlayerMovable(true);
		PlayerController->bShowMouseCursor = false;
		PlayerController->bEnableClickEvents = false;
		PlayerController->bEnableMouseOverEvents = false;
	}
```

* The rotation is now reworked, so it works correctly with the mosue movement. To get the rotation the past and present position of the mouse are subtracted and the different ist added to the current rotation of the object.

```
	// Rotate the object depending of the rotation mode 
	if (PickupComponent->bRotationStarted)
	{
		float XMousePositionCurrent;
		float YMousePositionCurrent;

		PlayerController->GetMousePosition(XMousePositionCurrent, YMousePositionCurrent);

		// Check if roation just startet
		if (XMousePosition != .0f && YMousePosition != .0f)
		{
			XMousePosition -= XMousePositionCurrent;
			YMousePosition -= YMousePositionCurrent;

			ControlRotation = FRotator(YMousePosition, 0, XMousePosition);

			PickupComponent->ItemInRotaitonPosition->AddActorWorldRotation(ControlRotation.Quaternion());
		}

		XMousePosition = XMousePositionCurrent;
		YMousePosition = YMousePositionCurrent;

		UE_LOG(LogTemp, Warning, TEXT("Actor Rotation %s"), *PickupComponent->ItemInRotaitonPosition->GetActorRotation().ToString());
	}
```

## Pick up
