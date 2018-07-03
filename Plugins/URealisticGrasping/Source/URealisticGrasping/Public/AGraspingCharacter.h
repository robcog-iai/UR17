// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "AGraspingHand.h"
#include "UGraspingAnimationController.h"
#include "UGraspingController.h"
#include "PIDController3D.h"
#include "Runtime/Engine/Classes/Components/TextRenderComponent.h"

#include "AGraspingCharacter.generated.h"

UCLASS()
class UREALISTICGRASPING_API AGraspingCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGraspingCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//If the head was reseted using the motion controller
	bool bHeadReset = false;

	// Left hand controller
	FPIDController3D LeftPIDController;

	// Right hand controller
	FPIDController3D RightPIDController;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PGain = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float IGain = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DGain = 20.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxOutput = 350000.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotationBoost = 8000.f;

	//HMD Camera
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UCameraComponent* Camera;

	//Camera Root Scene coponent
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USceneComponent* CameraRoot;

	//Left hand of the pawn
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		AGraspingHand* LeftHand;

	//Motion Controller Left
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UMotionControllerComponent* MotionControllerLeft;

	//Motion Controller Right
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UMotionControllerComponent* MotionControllerRight;

	//The right hand of the pawn
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		AGraspingHand* RightHand;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UGraspingController* GraspingController;

	//The Animation Controller that handles all animations for both hands
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UGraspingAnimationController* AnimationController;

	// Text component to show Text in VR
	UPROPERTY(EditAnywhere)
		UTextRenderComponent* TextComponent;

	// Array of grasping types from the right hand
		TArray<FString> GrapsTypesRight;

	// Array of grasping types from the left hand
	//	TArray<FString> GrapsTypesLeft;

	// active grasp
		FString Grasp;

	// Index number of grasp type for the GrapsTypes-Array
		int IndexGraspType;

	// Offset for the left hands Roation
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator OffsetL = FRotator(90, 0, 0);;

	// Offset for the right hands Roation
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator OffsetR = FRotator(90,0,180);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Moves the player to the cameras world position
	void ResetHeadPosition();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//tells the grasping controller to grasp with left hand
	void GraspWithLeftHand(const float Input);

	//tells the grasping controller to grasp with right hand
	void GraspWithRightHand(const float Input);

	//Basic functions to get the next animation
	void NextGraspRightHand();
	void NextGraspLeftHand();
	

	// Update hand positions
	FORCEINLINE void UpdateHandLocationAndRotation(
		UMotionControllerComponent* MC,
		FPIDController3D& PIDController,
		USkeletalMeshComponent* SkelMesh,
		FRotator Offset,
		const float DeltaTime);

	//Basic functions to switch the next or previous Grasp
	void SwitchToNextGraspType();
	void SwitchToPreviousGraspType();

private:

	//adds the function responsible for loading grasping data into the grasping controller to the delegate,
	//that broadcasts everytime the player changes grasping types
	void EventSetup();
};
