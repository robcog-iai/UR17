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

				AGameController * PlayerCharacter; // The player character instance

				TArray<AStaticMeshActor*> ShadowItems; // The array that holds all the current shadow items

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
				float RaycastRange;
				float MaxMovementSpeed;

				AStaticMeshActor* LeftHandActor;
				AStaticMeshActor* RightHandActor;
				AStaticMeshActor* BothHandActor;

				TSet<AActor*> SetOfPickupItems; // All items in the world wich can be picked up

				AStaticMeshActor* BaseItemToPick; // The item we are about to pick up	

				EHand UsedHand; // The hand we currently use for our interaction

				AStaticMeshActor* ItemToHandle; // The item which is currently focused by the player

				// *** *** *** ***

				void UnstackItems(AStaticMeshActor* BaseItem); // De-stacks a stack of items


				float MassToCarry; // The current mass the player carries
				float MassOfLastItemPickedUp; // The mass of the item we try to pick up

				void SetMovementSpeed(float Weight);

				FHitResult CheckForCollision(FVector From, FVector To, AStaticMeshActor* ItemToSweep, TArray<AActor*> IgnoredActors);

				FVector GetPositionOnSurface(AActor* Item, FVector PointOnSurface); // Calculates the final position of an item on a surface

				FHitResult RaytraceWithIgnoredActors(TArray<AActor*> IgnoredActors, FVector MousePosition = FVector::ZeroVector, FVector MouseDirection = FVector::ZeroVector);

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

				bool bButtonReleased;

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

