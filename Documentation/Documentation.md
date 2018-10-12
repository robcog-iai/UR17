# Realistic Walking  
* The realistic walking for the character is implemented in [GMovment.h](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/Character/Components/GMovement.h) 
and [GMovment.cpp](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/Character/Components/GMovement.cpp). GMovment is the movment component which handles the movment with the keyboard button W, A, S and D.

* For the realisitc movment the curved function [PT1-Glied](https://de.wikipedia.org/wiki/PT1-Glied) was used, 
which increasses the movment speed until the maximum threshold is reached. [The TickComponent checks if the character is still moving and if not resets the speed up.](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/Character/Components/GMovement.cpp#L69-L80)

```
// Called every frame
void UGMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    // Check if the character is still moving. If not reset the CurrentSpeed and SpeedUpTime. 
    if (Character->GetVelocity() == FVector::ZeroVector && bIsMoving) {
        CurrentSpeed = 0;
        SpeedUpTime = 0.05;
        bIsMoving = false;
        UE_LOG(LogTemp, Warning, TEXT("Reset Speed!"));
    }
}
```

* [The actuel speed up is handled in the speed up function, which increassed the SpeedUpTime. For the calculation of the current speed the PT1-Glied function, in CalculateNewSpeed, computes the new speed. The new speed has to be adjusted to the correct direction and if necessary needs to be inverted.](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/Character/Components/GMovement.cpp#L102-142)

```
float UGMovement::CalculateNewSpeed(float TimeStep)
{
    // The variables are fixed and change the curve of the function, if changed
    int TVariable = 1;

    // Variables to hold the values from the function
    float Tmp = -(TimeStep / TVariable);
    float BracePart = (1 - FMath::Exp(Tmp));
    float NewSpeed = MaxMovementSpeed * BracePart;

    return NewSpeed;
}

void UGMovement::SpeedUp(const FVector Direction, const float Val)
{
    // Recalculate the current speed (by Waldemar Zeitler)
    if (!bIsMoving) {
        bIsMoving = true;
    }

    // Rising speed up
    if (SpeedUpTime < 2) {
        SpeedUpTime += GetWorld()->GetDeltaSeconds();
    }

    // If the max speed is not reached calculate the new and faster speed
    if (CurrentSpeed <= MaxMovementSpeed)
    {
        CurrentSpeed = CalculateNewSpeed(SpeedUpTime) + MinMovementSpeed;
        CurrentSpeed = CurrentSpeed * Val;
        UE_LOG(LogTemp, Warning, TEXT("Current Speed: %f"), CurrentSpeed);
    }

    // If the max speed is reached the movment for a negative/direction has to be switched accordingly
    if ((CurrentSpeed < 0 && Val > 0) || (CurrentSpeed > 0 && Val < 0))
    {
        CurrentSpeed = -CurrentSpeed;
    }

    Character->AddMovementInput(Direction, CurrentSpeed);
}
```

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

# Rotation, Pickup and Drop
GPickup handles the interaction calls with the objects and picks the objects up, while the GameController handles the movment and rotaiton of the objects ([GPickup.cpp](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/Character/Components/GPickup.cpp), [GameController.cpp](https://github.com/wzeitler/UR17/blob/ur17-p4/Source/UGame/Private/Character/GameController.cpp)).  
The right postion of the hands is set by three actors, for the right, left and middle position.
```
void UGPickup::SetupHands()
{
    // Create Static mesh actors for hands to weld items we pickup into this position
// Spawn new actors and set them up
    LeftHandActor = GetWorld()->SpawnActor<AStaticMeshActor>();
    RightHandActor = GetWorld()->SpawnActor<AStaticMeshActor>();
    BothHandActor = GetWorld()->SpawnActor<AStaticMeshActor>();

    LeftHandActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
    RightHandActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
    BothHandActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);

    FVector PlayerPosition = PlayerCharacter->GetTargetLocation();

    LeftHandActor->SetActorRelativeLocation(PlayerPosition + FVector(40, -40, 28));
    RightHandActor->SetActorRelativeLocation(PlayerPosition + FVector(40, 40, 28));
    BothHandActor->SetActorRelativeLocation(PlayerPosition + FVector(60, 0, 10));

    LeftHandActor->AttachToActor(PlayerCharacter, FAttachmentTransformRules::KeepWorldTransform);
    RightHandActor->AttachToActor(PlayerCharacter, FAttachmentTransformRules::KeepWorldTransform);
    BothHandActor->AttachToActor(PlayerCharacter, FAttachmentTransformRules::KeepWorldTransform);

    LeftHandActor->GetStaticMeshComponent()->SetSimulatePhysics(false);
    RightHandActor->GetStaticMeshComponent()->SetSimulatePhysics(false);
    BothHandActor->GetStaticMeshComponent()->SetSimulatePhysics(false);

    LeftHandActor->GetStaticMeshComponent()->SetCollisionProfileName("NoCollision");
    RightHandActor->GetStaticMeshComponent()->SetCollisionProfileName("NoCollision");
    BothHandActor->GetStaticMeshComponent()->SetCollisionProfileName("NoCollision");
}
```
The pickup and rotation interactions start by pressing the according button. Afterwards the object will be moved to the picked location and a falg will be set, to know in which state the character currently is.
```
void UGPickup::MoveToRotationPosition()
{   
    BaseItemToPick = ItemToHandle;

    // If we want to rotate an item from the hands the item in the hand has to be set to null.
    if (ItemInLeftHand == BaseItemToPick)
    {
        ItemInLeftHand = nullptr;
    }
    else if (ItemInRightHand == BaseItemToPick)
    {
        ItemInRightHand = nullptr;
    }

    FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepRelativeTransform;
    TransformRules.bWeldSimulatedBodies = true;

    BaseItemToPick->AttachToActor(BothHandActor, TransformRules);
    ItemInRotaitonPosition = BaseItemToPick;
    BaseItemToPick->SetActorRelativeLocation(FVector::ZeroVector, false, nullptr, ETeleportType::TeleportPhysics);
    BaseItemToPick->GetStaticMeshComponent()->SetSimulatePhysics(false);

    BaseItemToPick->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    bInRotationPosition = true;

    BaseItemToPick = nullptr;
    ItemToHandle = nullptr;   
}

void UGPickup::PickUpItemAfterMenu(bool leftHand)
{
    if (BaseItemToPick == nullptr) {
        if (ItemInRotaitonPosition != nullptr)
        {
            BaseItemToPick = ItemInRotaitonPosition;
            ItemInRotaitonPosition = nullptr;
        }
        else
        {
            BaseItemToPick = ItemToHandle;
        }
    }

    FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepWorldTransform;
    TransformRules.bWeldSimulatedBodies = true;

    if (!leftHand)
    {
        BaseItemToPick->AttachToActor(RightHandActor, TransformRules);

        ItemInRightHand = BaseItemToPick;
    }
    else
    {
        BaseItemToPick->AttachToActor(LeftHandActor, TransformRules);

        ItemInLeftHand = BaseItemToPick;
    }

    BaseItemToPick->SetActorRelativeLocation(FVector::ZeroVector, false, nullptr, ETeleportType::TeleportPhysics);

    BaseItemToPick->GetStaticMeshComponent()->SetSimulatePhysics(false);

    bInRotationPosition = false;
    bPickupAndRotationMenu = false;
    bPickupLeftRightHandMenu = false;
    bFreeMouse = false;

    ItemToHandle = nullptr;
    BaseItemToPick = nullptr;
}
```
The state of the character als indicates which action to take, so either rotate/drop the item or allow movment.
```
// Called to bind functionality to input
void AGameController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Default Camera view bindings
    PlayerInputComponent->BindAxis("CameraPitch", this, &AGameController::AddPitchInput);
    PlayerInputComponent->BindAxis("CameraYaw", this, &AGameController::AddYawInput);

    if (MovementComponent != nullptr) MovementComponent->SetupKeyBindings(PlayerInputComponent);
    if (PickupComponent != nullptr) PickupComponent->SetupKeyBindings(PlayerInputComponent);
}

void AGameController::AddPitchInput(const float Val) {
    // Rotate/drop the object 
    if (PickupComponent->bInRotationPosition)
    {    
        // Calculate the rotation and inverse it
        float RotationValue = Val * RotationRate * GetWorld()->GetDeltaSeconds();
        PickupComponent->ItemInRotaitonPosition->AddActorLocalRotation(FQuat(FRotator(RotationValue, 0, 0)), false, 0, ETeleportType::None);
    }
    else if (PickupComponent->bDropping)
    {
        // Calcualte the movment speed of the object
        float DropMovmentValue = Val * DropMovmentRate * GetWorld()->GetDeltaSeconds() * -1;
        // Move either left or right hand actor, where the item is attached to
        if (PickupComponent->bDroppingLeftHandItem)
        {
            PickupComponent->LeftHandActor->AddActorLocalOffset(FVector(0, 0, DropMovmentValue));
        }
        else
        {
            PickupComponent->RightHandActor->AddActorLocalOffset(FVector(0, 0, DropMovmentValue));
        }   
    }
    else if (MovementComponent->bCanMove == false)
    {
        return;
    }
    else
    {
        this->AddControllerPitchInput(Val);
    } 
}

void AGameController::AddYawInput(const float Val) {
    // Rotate/drop the object 
    if (PickupComponent->bInRotationPosition)
    {
        // Calculate the rotation and inverse it
        float RotationValue = Val * RotationRate * GetWorld()->GetDeltaSeconds();
        FRotator ControlRotation = FRotator(0, 0, RotationValue) + PickupComponent->ItemInRotaitonPosition->GetActorRotation();
        PickupComponent->ItemInRotaitonPosition->SetActorRotation(ControlRotation.Quaternion());;
    }
    else if (PickupComponent->bDropping)
    {
        // Calcualte the movment speed of the object
        float DropMovmentValue = Val * DropMovmentRate * GetWorld()->GetDeltaSeconds();
        // Move either left or right hand actor, where the item is attached to
        if (PickupComponent->bDroppingLeftHandItem)
        {
            PickupComponent->LeftHandActor->AddActorLocalOffset(FVector(0, DropMovmentValue, 0));
        }
        else
        {
            PickupComponent->RightHandActor->AddActorLocalOffset(FVector(0, DropMovmentValue, 0));
        }   
    }
    else if (MovementComponent->bCanMove == false)
    {
        return;
    }
    else
    {
        this->AddControllerYawInput(Val);
    }    
}
```
The code above also shows how the objects are moved and rotated. For the calculation of the position the movment value of the mouse gets multiplicated by DeltaSeconds and a value for the speed of the rotation/drop.  
After the rotation is finished the object can be clicked again to call a menu for picking it up into the right or left hand. For dropping the object it is necessary to set the physics for the object and detach it from the hand position.
```
void UGPickup::DropItem()
{
    ItemToHandle->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    ItemToHandle->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ItemToHandle->GetStaticMeshComponent()->SetSimulatePhysics(true);

    if (bDroppingRightHandItem)
    {
        ItemInRightHand = nullptr;
        bDroppingRightHandItem = false;
        RightHandActor->SetActorLocation(RightHandPosition);
    }      
    else if (bDroppingLeftHandItem)
    {
        ItemInLeftHand = nullptr;
        bDroppingLeftHandItem = false;
        LeftHandActor->SetActorLocation(LeftHandPosition);
    }

    ItemToHandle = nullptr;
    bFreeMouse = false;
}
```

# Bugs
* Since the update 4.20 of the unreal engine it is necessary to press the mouse buttons twice to start the interactions with the objects or remove the cursor.  

* Objects on the floor can be picked up and if they are moved to the rotation position the screen of the character stay at the lock down posiion. 
