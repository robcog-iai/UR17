// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#define TAG_KEY_OPENCLOSABLE "OpenCloseable"

// Fill out your copyright notice in the Description page of Project Settings.
#include "GOpenClose.h"

#include "../Private/Character/GameController.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TagStatics.h"
#include "DrawDebugHelpers.h"



// Sets default values for this component's properties
UGOpenClose::UGOpenClose()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ForceToApply = 400000.0f;
}


// Called when the game starts
void UGOpenClose::BeginPlay()
{
	Super::BeginPlay();

	SetOfOpenCloasableItems = FTagStatics::GetActorSetWithKeyValuePair(GetWorld(), "ClickInteraction", TAG_KEY_OPENCLOSABLE, "True");

	if (PlayerCharacter == nullptr) UE_LOG(LogTemp, Fatal, TEXT("UGOpenClose::BeginPlay: The PlayerCharacter was not assigned. Restarting the editor might fix this."));
}


// Called every frame
void UGOpenClose::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PlayerCharacter == nullptr) return;
	if (PlayerCharacter->LockedByComponent != nullptr &&  PlayerCharacter->LockedByComponent != this) return; // Another component has exclusive rights for the controlls

	OnInteractionKeyHold(bRightMouseHold, DeltaTime);
}

void UGOpenClose::SetupKeyBindings(UInputComponent * PlayerInputComponent)
{
	PlayerInputComponent->BindAction("LeftHandAction", IE_Pressed, this, &UGOpenClose::InputLeftHandPressed);
	PlayerInputComponent->BindAction("LeftHandAction", IE_Released, this, &UGOpenClose::InputLeftHandReleased);

	PlayerInputComponent->BindAction("RightHandAction", IE_Pressed, this, &UGOpenClose::InputRightHandPressed);
	PlayerInputComponent->BindAction("RightHandAction", IE_Released, this, &UGOpenClose::InputRightHandReleased);
}

void UGOpenClose::SetLockedByComponent(bool bIsLocked)
{
	if (bIsLocked) {
		PlayerCharacter->LockedByComponent = this;
	}
	else {
		PlayerCharacter->LockedByComponent = nullptr;
	}
}

void UGOpenClose::AddForceToObject(float DeltaTime)
{
	float MouseX;
	float MouseY;
	GetWorld()
		->GetFirstPlayerController()
		->GetInputMouseDelta(MouseX, MouseY);

	AStaticMeshActor* Actor = Cast<AStaticMeshActor>(ClickedActor);

	if (Actor == nullptr) return;

	// Get forward vector than add force along this vector
	float ForceToAdd = -MouseY * DeltaTime * ForceToApply;
	UStaticMeshComponent* Mesh = Actor->GetStaticMeshComponent();
	Mesh->AddForce(Actor->GetActorForwardVector() * ForceToAdd, NAME_None, true);
}

void UGOpenClose::InputLeftHandPressed()
{
	if (bLeftMouseHold) return; // We already clicked that key
	if (bRightMouseHold) return; // Ignore this call if the other key has been pressed

	bLeftMouseHold = true;
	OnInteractionKeyPressed(false);
}

void UGOpenClose::InputLeftHandReleased()
{
	if (bLeftMouseHold == false) return; // We can't release if we didn't clicked first
	if (bRightMouseHold) return; // Ignore this call if the other key has been pressed

	bLeftMouseHold = false;
	OnInteractionKeyReleased(false);
}

void UGOpenClose::InputRightHandPressed()
{
	if (bRightMouseHold) return; // We already clicked that key
	if (bLeftMouseHold) return; // Ignore this call if the other key has been pressed

	bRightMouseHold = true;
	OnInteractionKeyPressed(true);
}

void UGOpenClose::InputRightHandReleased()
{
	if (bRightMouseHold == false) return;  // We can't release if we didn't clicked first
	if (bLeftMouseHold) return; // Ignore this call if the other key has been pressed

	bRightMouseHold = false;
	OnInteractionKeyReleased(true);
}

void UGOpenClose::OnInteractionKeyPressed(bool bIsRightKey)
{
	if (PlayerCharacter->FocussedActor != nullptr && SetOfOpenCloasableItems.Contains(PlayerCharacter->FocussedActor))
	{
		bool bRightHandEmpty = true;
		bool bLeftHandEmpty = true;
		bool bTwoHandMode = true;

		// Check if hands are empty
		if (PlayerCharacter->PickupComponent != nullptr) {
			bRightHandEmpty = PlayerCharacter->PickupComponent->ItemInRightHand == nullptr;
			bLeftHandEmpty = PlayerCharacter->PickupComponent->ItemInLeftHand == nullptr;
			bTwoHandMode = PlayerCharacter->PickupComponent->bTwoHandMode;
		}

		// In TwoHandMode we can interact with both hands if they are empty. In OneHandMode we need all hands to be empty
		bool bCanInteractTwoHandMode = bTwoHandMode && (bIsRightKey && bRightHandEmpty || bIsRightKey == false && bLeftHandEmpty);
		bool bCanInteractOneHandMode = !bTwoHandMode && bRightHandEmpty && bLeftHandEmpty;

		if (bCanInteractTwoHandMode || bCanInteractOneHandMode) {
			// Start interacting

			ClickedActor = PlayerCharacter->FocussedActor;

			PlayerCharacter->MovementComponent->SetMovable(false);
			SetLockedByComponent(true);
		}
		else {
			GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, "Hand(s) not empty. Can't interact", false);
		}

	}
}

void UGOpenClose::OnInteractionKeyHold(bool bIsRightKey, float DeltaTime)
{
	if (ClickedActor != nullptr)
	{
		AddForceToObject(DeltaTime);
		ClickedActor->GetStaticMeshComponent()->SetRenderCustomDepth(true); // Keep object highlighted
	}
}

void UGOpenClose::OnInteractionKeyReleased(bool bIsRightKey)
{
	if (ClickedActor != nullptr)
	{
		// Stop interacting
		ClickedActor->GetStaticMeshComponent()->SetRenderCustomDepth(false);
		PlayerCharacter->MovementComponent->SetMovable(true);

		SetLockedByComponent(false);

		ClickedActor = nullptr;
	}
}

