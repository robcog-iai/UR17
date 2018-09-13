// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#define PLUGIN_TAG "UGame"
#define TAG_KEY_PICKUP "Pickup"
#define STACKCHECK_RANGE 500.0f

#include "GPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "../Private/Character/GameController.h"
#include "../../HUD/GameHUD.h"
#include "TagStatics.h"
#include "Engine.h"

// Sets default values for this component's properties
UGPickup::UGPickup()
	:bRotationStarted(false)
	, bRotationMenuActivated(false)
	, bDropStarted(false)
	, bPickupMenuActivated(false)
	, ItemInRotaitonPosition(nullptr)
	, bButtonReleased(false)
	, bPickUpStarted(false)
	, bFreeMouse(false)
	, bOverItem(false)
	, ItemToInteract(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaximumMassToCarry = 10.0f;
	bMassEffectsMovementSpeed = true;

	bTwoHandMode = true;
	bInRotationPosition = false;
	// *** *** *** *** *** *** *** ***
}

// Called when the game starts
void UGPickup::BeginPlay()
{
	Super::BeginPlay();

	SetOfPickupItems = FTagStatics::GetActorSetWithKeyValuePair(GetWorld(), PLUGIN_TAG, TAG_KEY_PICKUP, "True");

	// Go through the pickup items and give them mouse over events
	for (AActor* InteractableItem : SetOfPickupItems)
	{
		InteractableItem->OnBeginCursorOver.AddDynamic(this, &UGPickup::CustomOnBeginMouseOver);
		InteractableItem->OnEndCursorOver.AddDynamic(this, &UGPickup::CustomOnEndMouseOver);
	}

	if (PlayerCharacter == nullptr) UE_LOG(LogTemp, Fatal, TEXT("UCPickup::BeginPlay: The PlayerCharacter was not assigned. Restarting the editor might fix this."));

	UInputComponent* PlayerInputComponent = PlayerCharacter->InputComponent;

	// Create Static mesh actors for hands to weld items we pickup into this position
	if (PlayerCharacter->LeftHandPosition == nullptr || PlayerCharacter->RightHandPosition == nullptr || PlayerCharacter->BothHandPosition == nullptr) {
		GEngine->AddOnScreenDebugMessage(-1, 100000.0f, FColor::Red, "No hand actors found. Game paused", false);
		GetWorld()->GetFirstPlayerController()->SetPause(true);
	}

	// Spawn new actors and set them up
	LeftHandActor = GetWorld()->SpawnActor<AStaticMeshActor>();
	RightHandActor = GetWorld()->SpawnActor<AStaticMeshActor>();
	BothHandActor = GetWorld()->SpawnActor<AStaticMeshActor>();

	LeftHandActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	RightHandActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	BothHandActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);

	LeftHandActor->SetActorLocation(PlayerCharacter->LeftHandPosition->GetActorLocation());
	RightHandActor->SetActorLocation(PlayerCharacter->RightHandPosition->GetActorLocation());
	BothHandActor->SetActorLocation(PlayerCharacter->BothHandPosition->GetActorLocation());

	LeftHandActor->AttachToActor(PlayerCharacter, FAttachmentTransformRules::KeepWorldTransform);
	RightHandActor->AttachToActor(PlayerCharacter, FAttachmentTransformRules::KeepWorldTransform);
	BothHandActor->AttachToActor(PlayerCharacter, FAttachmentTransformRules::KeepWorldTransform);

	LeftHandActor->GetStaticMeshComponent()->SetSimulatePhysics(false);
	RightHandActor->GetStaticMeshComponent()->SetSimulatePhysics(false);
	BothHandActor->GetStaticMeshComponent()->SetSimulatePhysics(false);

	LeftHandActor->GetStaticMeshComponent()->SetCollisionProfileName("NoCollision");
	RightHandActor->GetStaticMeshComponent()->SetCollisionProfileName("NoCollision");
	BothHandActor->GetStaticMeshComponent()->SetCollisionProfileName("NoCollision");
	// *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***

	// Initilize the player controller to get the mouse axis
	PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	PlayerController->bEnableMouseOverEvents = true;
}

// Called every frame
void UGPickup::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool bLockedByOtherComponent = PlayerCharacter->LockedByComponent != nullptr &&  PlayerCharacter->LockedByComponent != this;

	// Handle the menu for the pickup and the rotation (Milestone 2)
	if (bLockedByOtherComponent == false)
	{
		// Mouse is free and right mouse button was pressed again
		if (bFreeMouse && !bRightMouse && !bPickupMenuActivated && bOverItem && !bDropStarted)
		{
			if (ItemToInteract != nullptr)
			{
				ItemToHandle = Cast<AStaticMeshActor>(ItemToInteract);
				bPickupMenuActivated = true;
			}
		}
		else
		{
			bPickupMenuActivated = false;
		}
	}
}

void UGPickup::SetupKeyBindings(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("LeftHandAction", IE_Pressed, this, &UGPickup::InputLeftHandPressed);
	PlayerInputComponent->BindAction("LeftHandAction", IE_Released, this, &UGPickup::InputLeftHandReleased);

	PlayerInputComponent->BindAction("RightHandAction", IE_Pressed, this, &UGPickup::InputRightHandPressed);
	PlayerInputComponent->BindAction("RightHandAction", IE_Released, this, &UGPickup::InputRightHandReleased);
}

void UGPickup::InputLeftHandPressed()
{
	bLeftMouse = true;

	if (bDropStarted)
	{
		bDropStarted = false;
		DropItem();
	}

 if (bRightMouse)
 {
     bRightMouse = false;
 }

 if (bFreeMouse)
 {
     bFreeMouse = false;
 }    
}

void UGPickup::InputLeftHandReleased()
{
	bLeftMouse = false;

 if (bFreeMouse)
 {
     bFreeMouse = false;
 }    
}

void UGPickup::InputRightHandPressed()
{
	bRightMouse = !bRightMouse;

	if (bRightMouse) 
 {
		bFreeMouse = true;
	} 
	else if (!bRightMouse && ItemToInteract == nullptr)
	{
		bFreeMouse = false;
	}
}

void UGPickup::InputRightHandReleased()
{
	bButtonReleased = true;
}

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

	FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepWorldTransform;
	TransformRules.bWeldSimulatedBodies = true;

	BaseItemToPick->AttachToActor(BothHandActor, TransformRules);
	ItemInRotaitonPosition = BaseItemToPick;

	BaseItemToPick->SetActorRelativeLocation(FVector::ZeroVector, false, nullptr, ETeleportType::TeleportPhysics);
	BaseItemToPick->GetStaticMeshComponent()->SetSimulatePhysics(false);

	bInRotationPosition = true;
	bRotationStarted = true;

	BaseItemToPick = nullptr;
	ItemToHandle = nullptr;
}

void UGPickup::PickUpItemAfterMenu(bool leftHand)
{
	bInRotationPosition = false;

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

	bPickupMenuActivated = false;
	bFreeMouse = false;

	ItemToHandle = nullptr;
	BaseItemToPick = nullptr;
}

void UGPickup::DropItem()
{
	ItemToHandle->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemToHandle->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemToHandle->GetStaticMeshComponent()->SetSimulatePhysics(true);

	if (ItemInRightHand == ItemToHandle)
		ItemInRightHand = nullptr;
	else
		ItemInLeftHand = nullptr;

	ItemToHandle = nullptr;
	bDropStarted = false;
	bFreeMouse = false;
}

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