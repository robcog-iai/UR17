// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#define PLUGIN_TAG "UGame"
#define TAG_KEY_PICKUP "Pickup"

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
    , bDropStarted(false)
    , ItemInRotaitonPosition(nullptr)
    , bPickUpStarted(false)
    , bFreeMouse(false)
    , bOverItem(false)
    , ItemToInteract(nullptr)
    , bRightMouse(false)
    , bDropping(false)
    , bRotating(false)
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;

    bInRotationPosition = false;

    // Initiate the starting state
    bNoItemHold = true;
    bItemInLeftHand = false;
    bItemInRightHand = false;
    bInRotationPosition = false;
    bPickupAndRotationMenu = false;
    bPickupLeftRightHandMenu = false;
    bDropItemMenu = false;

    bool bCallMenu = false;
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
    // Spawn new actors and set them up
    LeftHandActor = GetWorld()->SpawnActor<AStaticMeshActor>();
    RightHandActor = GetWorld()->SpawnActor<AStaticMeshActor>();
    BothHandActor = GetWorld()->SpawnActor<AStaticMeshActor>();

    LeftHandActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
    RightHandActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
    BothHandActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);

    FVector PlayerPosition = PlayerCharacter->GetTargetLocation();

    LeftHandActor->SetActorRelativeLocation(PlayerPosition + FVector(40, -40, 28));
    RightHandActor->SetActorRelativeLocation(PlayerPosition + FVector(40, 40, 28));
    BothHandActor->SetActorRelativeLocation(PlayerPosition + FVector(60, 0, 10));

    LeftHandActor->AttachToActor(PlayerCharacter, FAttachmentTransformRules::KeepWorldTransform);
    RightHandActor->AttachToActor(PlayerCharacter, FAttachmentTransformRules::KeepWorldTransform);
    BothHandActor->AttachToActor(PlayerCharacter, FAttachmentTransformRules::KeepWorldTransform);

    LeftHandActor->GetStaticMeshComponent()->SetSimulatePhysics(false);
    RightHandActor->GetStaticMeshComponent()->SetSimulatePhysics(false);
    BothHandActor->GetStaticMeshComponent()->SetSimulatePhysics(false);

    LeftHandActor->GetStaticMeshComponent()->SetCollisionProfileName("NoCollision");
    RightHandActor->GetStaticMeshComponent()->SetCollisionProfileName("NoCollision");
    BothHandActor->GetStaticMeshComponent()->SetCollisionProfileName("NoCollision");

    // Initilize the player controller to get the mouse axis
    PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
    PlayerController->bEnableMouseOverEvents = true;
}

// Called every frame
void UGPickup::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    
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

    bRightMouse = false;
    bFreeMouse = false;
}

void UGPickup::InputLeftHandReleased()
{
    bLeftMouse = false;
}

void UGPickup::InputRightHandPressed()
{
    bRightMouse = true;

    HandleRightClick();
    
    // Debug information, for the current state
    UE_LOG(LogTemp, Warning, TEXT("bIsItemDropping: %s"), bIsItemDropping ? TEXT("True\n") : TEXT("False\n"));
    UE_LOG(LogTemp, Warning, TEXT("bPickUpStarted: %s"), bPickUpStarted ? TEXT("True\n") : TEXT("False\n"));
    UE_LOG(LogTemp, Warning, TEXT("bRotationStarted: %s"), bRotationStarted ? TEXT("True\n") : TEXT("False\n"));
    UE_LOG(LogTemp, Warning, TEXT("bDropStarted: %s"), bDropStarted ? TEXT("True\n") : TEXT("False\n"));
    UE_LOG(LogTemp, Warning, TEXT("bInRotationPosition: %s"), bInRotationPosition ? TEXT("True\n") : TEXT("False\n"));
    UE_LOG(LogTemp, Warning, TEXT("bFreeMouse: %s"), bFreeMouse ? TEXT("True\n") : TEXT("False\n"));
    UE_LOG(LogTemp, Warning, TEXT("bLeftMouse: %s"), bLeftMouse ? TEXT("True\n") : TEXT("False\n"));
    UE_LOG(LogTemp, Warning, TEXT("bRightMouse: %s"), bRightMouse ? TEXT("True\n") : TEXT("False\n"));
    UE_LOG(LogTemp, Warning, TEXT("bOverItem: %s"), bOverItem ? TEXT("True\n") : TEXT("False\n"));
}

void UGPickup::InputRightHandReleased()
{
    bRightMouse = false;
}

void UGPickup::HandleRightClick()
{
    bFreeMouse = true;

    // Right mouse was pressed, check which menu needs to be activated
    if (ItemToInteract != nullptr)
    {
        ItemToHandle = Cast<AStaticMeshActor>(ItemToInteract);

        // Item ist not in right or left hand
        if (!(ItemToHandle == ItemInLeftHand) && !(ItemToHandle == ItemInRightHand) && !bInRotationPosition)
        {
            bPickupAndRotationMenu = true;
            bCallMenu = true;
        }
        else if (ItemToHandle == ItemInLeftHand || ItemToHandle == ItemInRightHand) // Item is in on of the hands, call the drop menu
        {
            bDropItemMenu = true;
            bCallMenu = true;
        }
        else // Item is in rotation position, call the left right pickup menu
        {
            bPickupLeftRightHandMenu = true;
            bCallMenu = true;
            
        }
        return;
    }
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

    bPickupAndRotationMenu = false;
    bFreeMouse = false;

    bRotationStarted = false;

    ItemToHandle = nullptr;
    BaseItemToPick = nullptr;
}

void UGPickup::DropItem()
{
    ItemToHandle->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    ItemToHandle->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ItemToHandle->GetStaticMeshComponent()->SetSimulatePhysics(true);

    if (ItemInRightHand == ItemToHandle)
    {
        ItemInRightHand = nullptr;
    }      
    else
    {
        ItemInLeftHand = nullptr;
    }
        
    ItemToHandle = nullptr;
    bDropping = false;
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