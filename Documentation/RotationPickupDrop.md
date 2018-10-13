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
