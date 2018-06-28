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
	GraspRange = 300.0f;
	bRaytraceEnabled = true;

	// Initiate empty rotator
	ControlRotation = FRotator::ZeroRotator;

	// Set PlayerController (by Waldemar Zeitler)
	PlayerController = nullptr;

	// Setup for the components
	MovementComponent = CreateDefaultSubobject<UGMovement>(TEXT("Movement Component"));

	PickupComponent = CreateDefaultSubobject<UGPickup>(TEXT("Pickup Component"));
	PickupComponent->PlayerCharacter = this;

	OpenCloseComponent = CreateDefaultSubobject<UGOpenClose>(TEXT("OpenClose Component"));
	OpenCloseComponent->PlayerCharacter = this;

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

	SetOfInteractableItems = FTagStatics::GetActorSetWithKeyValuePair(GetWorld(), "UGame", TAG_KEY_INTERACTABLE, "True");

	// *** *** *** *** *** ***
	//LeftHandPosition = SpawnActor<AActor>(FVector(0, 0, 0), FRotator(0, 0, 0));

 // Setup HUD
	PickupHUD = Cast<AGameHUD>(PlayerController->GetHUD());

	SetupScenario();

	PickupHUD->GPickup = PickupComponent;
}

// Called every frame
void AGameController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PickupComponent == nullptr) UE_LOG(LogTemp, Warning, TEXT("NULL"));

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
}

// Called to bind functionality to input
void AGameController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (MovementComponent != nullptr) MovementComponent->SetupKeyBindings(PlayerInputComponent);
	if (PickupComponent != nullptr) PickupComponent->SetupKeyBindings(PlayerInputComponent);
	if (OpenCloseComponent != nullptr) OpenCloseComponent->SetupKeyBindings(PlayerInputComponent);
}

void AGameController::SetPlayerMovable(bool bIsMovable)
{
	if (MovementComponent != nullptr)
	{
		bIsMovementLocked = !bIsMovable;
		MovementComponent->SetMovable(bIsMovable);
	}
}

void AGameController::SetupScenario()
{
	switch (InteractionMode) {
	case EInteractionMode::OneHandMode:
		if (PickupComponent != nullptr) {
			PickupComponent->bTwoHandMode = false;
		}
		break;
	case EInteractionMode::TwoHandMode:
		if (PickupComponent != nullptr) {
			PickupComponent->bTwoHandMode = true;
		}
		break;
	case EInteractionMode::TwoHandStackingMode:
		if (PickupComponent != nullptr) {
			PickupComponent->bTwoHandMode = true;
		}
		break;
	}
}

bool AGameController::CheckForVisibleObjects()
{
	float MinRecentTime = 0.01;

	//Iterate Over Actors
	for (TObjectIterator<AActor> Itr; Itr; ++Itr)
	{
		TArray<FName> ObjectTags = Itr->Tags;
		if (ObjectTags.Num() > 0 && ObjectTags[0].ToString().Contains("Pickup")) {
			float Time = Itr->GetLastRenderTime();
			if (Time > 0, 1) return true;
		}
	}

	return false;
}