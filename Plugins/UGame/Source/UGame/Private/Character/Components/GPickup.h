// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#pragma once

#include "Components/ActorComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine.h"
#include "../../UGameModeBase.h"
#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "GPickup.generated.h"

UENUM()
enum EHand
{
	Right UMETA(DisplayName = "Right"),
	Left UMETA(DisplayName = "Left"),
	Both UMETA(DisplayName = "Both")
};

class AGameController; // Use Forward Declaration. Including the header in CPickup.cpp

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UGAME_API UGPickup : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGPickup();

	AStaticMeshActor* ItemInLeftHand;
	AStaticMeshActor* ItemInRightHand;

	// The materials for the shadow items
	UPROPERTY(EditAnyWhere, Category = "CI - General")
		UMaterial* TransparentMaterial;

	// Whether or nor the player can use both hands
	UPROPERTY(EditAnyWhere, Category = "CI - Hand Setup")
		bool bTwoHandMode;

	// Whether or not the player can create stacks
	UPROPERTY(EditAnyWhere, Category = "CI - Hand Setup")
		bool bStackModeEnabled;

	// Wether or not both hands needed for certain items (heavy or large items)
	UPROPERTY(EditAnyWhere, Category = "CI - Hand Setup")
		bool bNeedBothHands;

	// Whether or not mass in taking into account if both hands are needed
	UPROPERTY(EditAnyWhere, Category = "CI - Hand Setup")
		bool bBothHandsDependOnMass;

	// Maximum mass of an object to pickup
	UPROPERTY(EditAnywhere, Category = "CI - Hand Setup")
		float MassThresholdBothHands;

	// Whether or not mass in taking into account if both hands are needed
	UPROPERTY(EditAnyWhere, Category = "CI - Hand Setup")
		bool bBothHandsDependOnVolume;

	// Maximum mass of an object to pickup
	UPROPERTY(EditAnywhere, Category = "CI - Hand Setup")
		float VolumeThresholdBothHands;

	// Whether or not the player's movement speed depends on the weight the player carries
	UPROPERTY(EditAnywhere, Category = "CI - Physics")
		bool bMassEffectsMovementSpeed;

	// Use quadratic formula to calculate speed. If false the speed calculation is linear. Using quadratic formula leads to higher speed if lighter items are carried
	UPROPERTY(EditAnywhere, Category = "CI - Physics")
		bool bUseQuadratricEquationForSpeedCalculation;

	// How much mass can the player carry
	UPROPERTY(EditAnywhere, Category = "CI - Physics")
		float MaximumMassToCarry;

	// Whether or not to check for collisions on pickup
	UPROPERTY(EditAnywhere, Category = "CI - Collision")
		bool bCheckForCollisionsOnPickup;

	// Whether or not to check for collisions on drop
	UPROPERTY(EditAnywhere, Category = "CI - Collision")
		bool bCheckForCollisionsOnDrop;

	// Whether or not to check for collisions while holding items in hands
	UPROPERTY(EditAnywhere, Category = "CI - Collision")
		bool bEnableCollisionOfItemsInHand;

	AGameController * PlayerCharacter; // The player character instance

	TArray<AStaticMeshActor*> ShadowItems; // The array that holds all the current shadow items

	bool bIsStackChecking;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetupKeyBindings(UInputComponent* PlayerInputComponent);

private:
	float RaycastRange;
	float MaxMovementSpeed;

	AStaticMeshActor* LeftHandActor;
	AStaticMeshActor* RightHandActor;
	AStaticMeshActor* BothHandActor;

	TSet<AActor*> SetOfPickupItems; // All items in the world wich can be picked up

	AStaticMeshActor* BaseItemToPick; // The item we are about to pick up	

	EHand UsedHand; // The hand we currently use for our interaction

	bool bIsDragging;
	bool bAllCanceled;

	bool bStackCheckSuccess;
	bool bItemCanBePickedUp;

	AStaticMeshActor* ItemToHandle; // The item which is currently focused by the player
	AActor* ItemToDrag; // The item we are currently dragging around
	AStaticMeshActor* ShadowBaseItem; // The shadow root item fr pickup and drop actions

	// The callback function after stack check is done
	UFUNCTION()
		void OnStackCheckIsDone(bool wasSuccessful);

	// *** Pickup ***
	void StartPickup();
	void PickupItem();
	void ShadowPickupItem();
	TArray<AStaticMeshActor*> FindAllStackableItems(AStaticMeshActor* ActorToPickup);
	AStaticMeshActor* GetItemStack(AStaticMeshActor* BaseItem); // Converts all found items to children of BaseItem
	// *** *** *** ***

	AStaticMeshActor* GetNewShadowItem(AStaticMeshActor* FromActor);

	// *** Dropping ***
	bool bIsItemDropping;
	void StartDropItem();
	void DropItem();
	void ShadowDropItem();
	// *** *** *** ***

	void UnstackItems(AStaticMeshActor* BaseItem); // De-stacks a stack of items

	void CancelActions(); // Cancels all actions
	void StopCancelActions();
	void CancelDetachItems(); // Simple detaches all children of the base item

	void SetLockedByComponent(bool bIsLocked); // Tells the PLayerCharacter that this component has exclusive control

	bool CalculateIfBothHandsNeeded(); // Check if both hands are needed to pick up the item
	float MassToCarry; // The current mass the player carries
	float MassOfLastItemPickedUp; // The mass of the item we try to pick up

	void SetMovementSpeed(float Weight);

	FHitResult CheckForCollision(FVector From, FVector To, AStaticMeshActor* ItemToSweep, TArray<AActor*> IgnoredActors);
	void DisableShadowItems();

	FVector GetPositionOnSurface(AActor* Item, FVector PointOnSurface); // Calculates the final position of an item on a surface

	FHitResult RaytraceWithIgnoredActors(TArray<AActor*> IgnoredActors, FVector StartOffset = FVector::ZeroVector, FVector TargetOffset = FVector::ZeroVector);

	// *** Input ***
	bool bLeftMouseHold;
	bool bRightMouseHold;

	void InputLeftHandPressed();
	void InputLeftHandReleased();
	void InputRightHandPressed();
	void InputRightHandReleased();
	// *** *** *** *** ***

	void OnInteractionKeyPressed(bool bIsRightKey);
	void OnInteractionKeyHold(bool bIsRightKey);
	void OnInteractionKeyReleased(bool bIsRightKey);

	void ResetComponentState();

	//After this point added by Waldemar Zeitler
public:
	bool bPickUpStarted;

	bool bRotationStarted;

	// Bool to check if the menu is active and movment should be stopped.
	bool bRotationMenuActivated;
	// Bool to check if pick up menu should be started.
	bool bPickupnMenuActivated;

	// GameMode to access the menu hud.
	AUGameModeBase* UGameMode;

	void MoveToRotationPosition();

	AStaticMeshActor* ItemInRotaitonPosition;

	bool bButtonReleased;

	void PickUpItemAfterMenu(bool leftHand);
};

