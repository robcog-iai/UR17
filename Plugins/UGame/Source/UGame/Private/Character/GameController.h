// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#pragma once

#include "../Private/Character/Components/GMovement.h"
#include "../Private/Character/Components/GOpenClose.h"
#include "../Private/Character/Components/GPickup.h"

//#include "HUD/GameHUD.h"
#include "../UGameModeBase.h"

#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"

#include "Engine/StaticMeshActor.h"
#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "GameController.generated.h"

UENUM(BlueprintType)
enum class EScenarioType : uint8
{
	OnePersonBreakfast,
	TwoPersonBreakfast,
	FourPersonBreakfast 
};

UENUM(BlueprintType)
enum class EInteractionMode : uint8
{
	OneHandMode,
	TwoHandMode,
	TwoHandStackingMode
};

UCLASS()
class UGAME_API AGameController : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGameController();

	// The context scenario type
	UPROPERTY(EditAnywhere, Category = "CI - Scenario Setup")
		EScenarioType ScenarioType;

	// The interaction mode
	UPROPERTY(EditAnywhere, Category = "CI - Scenario Setup")
		EInteractionMode InteractionMode;

	// The grasp range of the player
	UPROPERTY(EditAnywhere, Category = "CI - Player Setup")
		float GraspRange;

	// The left hand dummy actor
	UPROPERTY(EditAnywhere, Category = "CI - Player Setup")
		AActor* LeftHandPosition;

	// The right hand dummy actor
	UPROPERTY(EditAnywhere, Category = "CI - Player Setup")
		AActor* RightHandPosition;

	// The dummy actor for the both hand position
	UPROPERTY(EditAnywhere, Category = "CI - Player Setup")
		AActor* BothHandPosition;

	// Enables the debug mode for additional information
	UPROPERTY(EditAnywhere, Category = "CI - Debug")
		bool bIsDebugMode;

	bool bRaytraceEnabled;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Components attached to this actor
	UPROPERTY(EditAnywhere, Instanced)
	UGMovement* MovementComponent;

	UPROPERTY(EditAnywhere, Instanced)
	UGOpenClose* OpenCloseComponent;

	UPROPERTY(EditAnywhere, Instanced)
	UGPickup* PickupComponent;

	// ******************************

	AStaticMeshActor* FocussedActor; // The currently focussed actor

	UActorComponent* LockedByComponent; // If this isn't nuullptr this component has exclusive rights

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FHitResult GetRaycastResult();

private:
	ACharacter* Character;
	FHitResult RaycastResult; // The result of the constant raycasting
	FVector PreviousPosition;
	FRotator PreviousRotation;

	TSet<AActor*> SetOfInteractableItems; // All interactable actors 
	TMap<AActor*, TArray<UMaterialInterface*>> DefaultActorMaterial;	

	bool bIsMovementLocked; // Whether or not the player can move
	bool bComponentsLocked;

	// Creates all actor components.
	void SetupComponentsOnConstructor();

	void StartRaytrace(FVector Start = FVector::ZeroVector, FVector DirectionIn = FVector::ZeroVector);

	void CheckIntractability();

	void SetPlayerMovable(bool bIsMovable);

	void SetupScenario();

	//After this point the code is written by Wlademar Zeitler
public:
	APlayerController* PlayerController;

	/** Checks if there are interactable objects in the viewport. */
	bool CheckForVisibleObjects();

 /** Game HUD. */
 //AGameHUD* PickupHUD;

 // GameMode to access the menu hud.
 AUGameModeBase* UGameMode;

 //Function to handle the interaction
 UFUNCTION()
  void CustomOnBeginMouseOver(AActor* TouchedComponent);
};
