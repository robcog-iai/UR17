// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#define TAG_KEY_INTERACTABLE "Interactable"
#define CAMERA_TAG "Runtime,Dynamic;Class,CharacterCamera;Id,Abci;"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 100000.0f,FColor::White,text, false);
#define printAndStop(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 100000.0f,FColor::Red,text, false); GetWorld()->GetFirstPlayerController()->SetPause(true);
#define logText(text) UE_LOG(LogTemp, Warning, TEXT(text));
#define logAndStop(text) UE_LOG(LogTemp, Warning, TEXT(text)); GetWorld()->GetFirstPlayerController()->SetPause(true);

#include "GameController.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "TagStatics.h"
#include "Engine.h" // Needed for GEngine

// Sets default values
AGameController::AGameController()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Chagned to 300 (from 120) for testing
	GraspRange = 100.0f;

	// Set PlayerController (by Waldemar Zeitler)
	PlayerController = nullptr;

	// Setup for the components
	MovementComponent = CreateDefaultSubobject<UGMovement>(TEXT("Movement Component"));

	PickupComponent = CreateDefaultSubobject<UGPickup>(TEXT("Pickup Component"));
	PickupComponent->PlayerCharacter = this;

	XMousePosition = .0f;
	YMousePosition = .0f;

	GetCapsuleComponent()->SetCapsuleRadius(0.01f);
}

// Called when the game starts or when spawned
void AGameController::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ACharacter>(this);
	if (Character == nullptr)
	{
		printAndStop("ACharacterController::BeginPlay: Character is null. Game paused to prevent crash.");
		Character->PlayerState->bIsSpectator = true;
	}

	// Initilize the player controller to get the mouse axis (by Wlademar Zeitler)
	PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) {
		UE_LOG(LogTemp, Warning, TEXT("Player controller was not set."));
	}
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

	// Stop movment when menu is active 
	if (PickupComponent->bFreeMouse && !bIsMovementLocked && !PickupComponent->bDropStarted && !PickupComponent->bDropping && !PickupComponent->bRotating)
	{
		SetPlayerMovable(false);
		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;
		PickupComponent->bRotationStarted = false;
	}

	if (PickupComponent->bFreeMouse && PickupComponent->bRightMouse && PickupComponent->bPickupMenuActivated && PickupComponent->bOverItem
		&& !(PickupComponent->bRotationStarted || PickupComponent->bDropStarted) && !PickupComponent->bDropping && !PickupComponent->bRotating)
	{
		XMousePosition = .0f;
		YMousePosition = .0f;

		float XMouse;
		float YMouse;

		PlayerController->GetMousePosition(XMouse, YMouse);
		PickupHUD->DrawPickUpMenu(XMouse, YMouse);
	}
	else if (!PickupComponent->bFreeMouse && bIsMovementLocked && !PickupComponent->bDropStarted && !PickupComponent->bDropping
     && !PickupComponent->bRotating)
	{
		SetPlayerMovable(true);
		PlayerController->bShowMouseCursor = false;
		PlayerController->bEnableClickEvents = false;
		PlayerController->bEnableMouseOverEvents = false;
  // Sets the focus back to the screen after the mouse was freed
  PlayerController->SetInputMode(FInputModeGameOnly());
	} 

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
}

// Called to bind functionality to input
void AGameController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (MovementComponent != nullptr) MovementComponent->SetupKeyBindings(PlayerInputComponent);
	if (PickupComponent != nullptr) PickupComponent->SetupKeyBindings(PlayerInputComponent);
}

void AGameController::SetPlayerMovable(bool bIsMovable)
{
	if (MovementComponent != nullptr)
	{
		bIsMovementLocked = !bIsMovable;
		MovementComponent->SetMovable(bIsMovable);
	}
}