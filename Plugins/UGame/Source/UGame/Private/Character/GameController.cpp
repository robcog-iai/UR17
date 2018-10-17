// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#define TAG_KEY_INTERACTABLE "Interactable"

#include "GameController.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "TagStatics.h"
#include "Engine.h" // Needed for GEngine
#include "GameFramework/InputSettings.h"

AGameController::AGameController()
{
    PrimaryActorTick.bCanEverTick = true;

    // Can be adjusted in the editor for faster/slower drop and rotation
    RotationRate = 300;
    DropMovmentRate = 150;

    // Allowed distance to move from and to the character
    AllowedDepthDistance = 70;

    // Set PlayerController 
    PlayerController = nullptr;

    // Setup for the components
    MovementComponent = CreateDefaultSubobject<UGMovement>(TEXT("Movement Component"));

    PickupComponent = CreateDefaultSubobject<UGPickup>(TEXT("Pickup Component"));
    PickupComponent->PlayerCharacter = this;

    GetCapsuleComponent()->SetCapsuleRadius(0.01f);

    bMenuOpen = false;
}

// Called when the game starts or when spawned
void AGameController::BeginPlay()
{
    Super::BeginPlay();

    // Initilize the player controller to get the mouse axis
    PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController) {
        UE_LOG(LogTemp, Warning, TEXT("Player controller was not set."));
    }

    // Allow mouse over events, for the menu pop up over the items.
    PlayerController->bEnableMouseOverEvents = true;

    // *** *** *** *** *** ***

    // Setup HUD
    PickupHUD = Cast<AGameHUD>(PlayerController->GetHUD());
    PickupHUD->GPickup = PickupComponent;
}

// Called every frame
void AGameController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PickupComponent == nullptr) UE_LOG(LogTemp, Warning, TEXT("NULL"));

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
}

// Called to bind functionality to input
void AGameController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Default Camera view bindings
    PlayerInputComponent->BindAxis("CameraPitch", this, &AGameController::AddPitchInput);
    PlayerInputComponent->BindAxis("CameraYaw", this, &AGameController::AddYawInput);

    // Action button for the dropping into the depth and rotating around the own axis
    PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AGameController::ActionPressed);
    PlayerInputComponent->BindAction("Action", IE_Released, this, &AGameController::ActionReleased);

    if (MovementComponent != nullptr) MovementComponent->SetupKeyBindings(PlayerInputComponent);
    if (PickupComponent != nullptr) PickupComponent->SetupKeyBindings(PlayerInputComponent);
}

void AGameController::AddPitchInput(const float Val) {
    // Rotate/drop the object 
    if (PickupComponent->bInRotationPosition)
    {    
        // Calculate the rotation and inverse it
        float RotationValue = Val * RotationRate * GetWorld()->GetDeltaSeconds();

        // Check if the object should be rotated around the own axis
        if (!bActionButtonHold)
        {
            PickupComponent->ItemInRotaitonPosition->AddActorLocalRotation(FQuat(FRotator(RotationValue, 0, 0)), false, 0, ETeleportType::None);
        }
        else
        {
            FRotator ControlRotation = FRotator(0, RotationValue, 0) + PickupComponent->ItemInRotaitonPosition->GetActorRotation();
            PickupComponent->ItemInRotaitonPosition->SetActorRotation(ControlRotation.Quaternion());
        }
        
    }
    else if (PickupComponent->bDropping)
    {
        // Calcualte the movment speed of the object
        float DropMovmentValue = Val * DropMovmentRate * GetWorld()->GetDeltaSeconds() * -1;

        // Move either left or right hand actor, where the item is attached to
        if (PickupComponent->bDroppingLeftHandItem)
        {
            // Calculate to the distance to the moving object
            float DistanceToObject = CalculateDistanceToObject(PickupComponent->LeftHandActor->GetActorLocation());

            // Check if the object should be moved into the depth
            if (!bActionButtonHold)
            {
                PickupComponent->LeftHandActor->AddActorLocalOffset(FVector(0, 0, DropMovmentValue));
            }
            else if (DistanceToObject < AllowedDepthDistance && Val < 0) // Check if movment is still allowed
            {
                PickupComponent->LeftHandActor->AddActorLocalOffset(FVector(DropMovmentValue, 0, 0));
            }
            else if (DistanceToObject > 50 && Val > 0) // The distance has to be in a certain range to not move the object out of the camera view
            {
                PickupComponent->LeftHandActor->AddActorLocalOffset(FVector(DropMovmentValue, 0, 0));
            }
            
        }
        else
        {
            // Calculate to the distance to the moving object
            float DistanceToObject = CalculateDistanceToObject(PickupComponent->RightHandActor->GetActorLocation());

            // Check if the object should be moved into the depth
            if (!bActionButtonHold)
            {
                PickupComponent->RightHandActor->AddActorLocalOffset(FVector(0, 0, DropMovmentValue));
            }
            else if(DistanceToObject < AllowedDepthDistance && Val < 0) // Check if movment is still allowed
            {
                PickupComponent->RightHandActor->AddActorLocalOffset(FVector(DropMovmentValue, 0, 0));
            } 
            else if (DistanceToObject > 50 && Val > 0) // The distance has to be in a certain range to not move the object out of the camera view
            {
                PickupComponent->RightHandActor->AddActorLocalOffset(FVector(DropMovmentValue, 0, 0));
            }
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
        PickupComponent->ItemInRotaitonPosition->SetActorRotation(ControlRotation.Quaternion());
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

void AGameController::ActionPressed()
{
    bActionButtonHold = true;
}

void AGameController::ActionReleased()
{
    bActionButtonHold = false;
}

void AGameController::SetPlayerMovable(bool bIsMovable)
{
    if (MovementComponent != nullptr)
    {
        bIsMovementLocked = !bIsMovable;
        MovementComponent->SetMovable(bIsMovable);
    }
}

float AGameController::CalculateDistanceToObject(FVector ItemPosition)
{
    return FVector::Distance(this->GetActorLocation(), ItemPosition);
}

void AGameController::SetupRotationOrDrop()
{
    SetPlayerMovable(false);
    ShowCursor(false);
    // Sets the focus back to the screen after the mouse was freed
    PlayerController->SetInputMode(FInputModeGameOnly());
    PickupComponent->bDropping = true;
}


void AGameController::ShowCursor(bool bShow)
{
    PlayerController->bShowMouseCursor = bShow;
    PlayerController->bEnableClickEvents = bShow;
    PlayerController->bEnableMouseOverEvents = bShow;
}
