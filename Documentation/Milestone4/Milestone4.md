# Milestone 4 - Clickable objects with menu and rotation

## Objects Dropable 
Objekts can now be dropped by choosing the button for the drop. The object can than be moved with the mouse and dropped with the left mouse button. The following code shows the drop and since it is similar to the rotation it is also shown here.  
```
	// Rotate/drop the object 
	if (PickupComponent->bRotationStarted || PickupComponent->bDropStarted)
	{
  if (PickupComponent->bFreeMouse && !PickupComponent->bDropStarted)
  {
   PickupComponent->bRotationStarted = false;
   PlayerController->bShowMouseCursor = true;
   PlayerController->bEnableClickEvents = true;
   PlayerController->bEnableMouseOverEvents = true;
   return;
  }

		float XMousePositionCurrent;
		float YMousePositionCurrent;

		PlayerController->GetMousePosition(XMousePositionCurrent, YMousePositionCurrent);

		// Check if roation/drop just startet
		if (XMousePosition != .0f && YMousePosition != .0f)
		{
			XMousePosition -= XMousePositionCurrent;
			YMousePosition -= YMousePositionCurrent;

			if (PickupComponent->bRotationStarted)
			{
				FRotator ControlRotation = FRotator(YMousePosition, 0, XMousePosition);
				PickupComponent->ItemInRotaitonPosition->AddActorWorldRotation(ControlRotation.Quaternion());

    if (!PickupComponent->bRotating)
    {
        SetPlayerMovable(false);
        PlayerController->bShowMouseCursor = false;
        PlayerController->bEnableClickEvents = false;
        PlayerController->bEnableMouseOverEvents = false;
        // Sets the focus back to the screen after the mouse was freed
        PlayerController->SetInputMode(FInputModeGameOnly());
        PickupComponent->bRotating = true;
    }
			}
			else
			{
    if (!PickupComponent->bDropping)
    {
     SetPlayerMovable(false);
     PlayerController->bShowMouseCursor = false;
     PlayerController->bEnableClickEvents = false;
     PlayerController->bEnableMouseOverEvents = false;
     // Sets the focus back to the screen after the mouse was freed
     PlayerController->SetInputMode(FInputModeGameOnly());
     PickupComponent->bDropping = true;
    }
				FVector Distance = FVector(0, XMousePosition, YMousePosition) / 10;
				PickupComponent->ItemToHandle->AddActorLocalOffset(Distance);
			}				
		}

		XMousePosition = XMousePositionCurrent;
		YMousePosition = YMousePositionCurrent;
	}
```  

The following code shows the creation of the drop button.
```
	else if (!GameHUD->GPickup->bDropStarted && GameHUD->GPickup->ItemToHandle != nullptr && 
		(GameHUD->GPickup->ItemInLeftHand == GameHUD->GPickup->ItemToInteract || GameHUD->GPickup->ItemInRightHand == GameHUD->GPickup->ItemToInteract))
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
				.OnClicked(this, &SPickupUI::Drop)
			]
		];
	}
```
```
FReply SPickupUI::Drop()
{
	GameHUD->GPickup->bDropStarted = true;

	GameHUD->RemoveMenu();
 GameHUD->GPickup->bRightMouse = false;

	return FReply::Handled();
}
```

## Pure Plugin
The project is now a plugin which needs to be added to the plugin folder of the robcog project.

## Fixes and clean up
It is now not necessary to press the left mouse button after the mouse was freed to get the focus of the game back. This is done with the following code  
```
// Sets the focus back to the screen after the mouse was freed
  PlayerController->SetInputMode(FInputModeGameOnly());
```

Other small bugs were also fixed and not used code was removed.

# Bugs
* There is currently a bug that after an object was rotated the drop axis get inverted. A possible solution is to give the object a bool, that the object was rotated, check if that happend and invert the axis manuelly in the code.  

* Since the new Engine update it is often necessary to press the right mouse button more than once to open the menu on the objects.  
