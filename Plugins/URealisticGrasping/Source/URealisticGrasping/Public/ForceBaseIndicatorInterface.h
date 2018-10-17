// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Animation/SkeletalMeshActor.h"
#include "UAnimationDataStructure.h"
#include "ForceBaseIndicatorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UForceBaseIndicatorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This Interface is used to visualize the force when 
 * you grasp something
 */
class UREALISTICGRASPING_API IForceBaseIndicatorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	/*
	This function is get called every tick and gives you the information, witch hand should be 
	currently visualized. This function has to be implemented.
	@param CurrentHand, the current hand that should be visualized
	@param bIsRightHand, gives you the information if this is the right or the left hand
	@param DeltaTime, the tick time
	*/
	UFUNCTION(Category = "Force Base Indicator")
		virtual void CreateNewHandDebug(ASkeletalMeshActor* CurrentHand, bool bIsRightHand, float DeltaTime) = 0;

	/*
	This function is a helper function to lerp between two colors. You don't need to
	implemate this one.
	@param LowColor, the low color for the interpolation
	@param HightColor, the hight color for the interpolation
	@param Alpha, the alpha value between 0 and 1
	@return the new color between the other two colors
	*/
	UFUNCTION(Category = "Force Base Indicator")
		virtual FColor LinearColorInterpolate(FColor LowColor, FColor HightColor, float Alpha);

	/*
	This Function calculates the force between one point and one force vector.
	This is also a helper function and you don't need to implemate this one.
	@param Location, the location vector
	@param Force, the force vector
	@return, the distance between two points
	*/
	UFUNCTION(Category = "Force Base Indicator")
		virtual float GetForce(FVector Location, FVector Force);

	/*
	This function gets called every time when the animation gets updated/changed
	You need to implemate this function.
	@param Data, the new animation data
	@param bIsRightHand, the information for which hand
	*/
	UFUNCTION(Category = "Force Base Indicator")
		virtual void SetGraspingData(FHandAnimationData Data, bool bIsRightHand) = 0;
	
};
