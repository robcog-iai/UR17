// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#define PLUGIN_TAG "UGame"
#define TAG_KEY_PICKUP "Pickup"
#define STACKCHECK_RANGE 500.0f
#define PICKUP_SHADOW_SCALE_FACTOR 0.4f // The scale factor when testing collisions on pickup

#include "GPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "../Private/Character/GameController.h"
#include "../../HUD/GameHUD.h"
#include "TagStatics.h"
#include "Engine.h"

// Sets default values for this component's properties
UGPickup::UGPickup()
	:bRotationStarted(false)
	, bRotationMenuActivated(false)
	, bPickupnMenuActivated(false)
	, ItemInRotaitonPosition(nullptr)
	, bButtonReleased(false)
	, bPickUpStarted(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaximumMassToCarry = 10.0f;
	bMassEffectsMovementSpeed = true;
	bUseQuadratricEquationForSpeedCalculation = true;

	bTwoHandMode = true;
	bStackModeEnabled = true;
	bNeedBothHands = true;
	bBothHandsDependOnMass = true;
	MassThresholdBothHands = 1.5f;

	bBothHandsDependOnVolume = false;
	VolumeThresholdBothHands = 3000.0f;

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

	if (PlayerCharacter == nullptr) UE_LOG(LogTemp, Fatal, TEXT("UCPickup::BeginPlay: The PlayerCharacter was not assigned. Restarting the editor might fix this."));

	RaycastRange = PlayerCharacter->GraspRange;

	UInputComponent* PlayerInputComponent = PlayerCharacter->InputComponent;

	PlayerInputComponent->BindAction("CancelAction", IE_Pressed, this, &UGPickup::CancelActions);
	PlayerInputComponent->BindAction("CancelAction", IE_Released, this, &UGPickup::StopCancelActions);

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

	UGameMode = (AUGameModeBase*)GetWorld()->GetAuthGameMode();
}


// Called every frame
void UGPickup::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool bLockedByOtherComponent = PlayerCharacter->LockedByComponent != nullptr &&  PlayerCharacter->LockedByComponent != this;

	// Handle the menu for the pickup and the rotation (Milestone 2)
	if (bLockedByOtherComponent == false && bAllCanceled == false) {
		ItemToHandle = PlayerCharacter->FocussedActor;
		/**
		// Pick up is initiated and first it will be asked for rotation
		if (bRightMouseHold && !bRotationMenuActivated && !bPickupnMenuActivated) {
			bRotationMenuActivated = true;
			UGameMode->DrawHudMenu();
			
			bPickUpStarted = true;
		}
		// Player choose to rotate the object first
		else if (bLeftMouseHold && bRotationMenuActivated)
		{
			bRotationStarted = true;
			UGameMode->RemoveMenu();
			bRotationMenuActivated = false;
			MoveToRotationPosition();
		}
		// Instead of rotation the player wants to directly pick the object up
		else if (bRightMouseHold && bRotationMenuActivated && bButtonReleased)
		{
			bRotationStarted = false;
			bRotationMenuActivated = false;
			UGameMode->RemoveMenu();
			bPickupnMenuActivated = true;
			UGameMode->DrawPickupHudMenu();
			bButtonReleased = false;
		}

		// Pickup with either left or right hand
		if (bLeftMouseHold && bPickupnMenuActivated && !bRotationMenuActivated && bPickUpStarted)
		{
			bRotationStarted = false;
			UGameMode->RemoveMenu();
			PickUpItemAfterMenu(true);
			bPickUpStarted = false;
		}
		else if (bRightMouseHold && bPickupnMenuActivated && !bRotationMenuActivated && bPickUpStarted && bButtonReleased)
		{
			bRotationStarted = false;
			UGameMode->RemoveMenu();
			PickUpItemAfterMenu(false);
			bPickUpStarted = false;
			bButtonReleased = false;
		}
		*/
	}
}

