// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: David Brinkmann

#define TAG_KEY_INTERACTABLE "Interactable"
#define CAMERA_TAG "Runtime,Dynamic;Class,CharacterCamera;Id,Abci;"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 100000.0f,FColor::White,text, false);
#define printAndStop(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 100000.0f,FColor::Red,text, false); GetWorld()->GetFirstPlayerController()->SetPause(true);
#define logText(text) UE_LOG(LogTemp, Warning, TEXT(text));
#define logAndStop(text) UE_LOG(LogTemp, Warning, TEXT(text)); GetWorld()->GetFirstPlayerController()->SetPause(true);

#include "CharacterController.h"
#include "DrawDebugHelpers.h"
#include "TagStatics.h"
#include "Engine.h" // Needed for GEngine

// Sets default values
ACharacterController::ACharacterController()
	:PlayerController(nullptr) // Set PlayerController (by Waldemar Zeitler)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GraspRange = 120.0f;
	bRaytraceEnabled = true;

	GetCapsuleComponent()->SetCapsuleRadius(0.01f);

	SetupComponentsOnConstructor();
}

// Called when the game starts or when spawned
void ACharacterController::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ACharacter>(this);
	if (Character == nullptr)
	{
		printAndStop("ACharacterController::BeginPlay: Character is null. Game paused to prevent crash.");
		Character->PlayerState->bIsSpectator = true;
	}

	SetOfInteractableItems = FTagStatics::GetActorSetWithKeyValuePair(GetWorld(), "ClickInteraction", TAG_KEY_INTERACTABLE, "True");

	// Initilize the player controller to get the mouse axis (by Wlademar Zeitler)
	PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) {
		UE_LOG(LogTemp, Warning, TEXT("Player controller was not set."));
	}
	// *** *** *** *** *** ***
	
	SetupScenario();
}

// Called every frame
void ACharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	StartRaytrace();
	CheckIntractability();

	if (bIsDebugMode && FocussedActor != nullptr)
	{
		DrawDebugLine(GetWorld(), RaycastResult.ImpactPoint, RaycastResult.ImpactPoint + 20 * RaycastResult.ImpactNormal, FColor::Blue, false, 0, 0, .5f);
	}

	if (PickupComponent == nullptr) UE_LOG(LogTemp, Warning, TEXT("NULL"));

	// Rotate the object depending of the rotation mode
	if (PickupComponent->RotationValue != 0) {
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
		Result.X = -(YMouse - CharInScreen.Y);
		Result.Y = XMouse - CharInScreen.X;

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
		if (PickupComponent->RotationValue == 1) {
			PickupComponent->ItemInLeftHand->SetActorRotation(rot);
		}
		else {
			PickupComponent->ItemInRightHand->SetActorRotation(rot);
		}
	}
}

// Called to bind functionality to input
void ACharacterController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (MovementComponent != nullptr) MovementComponent->SetupKeyBindings(PlayerInputComponent);
	if (PickupComponent != nullptr) PickupComponent->SetupKeyBindings(PlayerInputComponent);
	if (OpenCloseComponent != nullptr) OpenCloseComponent->SetupKeyBindings(PlayerInputComponent);
}

void ACharacterController::SetupComponentsOnConstructor()
{
	if (MovementComponent == nullptr) {
		MovementComponent = CreateDefaultSubobject<UCMovement>(TEXT("Movement Component"));
		MovementComponent->bEditableWhenInherited = true;
		AddInstanceComponent(MovementComponent);
		MovementComponent->RegisterComponent();
	}

	if (OpenCloseComponent == nullptr) {
		OpenCloseComponent = CreateDefaultSubobject<UCOpenClose>(TEXT("OpenClose Component"));
		OpenCloseComponent->bEditableWhenInherited = true;
		AddInstanceComponent(OpenCloseComponent);
		OpenCloseComponent->RegisterComponent();

		OpenCloseComponent->PlayerCharacter = this;
	}

	if (PickupComponent == nullptr) {
		PickupComponent = CreateDefaultSubobject<UCPickup>(TEXT("Pickup Component"));
		PickupComponent->bEditableWhenInherited = true;
		AddInstanceComponent(PickupComponent);
		PickupComponent->RegisterComponent();

		PickupComponent->PlayerCharacter = this;
	}
}

