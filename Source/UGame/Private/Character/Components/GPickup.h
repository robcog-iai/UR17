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

    // Items hold in the left and right hand
    AStaticMeshActor* ItemInLeftHand;
    AStaticMeshActor* ItemInRightHand;

    // The player character instance
    AGameController * PlayerCharacter;

    // The item which is currently focused by the player
    AStaticMeshActor* ItemToHandle;

    // Sets item values, so that the item is dropable
    void DropItem();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    void SetupKeyBindings(UInputComponent* PlayerInputComponent);

    // Mouse button press events
    void InputLeftHandPressed();
    void InputLeftHandReleased();
    void InputRightHandPressed();
    void InputRightHandReleased();

private:
    // Actors for the hands positions and the rotation position
    AStaticMeshActor* LeftHandActor;
    AStaticMeshActor* RightHandActor;
    AStaticMeshActor* BothHandActor;

    // All items in the world wich can be picked up
    TSet<AActor*> SetOfPickupItems;

    // The item we are about to pick up	
    AStaticMeshActor* BaseItemToPick;   

    void HandleRightClick();

public:
    // There is an object in on of the hands
    bool bItemInLeftHand;
    bool bItemInRightHand;

    // Hands are empty
    bool bNoItemHold;

    // Checks for which menu should be called.
    bool bPickupAndRotationMenu;
    bool bPickupLeftRightHandMenu;
    bool bDropItemMenu;

    // Check for the pick up mode
    bool bPickUpStarted;

    // Check for the rotation mode
    bool bRotationStarted;

    // Check for the drop start (positioning)
    bool bDropStarted;

    // Check for the drop mode
    bool bIsItemDropping;

    // Check if the item is in the rotation positon, to allow rotation
    bool bInRotationPosition;
    bool bRotating;

    bool bDropping;

    bool bCallMenu;

    // Bool for free mouse mode
    bool bFreeMouse;

    // Move the picked up object to the rotation position
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

