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
	bUseQuadratricEquationForSpeedCalculation = true;

	bTwoHandMode = true;
	bInRotationPosition = false;

	bCheckForCollisionsOnPickup = false;
	bCheckForCollisionsOnDrop = true;
	// *** *** *** *** *** *** *** ***

	TransparentMaterial = nullptr;
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

	RaycastRange = PlayerCharacter->GraspRange;

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


	MaxMovementSpeed = PlayerCharacter->MovementComponent->MaxMovementSpeed;

	/////////////////////////////////////////////////////////

	// Initilize the player controller to get the mouse axis (by Wlademar Zeitler)
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
		if (bFreeMouse && !bRightMouse && !bPickupMenuActivated && bOverItem)
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

void UGPickup::UnstackItems(AStaticMeshActor * BaseItem)
{
	if (BaseItem == nullptr) return;

	TArray<AActor*> ChildItemsOfBaseItem;
	BaseItem->GetAttachedActors(ChildItemsOfBaseItem);

	BaseItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	BaseItem->GetStaticMeshComponent()->SetSimulatePhysics(true);
	BaseItem->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	for (auto& DroppedItem : ChildItemsOfBaseItem) {
		AStaticMeshActor* CastActor = Cast<AStaticMeshActor>(DroppedItem);
		DroppedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		if (CastActor != nullptr) {
			CastActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			CastActor->GetStaticMeshComponent()->SetSimulatePhysics(true);
		}
	}
}

FHitResult UGPickup::CheckForCollision(FVector From, FVector To, AStaticMeshActor * ItemToSweep, TArray<AActor*> IgnoredActors)
{
	TArray<FHitResult> Hits;
	FComponentQueryParams Params;

	Params.AddIgnoredActors(IgnoredActors);
	Params.AddIgnoredActor(GetOwner()); // Always ignore player

	GetWorld()->ComponentSweepMulti(Hits, ItemToSweep->GetStaticMeshComponent(), From, To, ItemToSweep->GetActorRotation(), Params);

	if (Hits.Num() > 0) {
		return Hits[0];
	}

	return FHitResult();
}

//TODO: Check if necessary for dropping an item
FVector UGPickup::GetPositionOnSurface(AActor * Item, FVector PointOnSurface)
{
	FVector ItemOrigin;
	FVector ItemBoundExtend;
	Item->GetActorBounds(false, ItemOrigin, ItemBoundExtend);

	FVector DeltaOfPivotToCenter = ItemOrigin - Item->GetActorLocation();

	return FVector(PointOnSurface.X, PointOnSurface.Y, PointOnSurface.Z + ItemBoundExtend.Z) - DeltaOfPivotToCenter;
}

//TODO: Check if necessary for dropping an item
FHitResult UGPickup::RaytraceWithIgnoredActors(TArray<AActor*> IgnoredActors, FVector MousePosition, FVector MouseDirection)
{
	FHitResult RaycastResult;

	const FVector EndTrace = MousePosition + MouseDirection * RaycastRange; // and trace end is the camera location + an offset in the direction

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActors(IgnoredActors);

	TraceParams.AddIgnoredActor(GetOwner()); // Always ignore player

	GetWorld()->LineTraceSingleByChannel(RaycastResult, MousePosition, EndTrace, ECollisionChannel::ECC_Visibility, TraceParams);

	return RaycastResult;
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
	if (bLeftMouse) return; // We already clicked that key

	bLeftMouse = true;
	if (bDropStarted)
	{
		bDropStarted = false;
		DropItem();
	}
}

void UGPickup::InputLeftHandReleased()
{
	if (bLeftMouse == false) return; // We can't release if we didn't clicked first
	if (bRightMouse) return; // Ignore this call if the other key has been pressed

	bLeftMouse = false;
}

void UGPickup::InputRightHandPressed()
{
	//if (bRightMouse) return; // We already clicked that key
	if (bLeftMouse) return; // Ignore this call if the other key has been pressed

	bRightMouse = !bRightMouse;

	if (bRightMouse) {
		bFreeMouse = true;
	}
	//UsedHand = EHand::Right;
}

void UGPickup::InputRightHandReleased()
{
	if (bRightMouse == false) return;  // We can't release if we didn't clicked first
	if (bLeftMouse) return; // Ignore this call if the other key has been pressed

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
		SetMovementSpeed(-MassOfLastItemPickedUp);
		if (ItemInRotaitonPosition != nullptr)
		{
			BaseItemToPick = ItemInRotaitonPosition;
			ItemInRotaitonPosition = nullptr;
		}
		else {
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

	ItemToHandle = nullptr;
	BaseItemToPick = nullptr;
}

void UGPickup::DropItem()
{
	FDetachmentTransformRules TransformRules = FDetachmentTransformRules::KeepWorldTransform;
	TransformRules.bCallModify = true;

	ItemToHandle->DetachFromActor(TransformRules);

}

void UGPickup::SetMovementSpeed(float Weight)
{
	if (PlayerCharacter->MovementComponent == nullptr) return;
	MassToCarry += Weight;

	float MinSpeed = PlayerCharacter->MovementComponent->MinMovementSpeed;
	float MaxSpeed = PlayerCharacter->MovementComponent->MaxMovementSpeed;

	float NewSpeed = 0;

	if (Weight <= 0 /*We are dropping*/ && ItemInLeftHand == nullptr && ItemInRightHand == nullptr) {	// Check if we still carry something
		PlayerCharacter->MovementComponent->CurrentSpeed = MaxMovementSpeed;
		UE_LOG(LogTemp, Warning, TEXT("New speed %f"), MaxMovementSpeed);
		MassOfLastItemPickedUp = 0;
	}
	else {
		if (bUseQuadratricEquationForSpeedCalculation) {
			NewSpeed = (MinSpeed - MaxSpeed) * MassToCarry * MassToCarry / (MaximumMassToCarry * MaximumMassToCarry) + MaxSpeed;
		}
		else {
			NewSpeed = (MinSpeed - MaxSpeed) * MassToCarry / MaximumMassToCarry + MaxSpeed;
		}

		UE_LOG(LogTemp, Warning, TEXT("New speed %f"), NewSpeed);

		PlayerCharacter->MovementComponent->CurrentSpeed = NewSpeed;
	}
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