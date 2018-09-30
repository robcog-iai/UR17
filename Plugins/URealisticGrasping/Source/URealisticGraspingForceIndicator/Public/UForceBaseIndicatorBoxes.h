// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "ForceBaseIndicatorInterface.h"
#include "Runtime/Engine/Classes/PhysicsEngine/ConstraintInstance.h"
#include "UForceBaseIndicatorBoxes.generated.h"

/**
 * This class should demonstrate how you can create and use a ForceBaseIndicatorInterface in order to
 * visualize the current force as a box.
 */
UCLASS()
class UREALISTICGRASPINGFORCEINDICATOR_API UForceBaseIndicatorBoxes : public UObject, public IForceBaseIndicatorInterface
{
	GENERATED_BODY()
public:
	UForceBaseIndicatorBoxes();
	~UForceBaseIndicatorBoxes();

	//Overrided function from the interface
	virtual void CreateNewHandDebug(ASkeletalMeshActor* CurrentHand, bool bIsRightHand, float DeltaTime);

	//Overrided function from the interface
	virtual void SetGraspingData(FHandAnimationData Data, bool bIsRightHand);

private:

	//Function to set the constraints map
	void SetNewConstraints(ASkeletalMeshActor* CurrentHand, TArray<FString> BoneNames, bool bIsRightHand);

	//Current max force (used for the interpolate between two colors)
	float CurrentMaxForce = 1.0f;

	//Array with bone names for the left and right hand
	TArray<FString> RightBoneNames;
	TArray<FString> LeftBoneNames;

	//Tmap with all constraints for every bone (needed to get the force)
	TMap<FString, FConstraintInstance*> BoneToConstraintRight;
	TMap<FString, FConstraintInstance*> BoneToConstraintLeft;
};
