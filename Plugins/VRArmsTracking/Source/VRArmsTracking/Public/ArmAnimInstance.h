// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "ArmAnimPawn.h"
#include "VRArmsTracking.h"
#include "ArmAnimInstance.generated.h"
/**
 * This Class calculate the positions for the head and hands. 
	You need also a blueprint with anim graph to use it
 */
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class UArmAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	//The Pawn who own this class
	AArmAnimPawn * OwningPawn;

	//The Head Transform for the Anim Graph
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform HeadWorldTransform;

	//The Left Hand World location for the Anim Graph
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector LeftHandWorldLocation;

	//The right Hand World location for the Anim Graph
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector RightHandWorldLocation;

	//The Left Hand World rotation for the Anim Graph
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator LeftHandWorldRotation;

	//The right Hand World rotation for the Anim Graph
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator RightHandWorldRotation;

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;
};