TArray<AStaticMeshActor*> UGPickup::FindAllStackableItems(AStaticMeshActor* ActorToPickup)
{
	TArray<AStaticMeshActor*>  StackedItems;
	int countNewItemsFound = 0;
	do
	{
		countNewItemsFound = 0;
		TArray<AActor*> IgnoredActors;

		for (auto &elem : StackedItems) {
			IgnoredActors.Add(elem);
		}

		FVector SecondPointOfSweep = ActorToPickup->GetActorLocation() + FVector::UpVector * STACKCHECK_RANGE;
		TArray<FHitResult> SweptActors;
		FComponentQueryParams Params;
		Params.AddIgnoredComponent_LikelyDuplicatedRoot(ActorToPickup->GetStaticMeshComponent());
		Params.AddIgnoredActors(IgnoredActors);
		Params.bTraceComplex = true;
		Params.bFindInitialOverlaps = true;

		ActorToPickup->GetWorld()->ComponentSweepMulti(
			SweptActors,
			ActorToPickup->GetStaticMeshComponent(),
			ActorToPickup->GetActorLocation(),
			SecondPointOfSweep,
			ActorToPickup->GetActorRotation(),
			Params);

		for (auto& elem : SweptActors) {
			if (elem.GetActor() != nullptr) {
				if (SetOfPickupItems.Contains(elem.GetActor())) {
					AStaticMeshActor* CastedActor = Cast<AStaticMeshActor>(elem.GetActor());

					if (StackedItems.Contains(CastedActor)) continue;

					UE_LOG(LogTemp, Warning, TEXT("Found item while sweeping %s"), *elem.GetActor()->GetName());
					FVector elemLocation = elem.GetActor()->GetActorLocation();
					FVector ActorToPickupLocation = ActorToPickup->GetActorLocation();

					if (elemLocation.Z < ActorToPickupLocation.Z) {
						UE_LOG(LogTemp, Warning, TEXT("Ignoring lower item %s"), *elem.GetActor()->GetName());
						continue; // ignore if the found item is lower than base item 
					}

					FVector DeltaPosition = elemLocation - ActorToPickupLocation;

					if (CastedActor != nullptr) {
						StackedItems.Add(CastedActor);
						countNewItemsFound++;
					}
				}
				else if (elem.GetActor()->GetActorLocation().Z >= ActorToPickup->GetActorLocation().Z) {
					break; // Break at non-pickupable item which is above our base item (It's probably a shelf or something)
				}
			}
		}
	} while (countNewItemsFound > 0);

	return StackedItems;
}

AStaticMeshActor * UGPickup::GetItemStack(AStaticMeshActor * BaseItem)
{
	TArray<AStaticMeshActor*>  StackOfItems = FindAllStackableItems(BaseItem);

	StackOfItems.Remove(BaseItem);

	for (auto& ChildItem : StackOfItems) {
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepWorldTransform;
		TransformRules.bWeldSimulatedBodies = true;

		ChildItem->AttachToActor(BaseItem, TransformRules);
	}

	return BaseItem;
}

AStaticMeshActor * UGPickup::GetNewShadowItem(AStaticMeshActor * FromActor)
{
	FActorSpawnParameters Parameters;
	AStaticMeshActor* NewShadowPickupItem = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Parameters);

	NewShadowPickupItem->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	NewShadowPickupItem->GetStaticMeshComponent()->SetSimulatePhysics(false);
	NewShadowPickupItem->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	NewShadowPickupItem->GetStaticMeshComponent()->SetStaticMesh(FromActor->GetStaticMeshComponent()->GetStaticMesh());
	NewShadowPickupItem->SetActorLocationAndRotation(FromActor->GetActorLocation(), FromActor->GetActorRotation());

	for (size_t i = 0; i < NewShadowPickupItem->GetStaticMeshComponent()->GetMaterials().Num(); i++)
	{
		NewShadowPickupItem->GetStaticMeshComponent()->SetMaterial(i, TransparentMaterial);
	}

	return NewShadowPickupItem;
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

void UGPickup::DisableShadowItems()
{
	for (auto& ShdwItm : ShadowItems) {
		ShdwItm->Destroy();
	}

	ShadowItems.Empty();

	if (ShadowBaseItem != nullptr)
	{
		ShadowBaseItem->Destroy();
		ShadowBaseItem = nullptr;
	}
}

FVector UGPickup::GetPositionOnSurface(AActor * Item, FVector PointOnSurface)
{
	FVector ItemOrigin;
	FVector ItemBoundExtend;
	Item->GetActorBounds(false, ItemOrigin, ItemBoundExtend);

	FVector DeltaOfPivotToCenter = ItemOrigin - Item->GetActorLocation();

	return FVector(PointOnSurface.X, PointOnSurface.Y, PointOnSurface.Z + ItemBoundExtend.Z) - DeltaOfPivotToCenter;
}

