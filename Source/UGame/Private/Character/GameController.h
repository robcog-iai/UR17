// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#pragma once

#include "../Private/Character/Components/GMovement.h"
#include "../Private/Character/Components/GPickup.h"

#include "../HUD/GameHUD.h"
#include "../UGameModeBase.h"

#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"

#include "Engine/StaticMeshActor.h"
#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "GameController.generated.h"

UCLASS()
class UGAME_API AGameController : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AGameController();

    // The grasp range of the player
    UPROPERTY(EditAnywhere, Category = "CI - Player Setup")
        float GraspRange;


protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Components attached to this actor
    UPROPERTY(EditAnywhere, Instanced)
        UGMovement* MovementComponent;

    UPROPERTY(EditAnywhere, Instanced)
        UGPickup* PickupComponent;

    UPROPERTY(EditAnywhere)
        float RotationRate;

    UPROPERTY(EditAnywhere)
        float DropMovmentRate;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void AddPitchInput(const float Val);

    void AddYawInput(const float Val);

private:
    ACharacter * Character;

    bool bIsMovementLocked; // Whether or not the player can move
    bool bComponentsLocked;

    void SetPlayerMovable(bool bIsMovable);

public:
    APlayerController * PlayerController;

    // Game HUD 
    AGameHUD* PickupHUD;

private:
    // Shows or hids the mouse cursor
    void ShowCursor(bool bShow);

    void SetupRotationOrDrop();

    bool bMenuOpen;

};
