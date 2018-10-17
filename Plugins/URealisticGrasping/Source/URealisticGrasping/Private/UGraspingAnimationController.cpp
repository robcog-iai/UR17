// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "UGraspingAnimationController.h"
#include "UGraspingController.h"

// Sets default values for this component's properties
UGraspingAnimationController::UGraspingAnimationController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGraspingAnimationController::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UGraspingAnimationController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGraspingAnimationController::SetMeshName(FString Name, bool bIsRightHand)
{
	//Set the parameters and get all current available animations for the current hand
	if (bIsRightHand)
	{
		//Set the name for the right hand and get all animations
		RightHandMeshName = Name;
		RightHandAnimationNames = ReadWrite.ReadNames(Name);
		RightHandAnimations = TMap<FString, FHandAnimationData>();
	}
	else
	{
		//Set the name for the left hand and get all animations
		LeftHandMeshName = Name;
		LeftHandAnimationNames = ReadWrite.ReadNames(Name);
		LeftHandAnimations = TMap<FString, FHandAnimationData>();
	}
}

TArray<FString> UGraspingAnimationController::GetAnimationNames(bool bIsRightHand)
{
	if (bIsRightHand)
	{
		return RightHandAnimationNames;
	}
	return LeftHandAnimationNames;
}

void UGraspingAnimationController::SetNewAnimation(FString AnimationName, bool bIsRightHand)
{
	FHandAnimationData CurrentAnimation;
	if (bIsRightHand)
	{
		//If the animation doesn't exist return
		if(!RightHandAnimationNames.Contains(AnimationName)) return;

		//checks if the animation was already loaded in the past 
		if (RightHandAnimations.Contains(AnimationName))
		{
			CurrentAnimation = *RightHandAnimations.Find(AnimationName);
		}
		else
		{
			//Read the Animation from a file
			FHandAnimationData Temp = ReadWrite.ReadFile(RightHandMeshName, AnimationName);
			CurrentAnimation = Temp;
			CurrentRightAnimation = &CurrentAnimation;
			RightHandAnimations.Add(AnimationName, CurrentAnimation);
		}

		//Send a update to all binded functions
		if (OnNextAnimationR.IsBound())
		{
			OnNextAnimationR.Broadcast(CurrentAnimation);
		}
		
	}
	else
	{
		//If the animation doesn't exist return
		if (!LeftHandAnimationNames.Contains(AnimationName)) return;

		//checks if the animation was already loaded in the past 
		if (LeftHandAnimations.Contains(AnimationName))
		{
			CurrentAnimation = *LeftHandAnimations.Find(AnimationName);
		}
		else
		{
			//Read the Animation from a file
			FHandAnimationData Temp = ReadWrite.ReadFile(LeftHandMeshName, AnimationName);
			CurrentAnimation = Temp;
			CurrentLeftAnimation = &CurrentAnimation;
			LeftHandAnimations.Add(AnimationName, CurrentAnimation);
		}

		//Send a update to all binded functions
		if (OnNextAnimationL.IsBound())
		{
			OnNextAnimationL.Broadcast(CurrentAnimation);
		}
	}
}

FHandAnimationData UGraspingAnimationController::GetNextAnimation(bool bIsRightHand)
{
	if (bIsRightHand)
	{
		//If we reached the end of the array start at the beginning
		if (CurrentRightGraspIndex + 1 == RightHandAnimationNames.Num())
		{
			CurrentRightGraspIndex = 0;
		}

		//Set the next animation 
		SetNewAnimation(RightHandAnimationNames[CurrentRightGraspIndex + 1], bIsRightHand);
		return *CurrentRightAnimation;
	}
	else
	{
		//If we reached the end of the array start at the beginning
		if (CurrentLeftGraspIndex + 1 == LeftHandAnimationNames.Num())
		{
			CurrentLeftGraspIndex = 0;
		}

		//Set the next animation 
		SetNewAnimation(LeftHandAnimationNames[CurrentLeftGraspIndex + 1], bIsRightHand);
		return *CurrentLeftAnimation;
	}
}