FHitResult UGPickup::RaytraceWithIgnoredActors(TArray<AActor*> IgnoredActors, FVector StartOffset, FVector TargetOffset)
{
	FHitResult RaycastResult;

	FVector CamLoc;
	FRotator CamRot;

	PlayerCharacter->Controller->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation
	const FVector StartTrace = CamLoc + StartOffset; // trace start is the camera location
	const FVector Direction = CamRot.Vector();
	const FVector EndTrace = StartTrace + Direction * RaycastRange + TargetOffset; // and trace end is the camera location + an offset in the direction

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActors(IgnoredActors);


	TArray<AActor*> ShadowActors;
	for (auto& shadow : ShadowItems) {
		ShadowActors.Add(shadow);
	}
	TraceParams.AddIgnoredActors(ShadowActors); // Always ignore shadow item

	if (ShadowBaseItem != nullptr) TraceParams.AddIgnoredActor(ShadowBaseItem); // Always ignore shadow item
	TraceParams.AddIgnoredActor(GetOwner()); // Always ignore player

	GetWorld()->LineTraceSingleByChannel(RaycastResult, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, TraceParams);

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
	if (bRightMouse) return; // Ignore this call if the other key has been pressed
	if (bIsStackChecking) return; // Don't react if stackchecking is active

	bLeftMouse = true;
	UsedHand = EHand::Left;
}

void UGPickup::InputLeftHandReleased()
{
	if (bLeftMouse == false) return; // We can't release if we didn't clicked first
	if (bRightMouse) return; // Ignore this call if the other key has been pressed
	if (bIsStackChecking) return; // Don't react if stackchecking is active

	bLeftMouse = false;
}

void UGPickup::InputRightHandPressed()
{
	//if (bRightMouse) return; // We already clicked that key
	if (bLeftMouse) return; // Ignore this call if the other key has been pressed
	if (bIsStackChecking) return; // Don't react if stackchecking is active

	bRightMouse = !bRightMouse;
	UsedHand = EHand::Right;
}

void UGPickup::InputRightHandReleased()
{
	if (bRightMouse == false) return;  // We can't release if we didn't clicked first
	if (bLeftMouse) return; // Ignore this call if the other key has been pressed
	if (bIsStackChecking) return; // Don't react if stackchecking is active

	UGameMode->DrawHudMenu();

	bButtonReleased = true;
}

void UGPickup::ResetComponentState()
{
	DisableShadowItems();
	SetLockedByComponent(false);
	PlayerCharacter->bRaytraceEnabled = true;
	CancelDetachItems();
}