void ACharacterController::StartRaytrace()
{
	if (bRaytraceEnabled == false) return;
	FVector CamLoc;
	FRotator CamRot;

	Character->Controller->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation
	const FVector StartTrace = CamLoc; // trace start is the camera location
	const FVector Direction = CamRot.Vector();
	const FVector EndTrace = StartTrace + Direction * GraspRange; // and trace end is the camera location + an offset in the direction

	FCollisionQueryParams TraceParams;

	TraceParams.AddIgnoredActor(this); // We don't want to hit ourself

	TArray<AActor*> IgnoredActors;
	if (PickupComponent != nullptr) {
		if (PickupComponent->ItemInRightHand != nullptr) {
			IgnoredActors.Add(PickupComponent->ItemInRightHand);

			TArray<AActor*> ChildrenOfItem;
			PickupComponent->ItemInRightHand->GetAttachedActors(ChildrenOfItem);
			IgnoredActors.Append(ChildrenOfItem);
		}

		if (PickupComponent->ItemInLeftHand != nullptr) {
			IgnoredActors.Add(PickupComponent->ItemInLeftHand);

			TArray<AActor*> ChildrenOfItem;
			PickupComponent->ItemInLeftHand->GetAttachedActors(ChildrenOfItem);
			IgnoredActors.Append(ChildrenOfItem);
		}
	}

	TraceParams.AddIgnoredActors(IgnoredActors);

	GetWorld()->LineTraceSingleByChannel(RaycastResult, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, TraceParams);
}

void ACharacterController::CheckIntractability()
{
	AActor* Actor = RaycastResult.GetActor();

	if (Actor != nullptr)
	{
		if (SetOfInteractableItems.Contains(Actor))
		{
			// Deactivate outline of previous object
			if (FocussedActor != nullptr && FocussedActor != Actor)
			{
				FocussedActor->GetStaticMeshComponent()->SetRenderCustomDepth(false);
			}

			FocussedActor = Cast<AStaticMeshActor>(Actor);
			FocussedActor->GetStaticMeshComponent()->SetRenderCustomDepth(true);
		}
		else if (FocussedActor != nullptr && bComponentsLocked == false)
		{
			FocussedActor->GetStaticMeshComponent()->SetRenderCustomDepth(false);
			FocussedActor = nullptr;
		}
	}
	else
	{
		if (FocussedActor != nullptr)
		{
			FocussedActor->GetStaticMeshComponent()->SetRenderCustomDepth(false);
			FocussedActor = nullptr;
		}
	}
}

void ACharacterController::SetPlayerMovable(bool bIsMovable)
{
	if (MovementComponent != nullptr)
	{
		bIsMovementLocked = !bIsMovable;
		MovementComponent->SetMovable(bIsMovable);
	}
}

void ACharacterController::SetupScenario()
{
	switch (InteractionMode) {
	case EInteractionMode::OneHandMode:
		if (PickupComponent != nullptr) {
			PickupComponent->bTwoHandMode = false;
			PickupComponent->bStackModeEnabled = false;
		}
		break;
	case EInteractionMode::TwoHandMode:
		if (PickupComponent != nullptr) {
			PickupComponent->bTwoHandMode = true;
			PickupComponent->bStackModeEnabled = false;
		}
		break;
	case EInteractionMode::TwoHandStackingMode:
		if (PickupComponent != nullptr) {
			PickupComponent->bTwoHandMode = true;
			PickupComponent->bStackModeEnabled = true;
		}
		break;
	}
}

FHitResult ACharacterController::GetRaycastResult()
{
	return RaycastResult;
}





