// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: David Brinkmann

#pragma once


#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Engine.h"
#include "CMovement.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CLICKINTERACTION_API UCMovement : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCMovement();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Handles moving forward/backward
	void MoveForward(const float Val);

	// Handles strafing Left/Right
	void MoveRight(const float Val);

	void AddControllerPitchInput(const float Val);

	void AddControllerYawInput(const float Val);

	// The maximum (or default) speed
	UPROPERTY(EditAnywhere, Category = "CI - Speed Setup")
		float MaxMovementSpeed;

	// The minimum movement speed. Used if player picks up items which then effects speed
	UPROPERTY(EditAnywhere, Category = "CI - Speed Setup")
		float MinMovementSpeed;

	float CurrentSpeed;

protected:


public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetupKeyBindings(UInputComponent* PlayerInputComponent);

	// Renders the player movable/unmovable
	void SetMovable(bool bCanMove);

	// Toggle crouch
	void ToggleCrouch();

private:
	// Regulates the max speed
	float SpeedLimit;

	ACharacter* Character;

	// Stores the default speed set at start game
	float DefaultSpeed;

	bool bCanMove;

	// *** Crouching ***
	float DefaultHeight;
	bool bIsCrouching;

	// Smooth crouch timer handle
	FTimerHandle CrouchTimer;

	// Smooth crouch
	void SmoothCrouch();

	// Smooth stand up
	void SmoothStandUp();
	// *** *** *** *** *** *** 

	// After this point done by Waldemar Zeitler
public:
	/**
	*  Calculates a rising speed up. This funktion is done with the PT1-Glied function. 
	*  The "Sprungantwort" function is used: a(t) = K(1-e^(-t/T)).
	*  https://de.wikipedia.org/wiki/PT1-Glied
	*  @param TimeStep - Current time to rise the speed
	*  @return the new walking speed
	*/
	float VelocitySpeedUp(float TimeStep);

	// SpeedUpValue for the how fast the character gets his max speed. 
	UPROPERTY(EditAnywhere, Category = "CI - Speed Setup")
		float SpeedUpValue;

private:
	// Time taken from the tick function, to calculate the speed up
	float SpeedUpTime;

	// Bool to check if the character is moving.
	bool bIsMoving;

};
