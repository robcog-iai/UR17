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

	// Set PlayerController (by Waldemar Zeitler)
	PlayerController = nullptr;

	// Setup for the components
	MovementComponent = nullptr;
	PickupComponent = nullptr;
	OpenCloseComponent = nullptr;

	GetCapsuleComponent()->SetCapsuleRadius(0.01f);

	SetupComponentsOnConstructor();

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

	// Stop movment when menu is active by Wlademar Zeitler
	if (PickupComponent->bFreeMouse && !bIsMovementLocked && CheckForVisibleObjects())
	{
		SetPlayerMovable(false);
		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;

	}
 if (PickupComponent->bFreeMouse && !PickupComponent->bRightMouse && PickupComponent->bPickupnMenuActivated && PickupComponent->bOverItem)
 {
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

	// Rotate the object depending of the rotation mode by Waldemar Zeitler
	if (PickupComponent->bRotationStarted) {
		// Get mouse position on screen
		float XMouse;
		float YMouse;
		PlayerController->GetMousePosition(XMouse, YMouse);
		
		// Get Character position on screen
		FVector CharLoc = Character->GetActorLocation();
		FVector2D CharInScreen;
		PlayerController->ProjectWorldLocationToScreen(CharLoc, CharInScreen);

		// Get mouse position relative to the Character.
		FVector2D Result;
		Result.X = XMouse - CharInScreen.X;
		Result.Y = -(YMouse - CharInScreen.Y);

		// Get angle rotation and rotation Character
		float angleX = FMath::RadiansToDegrees(FMath::Acos(Result.X / Result.Size()));
		float angleY = FMath::RadiansToDegrees(FMath::Acos(Result.Y / Result.Size()));

		if (Result.Y < 0) {
			angleX = 360 - angleX;
		}

		if (Result.X < 0) {
			angleY = 360 - angleX;
		}
		FRotator rot(angleY, angleX, 0);
		PickupComponent->ItemInRotaitonPosition->SetActorRotation(rot);
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

void AGameController::SetupComponentsOnConstructor()
{
	MovementComponent = CreateDefaultSubobject<UGMovement>(TEXT("Movement Component"));
	MovementComponent->bEditableWhenInherited = true;
	AddInstanceComponent(MovementComponent);
	MovementComponent->RegisterComponent();

	OpenCloseComponent = CreateDefaultSubobject<UGOpenClose>(TEXT("OpenClose Component"));
	OpenCloseComponent->bEditableWhenInherited = true;
	AddInstanceComponent(OpenCloseComponent);
	OpenCloseComponent->RegisterComponent();
	OpenCloseComponent->PlayerCharacter = this;
	

	PickupComponent = CreateDefaultSubobject<UGPickup>(TEXT("Pickup Component"));
	PickupComponent->bEditableWhenInherited = true;
	AddInstanceComponent(PickupComponent);
	PickupComponent->RegisterComponent();

	PickupComponent->PlayerCharacter = this;
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
			if (Time > 0,1) return true;	
		}
	}

	return false;
}