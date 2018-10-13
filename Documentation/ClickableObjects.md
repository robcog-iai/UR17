# Clickable object and menu
Clicking objects and opening the menus is handled in the HUD files, which draw the menus and add a ertain style to it, and in GPickup, which also handles mouse interactions ([GPickup.cpp](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/Character/Components/GPickup.cpp), [GameHUD.cpp](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/HUD/GameHUD.cpp), [PickupUI.cpp](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/HUD/PickupUI.cpp)).  
By pressing the right mouse button the mouse cursor gets freed and can be used ti click items. The items are recognized by the given Tags, which are given to them item. By going through the list of iteractable items the pickup items can be filtered and given an mouse over event, to allow the menus to pop up.
```
 // Go through the pickup items and give them mouse over events
    for (AActor* InteractableItem : SetOfPickupItems)
    {
        InteractableItem->OnBeginCursorOver.AddDynamic(this, &UGPickup::CustomOnBeginMouseOver);
        InteractableItem->OnEndCursorOver.AddDynamic(this, &UGPickup::CustomOnEndMouseOver);
    }
```
```
void UGPickup::CustomOnBeginMouseOver(AActor* TouchedComponent)
{
    UE_LOG(LogTemp, Warning, TEXT("Mouse over"));

    bOverItem = true;
    ItemToInteract = TouchedComponent;
}

void UGPickup::CustomOnEndMouseOver(AActor* TouchedComponent)
{
    UE_LOG(LogTemp, Warning, TEXT("Mouse over end"));

    bOverItem = false;
    ItemToInteract = nullptr;
}
```
By pushing the left/right mouse button without psuhing on a menu buttont the freed cursor vanishes and the character can move again. If the right mouse button is pushed over an item the fitting menu will be displayed.
```
void UGPickup::InputLeftHandPressed()
{
    bLeftMouse = true;

    if (bDropping)
    {
        bDropping = false;
        DropItem();
    }

    bRightMouse = false;
    bFreeMouse = false;
}

void UGPickup::InputLeftHandReleased()
{
    bLeftMouse = false;
}

void UGPickup::InputRightHandPressed()
{
    if (bDropping)
    {
        bDropping = false;
        DropItem();
    } 
    else
    {
        bRightMouse = true;

        HandleRightClick();
    }   
    
    // Debug information, for the current state
    UE_LOG(LogTemp, Warning, TEXT("bPickUpStarted: %s"), bPickUpStarted ? TEXT("True\n") : TEXT("False\n"));
    UE_LOG(LogTemp, Warning, TEXT("bInRotationPosition: %s"), bInRotationPosition ? TEXT("True\n") : TEXT("False\n"));
    UE_LOG(LogTemp, Warning, TEXT("bFreeMouse: %s"), bFreeMouse ? TEXT("True\n") : TEXT("False\n"));
    UE_LOG(LogTemp, Warning, TEXT("bLeftMouse: %s"), bLeftMouse ? TEXT("True\n") : TEXT("False\n"));
    UE_LOG(LogTemp, Warning, TEXT("bRightMouse: %s"), bRightMouse ? TEXT("True\n") : TEXT("False\n"));
    UE_LOG(LogTemp, Warning, TEXT("bOverItem: %s"), bOverItem ? TEXT("True\n") : TEXT("False\n"));
}

void UGPickup::InputRightHandReleased()
{
    bRightMouse = false;
}

void UGPickup::HandleRightClick()
{
    // Right mouse was pressed, check which menu needs to be activated
    if (ItemToInteract != nullptr)
    {
        ItemToHandle = Cast<AStaticMeshActor>(ItemToInteract);

        // Item ist not in right or left hand
        if (!(ItemToHandle == ItemInLeftHand) && !(ItemToHandle == ItemInRightHand) && !bInRotationPosition 
            && (ItemInLeftHand == nullptr || ItemInRightHand == nullptr))
        {
            bPickupAndRotationMenu = true;
            bCallMenu = true;
        }
        // Item is in on of the hands, call the drop menu and safe the current hand position
        else if (ItemToHandle == ItemInLeftHand) 
        {
            bDroppingLeftHandItem = true;
            bDropItemMenu = true;
            bCallMenu = true;
            LeftHandPosition = LeftHandActor->GetActorLocation();
        }
        else if (ItemToHandle == ItemInRightHand)
        {
            bDroppingRightHandItem = true;
            bDropItemMenu = true;
            bCallMenu = true;
            RightHandPosition = RightHandActor->GetActorLocation();
        }
        else if (ItemToHandle == ItemInRotaitonPosition) // Item is in rotation position, call the left right pickup menu
        {
            bPickupLeftRightHandMenu = true;
            bCallMenu = true;
            
        }
        return;
    }
    // Nothing clicked, so remove the mouse cursor
    else
    {
        bFreeMouse = !bFreeMouse;
    }
}
```
The gamecontroller receives the meun call from GPickup and tells the GameHUD which menu should be opened.
```
    // Stop movment when the mouse is freed and show the mouse. If the current state allows it
    if (PickupComponent->bFreeMouse  && !PickupComponent->bDropping)
    {
        SetPlayerMovable(false);
        ShowCursor(true);

        // If one of the menus is true, call it
        if (PickupComponent->bRightMouse && PickupComponent->bCallMenu && !bMenuOpen)
        {
            float XMouse;
            float YMouse;

            PlayerController->GetMousePosition(XMouse, YMouse);
            PickupHUD->DrawPickUpMenu(XMouse, YMouse);

            bMenuOpen = true;
        }
    } 
    else if (!PickupComponent->bFreeMouse) // Allow the character to move again
    {
        SetPlayerMovable(true);
        ShowCursor(false);
        // Sets the focus back to the screen after the mouse was freed
        PlayerController->SetInputMode(FInputModeGameOnly());

        bMenuOpen = false;
    }
```
The GameHUD receives the mouse information form the gamecontroller, to draw the menu at the right position. For the drawing of the menu it is necessary to call the PickupUI, which handles the menu creation.
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
With the SLATE library and the specific code the menus are created.
```
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
    // Calls for the different menus, depending on the current state
    if (GameHUD->GPickup->bPickupAndRotationMenu && !GameHUD->GPickup->bInRotationPosition) {
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
    else if (GameHUD->GPickup->bDropItemMenu && !GameHUD->GPickup->bInRotationPosition)
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
    else if (GameHUD->GPickup->bPickupLeftRightHandMenu)
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
```
The above code calls the following functions to further interact with the menu and the right approach of the buttons. GetActionsWidgetPos is necessary to draw the menu at the right position on the screen space.
```
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

    GameHUD->RemoveMenu();

    return FReply::Handled();
}

FReply SPickupUI::Rotate()
{
    GameHUD->GPickup->MoveToRotationPosition();

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

    GameHUD->RemoveMenu();

    return FReply::Handled();
}

FReply SPickupUI::Drop()
{
    GameHUD->GPickup->bDropping = true;

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
```
After every menu button the function RemoveMenu is called, which sets the right flags and removes the menu.
```
void AGameHUD::RemoveMenu()
{
    GPickup->bFreeMouse = false;
    GPickup->bRightMouse = false;
    GPickup->bCallMenu = false;

    GEngine->GameViewport->RemoveAllViewportWidgets();
}
```
