// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#pragma once

#include "Components/ActorComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "GPickup.generated.h"

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

	// Whether or not the player's movement speed depends on the weight the player carries
	UPROPERTY(EditAnywhere, Category = "CI - Physics")
		bool bMassEffectsMovementSpeed;

	// How much mass can the player carry
	UPROPERTY(EditAnywhere, Category = "CI - Physics")
		float MaximumMassToCarry;

	AGameController * PlayerCharacter; // The player character instance

	TArray<AStaticMeshActor*> ShadowItems; // The array that holds all the current shadow items

	AStaticMeshActor* ItemToHandle; // The item which is currently focused by the player

	// *** Dropping ***
	bool bIsItemDropping;
	void DropItem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetupKeyBindings(UInputComponent* PlayerInputComponent);

private:
	AStaticMeshActor* LeftHandActor;
	AStaticMeshActor* RightHandActor;
	AStaticMeshActor* BothHandActor;

	TSet<AActor*> SetOfPickupItems; // All items in the world wich can be picked up

	AStaticMeshActor* BaseItemToPick; // The item we are about to pick up	
	// *** *** *** ***

	float MassToCarry; // The current mass the player carries
	float MassOfLastItemPickedUp; // The mass of the item we try to pick up

	void InputLeftHandPressed();
	void InputLeftHandReleased();
	void InputRightHandPressed();
	void InputRightHandReleased();

public:
	bool bPickUpStarted;

	bool bRotationStarted;

	bool bDropStarted;

	bool bInRotationPosition;

	// Bool to check if the menu is active and movment should be stopped.
	bool bRotationMenuActivated;
	// Bool to check if pick up menu should be started.
	bool bPickupMenuActivated;

	// Bool for free mouse mode
	bool bFreeMouse;

	void MoveToRotationPosition();

	AStaticMeshActor* ItemInRotaitonPosition;

	void PickUpItemAfterMenu(bool leftHand);

	// *** Input ***
	bool bLeftMouse;
	bool bRightMouse;

	APlayerController* PlayerController;

	// True if we are above an item
	bool bOverItem;

	// The item the mouse is over
	AActor* ItemToInteract;

	//Function to handle the interaction if the mouse is over the item
	UFUNCTION()
		void CustomOnBeginMouseOver(AActor* TouchedComponent);

	//Function to handle the interaction if the mouse is not over the item
	UFUNCTION()
		void CustomOnEndMouseOver(AActor* TouchedComponent);
};

