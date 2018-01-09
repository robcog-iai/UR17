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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Camera Root Scene coponent
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* CameraRoot;

	//HMD Camera
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCameraComponent* Camera;

	//Motion Controller Left
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMotionControllerComponent* MotionControllerLeft;

	//Motion Controller Right
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UMotionControllerComponent* MotionControllerRight;

	//The Skeletal Mesh (Arms Mesh)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMeshComponent* Mesh;

	//Needed for the physical animation part
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UPhysicalAnimationComponent* AnimationComponent;

	//Name for the upperarm bone (Default upperarm_r)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName RightArmBoneName;

	//Name for the upperarm bone (Default upperarm_l)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName LeftArmBoneName;

	//Name for the constraints profile for the selected arms mesh
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ConstraintsProfileName;

	//The physical animation strenght (Default 1)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BlendWeight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float UserHeight = 170;

	FString HMDName;
	
};