void UGPickup::MoveToRotationPosition()
{
	if (bStackModeEnabled == false) {
		BaseItemToPick = ItemToHandle;
	}
	else {
		BaseItemToPick = GetItemStack(ItemToHandle);
	}

	if (ItemToHandle == nullptr) return;
	if (BaseItemToPick == nullptr) return;

	DisableShadowItems();

	FVector HandPosition;

	HandPosition = BothHandActor->GetActorLocation();
	
	TArray<AActor*> ChildItemsOfBaseItem;
	BaseItemToPick->GetAttachedActors(ChildItemsOfBaseItem);

	AStaticMeshActor* ShadowRoot = GetNewShadowItem(BaseItemToPick);
	ShadowItems.Add(ShadowRoot);

	// Create new shadow items for each child
	for (auto& ItemToShadow : ChildItemsOfBaseItem) {
		AStaticMeshActor* CastActor = Cast<AStaticMeshActor>(ItemToShadow);

		if (CastActor == nullptr) return;

		AStaticMeshActor* ShadowItem = GetNewShadowItem(CastActor);
		ShadowItems.Add(ShadowItem);

		ShadowItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepWorldTransform;
		TransformRules.bWeldSimulatedBodies = true;

		ShadowItem->AttachToActor(ShadowRoot, TransformRules);
	}
	//  *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***

	ShadowRoot->SetActorLocation(HandPosition);

	if (bCheckForCollisionsOnPickup) {
		// Check for collisions in between
		FVector CamLoc;
		FRotator CamRot;
		PlayerCharacter->Controller->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation

		FHitResult RaycastHit = RaytraceWithIgnoredActors(ChildItemsOfBaseItem);

		if (RaycastHit.Location != FVector::ZeroVector) {
			ShadowRoot->SetActorScale3D(FVector(PICKUP_SHADOW_SCALE_FACTOR));
			FVector FromPosition = RaycastHit.Location;
			FVector ToPosition = CamLoc;

			// *** Ignored Actors
			TArray<AActor*> IgnoredActors = ChildItemsOfBaseItem; // ignore items to pickup 
			IgnoredActors.Add(BaseItemToPick);
			IgnoredActors.Append(ShadowItems); //All shadow items are ignored
			IgnoredActors.Add(ItemInLeftHand);
			IgnoredActors.Add(ItemInRightHand);

			// Add all children in hands to ignored actors
			if (ItemInLeftHand != nullptr) {
				TArray<AActor*> ChildrenInLeftHand;
				ItemInLeftHand->GetAttachedActors(ChildrenInLeftHand);

				IgnoredActors.Append(ChildrenInLeftHand);
			}
			if (ItemInRightHand != nullptr) {
				TArray<AActor*> ChildrenInRightHand;
				ItemInRightHand->GetAttachedActors(ChildrenInRightHand);

				IgnoredActors.Append(ChildrenInRightHand);
			}
			// *****************

			FHitResult SweepHit = CheckForCollision(FromPosition, ToPosition, ShadowRoot, IgnoredActors);

			ShadowRoot->SetActorScale3D(FVector(1.0f));

			if (SweepHit.GetActor() != nullptr) {
				ShadowRoot->SetActorLocation(SweepHit.Location);
				bItemCanBePickedUp = false;
			}
			else {
				ShadowRoot->SetActorLocation(HandPosition);
				bItemCanBePickedUp = true;
			}
		}
	}
	else {
		ShadowRoot->SetActorLocation(HandPosition);
		bItemCanBePickedUp = true;
	}

	ShadowBaseItem = ShadowRoot;

	if (BaseItemToPick == nullptr || bItemCanBePickedUp == false) {
		SetMovementSpeed(-MassOfLastItemPickedUp);
		return;
	}

	FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepWorldTransform;
	TransformRules.bWeldSimulatedBodies = true;

	BaseItemToPick->AttachToActor(BothHandActor, TransformRules);
	ItemInRotaitonPosition = BaseItemToPick;
		
	BaseItemToPick->SetActorRelativeLocation(FVector::ZeroVector, false, nullptr, ETeleportType::TeleportPhysics);
	BaseItemToPick->SetActorRelativeRotation(FRotator::ZeroRotator);

	BaseItemToPick->GetStaticMeshComponent()->SetSimulatePhysics(false);

	// Disable collisions
	if (bEnableCollisionOfItemsInHand == false) {
		BaseItemToPick->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		TArray<AActor*> Children;
		BaseItemToPick->GetAttachedActors(Children);
		for (auto& Child : Children) {
			AStaticMeshActor* CastActor = Cast<AStaticMeshActor>(Child);
			if (CastActor != nullptr) CastActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	BaseItemToPick = nullptr;

	PlayerCharacter->bRaytraceEnabled = true;

	ResetComponentState();
}

void UGPickup::PickUpItemAfterMenu(bool leftHand)
{
	if (BaseItemToPick == nullptr || bItemCanBePickedUp == false) {
		SetMovementSpeed(-MassOfLastItemPickedUp);
		if (ItemInRotaitonPosition != nullptr)
		{
			BaseItemToPick = ItemInRotaitonPosition;
		}
		else {
			if (bStackModeEnabled == false) {
				BaseItemToPick = ItemToHandle;
			}
			else {
				BaseItemToPick = GetItemStack(ItemToHandle);
			}
		}
	}	

	FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepWorldTransform;
	TransformRules.bWeldSimulatedBodies = true;

	if (!leftHand) {
		if (ItemInRightHand != nullptr) return; // We already carry something 
		BaseItemToPick->AttachToActor(RightHandActor, TransformRules);
		ItemInRightHand = BaseItemToPick;
	}
	else if (leftHand) {
		if (ItemInLeftHand != nullptr) return; // We already carry something 
		BaseItemToPick->AttachToActor(LeftHandActor, TransformRules);
		ItemInLeftHand = BaseItemToPick;
	}

	BaseItemToPick->SetActorRelativeLocation(FVector::ZeroVector, false, nullptr, ETeleportType::TeleportPhysics);
	//BaseItemToPick->SetActorRelativeRotation(FRotator::ZeroRotator);

	BaseItemToPick->GetStaticMeshComponent()->SetSimulatePhysics(false);

	// Disable collisions
	if (bEnableCollisionOfItemsInHand == false) {
		BaseItemToPick->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		TArray<AActor*> Children;
		BaseItemToPick->GetAttachedActors(Children);
		for (auto& Child : Children) {
			AStaticMeshActor* CastActor = Cast<AStaticMeshActor>(Child);
			if (CastActor != nullptr) CastActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	BaseItemToPick = nullptr;

	PlayerCharacter->bRaytraceEnabled = true;

	if (ItemToHandle == nullptr) return;
	if (BaseItemToPick == nullptr) return;

	DisableShadowItems();

	FVector HandPosition;

	if (UsedHand == EHand::Right) {
		HandPosition = RightHandActor->GetActorLocation();
	}
	else if (UsedHand == EHand::Left) {
		HandPosition = LeftHandActor->GetActorLocation();
	}
	else {
		HandPosition = BothHandActor->GetActorLocation();
	}

	TArray<AActor*> ChildItemsOfBaseItem;
	BaseItemToPick->GetAttachedActors(ChildItemsOfBaseItem);

	AStaticMeshActor* ShadowRoot = GetNewShadowItem(BaseItemToPick);
	ShadowItems.Add(ShadowRoot);

	// Create new shadow items for each child
	for (auto& ItemToShadow : ChildItemsOfBaseItem) {
		AStaticMeshActor* CastActor = Cast<AStaticMeshActor>(ItemToShadow);

		if (CastActor == nullptr) return;

		AStaticMeshActor* ShadowItem = GetNewShadowItem(CastActor);
		ShadowItems.Add(ShadowItem);

		ShadowItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepWorldTransform;
		TransformRules.bWeldSimulatedBodies = true;

		ShadowItem->AttachToActor(ShadowRoot, TransformRules);
	}
	//  *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***

	ShadowRoot->SetActorLocation(HandPosition);

	if (bCheckForCollisionsOnPickup) {
		// Check for collisions in between
		FVector CamLoc;
		FRotator CamRot;
		PlayerCharacter->Controller->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation

		FHitResult RaycastHit = RaytraceWithIgnoredActors(ChildItemsOfBaseItem);

		if (RaycastHit.Location != FVector::ZeroVector) {
			ShadowRoot->SetActorScale3D(FVector(PICKUP_SHADOW_SCALE_FACTOR));
			FVector FromPosition = RaycastHit.Location;
			FVector ToPosition = CamLoc;

			// *** Ignored Actors
			TArray<AActor*> IgnoredActors = ChildItemsOfBaseItem; // ignore items to pickup 
			IgnoredActors.Add(BaseItemToPick);
			IgnoredActors.Append(ShadowItems); //All shadow items are ignored
			IgnoredActors.Add(ItemInLeftHand);
			IgnoredActors.Add(ItemInRightHand);

			// Add all children in hands to ignored actors
			if (ItemInLeftHand != nullptr) {
				TArray<AActor*> ChildrenInLeftHand;
				ItemInLeftHand->GetAttachedActors(ChildrenInLeftHand);

				IgnoredActors.Append(ChildrenInLeftHand);
			}
			if (ItemInRightHand != nullptr) {
				TArray<AActor*> ChildrenInRightHand;
				ItemInRightHand->GetAttachedActors(ChildrenInRightHand);

				IgnoredActors.Append(ChildrenInRightHand);
			}
			// *****************

			FHitResult SweepHit = CheckForCollision(FromPosition, ToPosition, ShadowRoot, IgnoredActors);

			ShadowRoot->SetActorScale3D(FVector(1.0f));

			if (SweepHit.GetActor() != nullptr) {
				ShadowRoot->SetActorLocation(SweepHit.Location);
				bItemCanBePickedUp = false;
			}
			else {
				ShadowRoot->SetActorLocation(HandPosition);
				bItemCanBePickedUp = true;
			}
		}
	}
	else {
		ShadowRoot->SetActorLocation(HandPosition);
		bItemCanBePickedUp = true;
	}

	ShadowBaseItem = ShadowRoot;
}

void UGPickup::StartDropItem()
{
	bIsItemDropping = true;
	SetLockedByComponent(true);
}

void UGPickup::DropItem()
{
	bIsItemDropping = false;
	if (ShadowBaseItem == nullptr) return;

	AStaticMeshActor* ItemToDrop;

	if (UsedHand == EHand::Right) {
		ItemToDrop = ItemInRightHand;
	}
	else {
		ItemToDrop = ItemInLeftHand; // Left hand or both hand item
	}

	if (ItemToDrop == nullptr) return;

	TArray<AActor*> ChildItemsOfBaseItem;
	ItemToDrop->GetAttachedActors(ChildItemsOfBaseItem);

	ItemToDrop->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemToDrop->GetStaticMeshComponent()->SetSimulatePhysics(true);
	ItemToDrop->SetActorLocation(ShadowBaseItem->GetActorLocation());
	ItemToDrop->SetActorRotation(ShadowBaseItem->GetActorRotation());
	ItemToDrop->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	float Mass = 0;

	for (auto& DroppedItem : ChildItemsOfBaseItem) {
		AStaticMeshActor* CastActor = Cast<AStaticMeshActor>(DroppedItem);
		DroppedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		if (CastActor != nullptr) {
			CastActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			CastActor->GetStaticMeshComponent()->SetSimulatePhysics(true);

			Mass += CastActor->GetStaticMeshComponent()->GetMass();
		}
	}
	Mass += ItemToDrop->GetStaticMeshComponent()->GetMass();

	if (ItemInRightHand == ItemInLeftHand) {
		ItemInRightHand = ItemInLeftHand = nullptr;
	}
	else if (UsedHand == EHand::Right) {
		ItemInRightHand = nullptr;
	}
	else if (UsedHand == EHand::Left) {
		ItemInLeftHand = nullptr;
	}

	if (bMassEffectsMovementSpeed) {
		SetMovementSpeed(-Mass);
	}
}

void UGPickup::ShadowDropItem()
{
	DisableShadowItems();

	AStaticMeshActor* ItemToDrop;

	if (UsedHand == EHand::Right) {
		ItemToDrop = ItemInRightHand;
	}
	else {
		ItemToDrop = ItemInLeftHand; // Left hand or both hand item
	}

	if (ItemToDrop == nullptr) return;

	TArray<AActor*> ChildItemsOfBaseItem;
	ItemToDrop->GetAttachedActors(ChildItemsOfBaseItem);

	AStaticMeshActor* ShadowRoot = GetNewShadowItem(ItemToDrop);
	ItemToDrop->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ShadowItems.Add(ShadowRoot);

	TArray<AActor*> IgnoredActors; // Actors that get ignored by following raytrace
	IgnoredActors.Add(ItemToDrop);

	for (auto& ItemToShadow : ChildItemsOfBaseItem) {
		AStaticMeshActor* CastActor = Cast<AStaticMeshActor>(ItemToShadow);

		if (CastActor == nullptr) return;
		CastActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		AStaticMeshActor* ShadowItem = GetNewShadowItem(CastActor);
		ShadowItems.Add(ShadowItem);

		ShadowItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepWorldTransform;
		TransformRules.bWeldSimulatedBodies = true;

		ShadowItem->AttachToActor(ShadowRoot, TransformRules);
		IgnoredActors.Add(CastActor);
	}

	FVector CamLoc;
	FRotator CamRot;
	PlayerCharacter->Controller->GetPlayerViewPoint(CamLoc, CamRot); // Get the camera position and rotation

	FHitResult RaycastHit = RaytraceWithIgnoredActors(IgnoredActors);

	if (RaycastHit.Actor != nullptr) {

		if (bCheckForCollisionsOnDrop) {
			ShadowRoot->SetActorRotation(ShadowRoot->GetActorRotation());

			// Checking for collisions between the player and the position the player wants to drop the item
			FVector FromPosition = CamLoc;
			FVector ToPosition = RaycastHit.Location;

			// *** Ignored Actors
			TArray<AActor*> IgnoredActors = ChildItemsOfBaseItem; // ignore items to pickup 
			IgnoredActors.Add(BaseItemToPick);
			IgnoredActors.Append(ShadowItems); //All shadow items are ignored
			IgnoredActors.Add(ItemInLeftHand);
			IgnoredActors.Add(ItemInRightHand);

			// Add all children in hands to ignored actors
			if (ItemInLeftHand != nullptr) {
				TArray<AActor*> ChildrenInLeftHand;
				ItemInLeftHand->GetAttachedActors(ChildrenInLeftHand);

				IgnoredActors.Append(ChildrenInLeftHand);
			}
			if (ItemInRightHand != nullptr) {
				TArray<AActor*> ChildrenInRightHand;
				ItemInRightHand->GetAttachedActors(ChildrenInRightHand);

				IgnoredActors.Append(ChildrenInRightHand);
			}
			// *****************

			FHitResult SweepHit = CheckForCollision(FromPosition, ToPosition, ShadowRoot, IgnoredActors);

			if (SweepHit.GetActor() != nullptr) {
				ShadowRoot->SetActorLocation(SweepHit.Location);
				ShadowBaseItem = ShadowRoot;
			}
		}
		else {
			ShadowRoot->SetActorLocation(GetPositionOnSurface(ItemToDrop, RaycastHit.Location));
			ShadowBaseItem = ShadowRoot;
		}
	}
	else {
		ShadowBaseItem = nullptr; // We didn't hit any surface
	}
}

void UGPickup::CancelActions()
{
	bAllCanceled = true;
	ResetComponentState();


	if (bIsItemDropping == false) {
		// Pick up event canceled
		if (BaseItemToPick != nullptr) {
			SetMovementSpeed(-MassOfLastItemPickedUp);
		}
	}

	BaseItemToPick = nullptr;

	bRightMouse = false;
	bLeftMouse = false;
}

void UGPickup::StopCancelActions()
{
	bAllCanceled = false;
}

void UGPickup::CancelDetachItems()
{
	if (BaseItemToPick != nullptr) {
		TArray<AActor*> Children;
		BaseItemToPick->GetAttachedActors(Children);
		for (auto& item : Children) {
			AStaticMeshActor* CastActor = Cast<AStaticMeshActor>(item);

			CastActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			CastActor->GetStaticMeshComponent()->SetSimulatePhysics(true);
		}
	}
}

void UGPickup::SetLockedByComponent(bool bIsLocked)
{
	if (bIsLocked) {
		PlayerCharacter->LockedByComponent = this;
	}
	else {
		PlayerCharacter->LockedByComponent = nullptr;
	}
}

bool UGPickup::CalculateIfBothHandsNeeded()
{
	BaseItemToPick->GetStaticMeshComponent()->SetSimulatePhysics(true);
	if (bBothHandsDependOnMass) {
		float MassOfItem = BaseItemToPick->GetStaticMeshComponent()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("Mass of object %f"), MassOfItem);

		if (MassOfItem >= MassThresholdBothHands) {
			BaseItemToPick->GetStaticMeshComponent()->SetSimulatePhysics(false);
			return true;
		}
	}

	if (bBothHandsDependOnVolume) {
		float Volume = BaseItemToPick->GetStaticMeshComponent()->Bounds.GetBox().GetSize().SizeSquared();
		UE_LOG(LogTemp, Warning, TEXT("Volume of object %f"), Volume);

		if (Volume >= VolumeThresholdBothHands) {
			BaseItemToPick->GetStaticMeshComponent()->SetSimulatePhysics(false);
			return true;
		}
	}
	BaseItemToPick->GetStaticMeshComponent()->SetSimulatePhysics(false);
	return false;
}

void UGPickup::SetMovementSpeed(float Weight)
{
	if (PlayerCharacter->MovementComponent == nullptr) return;
	MassToCarry += Weight;

	float MinSpeed = PlayerCharacter->MovementComponent->MinMovementSpeed;
	float MaxSpeed = PlayerCharacter->MovementComponent->MaxMovementSpeed;

	float NewSpeed = 0;

	if (Weight <= 0 /*We are dropping*/  && ItemInLeftHand == nullptr && ItemInRightHand == nullptr) {	// Check if we still carry something
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

