// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#pragma once

#include "../Private/Character/Components/GMovement.h"
#include "../Private/Character/Components/GOpenClose.h"
#include "../Private/Character/Components/GPickup.h"

#include "../HUD/GameHUD.h"
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

	UActorComponent* LockedByComponent; // If this isn't nullptr this component has exclusive rights

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	ACharacter * Character;

	bool bIsMovementLocked; // Whether or not the player can move
	bool bComponentsLocked;

	void SetPlayerMovable(bool bIsMovable);

	void SetupScenario();

public:
	APlayerController * PlayerController;

	/** Game HUD. */
	AGameHUD* PickupHUD;

	// GameMode to access the menu hud.
	AUGameModeBase* UGameMode;

private:
	//Mouseposition coordinates, for the rotation of the object
	float XMousePosition;
	float YMousePosition;
};
