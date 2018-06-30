// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicalAnimationComponent.h"
#include "VRArmsTracking.h"
#include "ArmAnimPawn.generated.h"

/*
This class represents a pawn which is intended to support realistic arm movement.
*/
UCLASS()
class AArmAnimPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AArmAnimPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	//Startrotation of the left controller
	FRotator ControllerLYawStart;

	//Startposition of the right controller
	FRotator ControllerRYawStart;

	//If the head was reseted using the motion controller
	bool bHeadReset = false;

	//Start location of the head
	FVector HeadStartLocation;

	//Startrotation of the head
	FRotator HeadYawStart;

	//Name of the hmd device
	FString HMDName;

	//The startlocation of the mesh
	FVector MeshStartLocation;

	//Needed for the physical animation part
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UPhysicalAnimationComponent* AnimationComponent;

	//The physical animation strenght (Default 1)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float BlendWeight;

	//HMD Camera
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UCameraComponent* Camera;

	//Camera Root Scene coponent
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USceneComponent* CameraRoot;

	//Name for the upperarm bone (Default upperarm_l)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName LeftArmBoneName;

	//Left hand of the pawn
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		AActor* LeftHand;

	//Offset for the left hand rotation
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator LeftHandRotationOffset = FRotator(90, 90, 0);

	//Adjustment to position the hand at the correct end of the meshs arm
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float LocalHandAdjustmentX = -10;

	//The Skeletal Mesh (Arms Mesh)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USkeletalMeshComponent* Mesh;

	//The rotation which has to be applied at the start of the game
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator MeshRotation = FRotator(0, 90, 0);

	//Motion Controller Left
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UMotionControllerComponent* MotionControllerLeft;

	//Motion Controller Right
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UMotionControllerComponent* MotionControllerRight;

	//Name for the upperarm bone (Default upperarm_r)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName RightArmBoneName;

	//The right hand of the pawn
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		AActor* RightHand;

	//Offset for the right hand rotation
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator RightHandRotationOffset = FRotator(90, -90, 0);

	//This threshold will determine when the mesh will turn to the left /right
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int TurnThreshold = 45;

	//The height of the user
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float UserHeight = 170;

	//Path to the arms skeletal mesh 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString SkeletalPath = "SkeletalMesh'/VRArmsTracking/ArmsV2/ArmsV2.ArmsV2'";

	//Can move the shoulders forwards or backwards in order to align with real shoulders
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ShouldersLocalX = -10;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Moves the player to the cameras world position
	void ResetHeadPosition();

	//Remembers current state of players head and hands so differences can be calculated later
	void SetMovementValues();

	//Turns in multiple steps
	void TurnInSteps();

	//Counts how far mesh has been turned
	int TurnStepCounter;

	//Number of steps to turn
	int MaxTurnSteps = 10;

	//One rotation step
	FRotator RotationStep;

	//Timer for turning
	FTimerHandle TurnTimer;

	//is mesh turning
	bool bIsTurning=false;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



};
