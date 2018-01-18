// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: David Brinkmann

#pragma once

#include "../Private/Character/Components/CMovement.h"
#include "../Private/Character/Components/COpenClose.h"
#include "../Private/Character/Components/CPickup.h"
#include "../Private/Character/Components/CLogger.h"
#include "SLLevelInfo.h"

#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"

#include "Engine/StaticMeshActor.h"
#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "CharacterController.generated.h"

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
class CLICKINTERACTION_API ACharacterController : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterController();

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

	ASLLevelInfo* LevelInfo;

	bool bRaytraceEnabled;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Components attached to this actor
	UPROPERTY(EditAnywhere, Instanced)
	UCMovement* MovementComponent;

	UPROPERTY(EditAnywhere, Instanced)
	UCOpenClose* OpenCloseComponent;

	UPROPERTY(EditAnywhere, Instanced)
	UCPickup* PickupComponent;

	UPROPERTY(EditAnywhere, Instanced)
	UCLogger* LogComponent;
	// ******************************

	AStaticMeshActor* FocussedActor; // The currently focussed actor

	UActorComponent* LockedByComponent; // If this isn't nuullptr this component has exclusive rights

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FHitResult GetRaycastResult();

	// Hand individual
	FOwlIndividualName LeftHandIndividual;
	FOwlIndividualName RightHandIndividual;

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

	void StartRaytrace();

	void CheckIntractability();

	void SetPlayerMovable(bool bIsMovable);

	void SetupScenario();

	void GenerateLevelInfo();

	//After this point the code is written by Wlademar Zeitler
public:
	APlayerController* PlayerController;
};
