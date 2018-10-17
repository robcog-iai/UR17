// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "UForceBaseIndicatorBoxes.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"

UForceBaseIndicatorBoxes::UForceBaseIndicatorBoxes()
{
}

UForceBaseIndicatorBoxes::~UForceBaseIndicatorBoxes()
{
}

void UForceBaseIndicatorBoxes::CreateNewHandDebug(ASkeletalMeshActor * CurrentHand, bool bIsRightHand, float DeltaTime)
{
	//Checks if the current hand is valid
	if (!CurrentHand) return;

	USkeletalMeshComponent* const SkelComp = CurrentHand->GetSkeletalMeshComponent();

	//visualize everything for the right hand
	if (bIsRightHand)
	{
		//If no bones are set return
		if (RightBoneNames.Num() <= 0) return;

		TMap<FString, float> BoneForceMap;

		//If the constraints map wasn't created, create it
		if (!BoneToConstraintRight.Num()) SetNewConstraints(CurrentHand, RightBoneNames, bIsRightHand);

		//Go through all bones
		for (FString Elem : RightBoneNames)
		{
			//Get the constraints instance and get the location of this bone
			FConstraintInstance* CurrentInstance = *BoneToConstraintRight.Find(Elem);
			FVector Location = SkelComp->GetBoneLocation(FName(*Elem), EBoneSpaces::WorldSpace);

			FVector LinearForce;
			FVector AngularForce;

			//Get the current force vector and calculate the force with the helper function
			CurrentInstance->GetConstraintForce(LinearForce,AngularForce);
			float Force = GetForce(CurrentInstance->GetConstraintLocation(), AngularForce);

			//If the current force value is higher than the old one, overwrite it
			if (CurrentMaxForce < Force) CurrentMaxForce = Force;

			BoneForceMap.Add(Elem, Force);
		}

		//Go through all bones and draw the boxes
		for (auto Elem : BoneForceMap)
		{
			//Get the body instance to get the size of the bone collision box
			FBodyInstance* BodyInstance = SkelComp->GetBodyInstance(FName(*Elem.Key));
			if (!BodyInstance || !GetWorld()) return;
			FBox Box = BodyInstance->GetBodyBounds();

			//Draw the box and use the helper function to interpolate between two colors
			DrawDebugBox(GetWorld(), Box.GetCenter(), Box.GetExtent(), 
				LinearColorInterpolate(FColor::Green, FColor::Red, CurrentMaxForce/Elem.Value), false, DeltaTime*2);
		}

	}
	else
	{
		//If no bones are set return
		if (LeftBoneNames.Num() <= 0) return;

		TMap<FString, float> BoneForceMap;

		//If the constraints map wasn't created, create it
		if (!BoneToConstraintLeft.Num()) SetNewConstraints(CurrentHand, LeftBoneNames, bIsRightHand);

		//Go through all bones
		for (FString Elem : LeftBoneNames)
		{
			//Get the constraints instance and get the location of this bone
			FConstraintInstance* CurrentInstance = *BoneToConstraintLeft.Find(Elem);
			FVector Location = SkelComp->GetBoneLocation(FName(*Elem), EBoneSpaces::WorldSpace);

			FVector LinearForce;
			FVector AngularForce;

			//Get the current force vector and calculate the force with the helper function
			CurrentInstance->GetConstraintForce(LinearForce, AngularForce);
			float Force = GetForce(CurrentInstance->GetConstraintLocation(), AngularForce);

			//If the current force value is higher than the old one, overwrite it
			if (CurrentMaxForce < Force) CurrentMaxForce = Force;

			BoneForceMap.Add(Elem, Force);
		}

		//Go through all bones and draw the boxes
		for (auto Elem : BoneForceMap)
		{
			//Get the body instance to get the size of the bone collision box
			FBodyInstance* BodyInstance = SkelComp->GetBodyInstance(FName(*Elem.Key));
			if (!BodyInstance || !GetWorld()) return;
			FBox Box = BodyInstance->GetBodyBounds();

			//Draw the box and use the helper function to interpolate between two colors
			DrawDebugBox(GetWorld(), Box.GetCenter(), Box.GetExtent(), 
				LinearColorInterpolate(FColor::Green, FColor::Red, CurrentMaxForce / Elem.Value), false, DeltaTime * 2);
		}
	}
}

void UForceBaseIndicatorBoxes::SetGraspingData(FHandAnimationData Data, bool bIsRightHand)
{
	//Set the grasping data for the left or right hand
	if (bIsRightHand)
	{
		//Sets the bone names and resets the constraints map
		RightBoneNames = Data.GetAllBoneNames();
		BoneToConstraintRight = TMap<FString, FConstraintInstance*>();
	}
	else
	{
		//Sets the bone names and resets the constraints map
		LeftBoneNames = Data.GetAllBoneNames();
		BoneToConstraintLeft = TMap<FString, FConstraintInstance*>();
	}
}

void UForceBaseIndicatorBoxes::SetNewConstraints(ASkeletalMeshActor * CurrentHand, TArray<FString> BoneNames, bool bIsRightHand)
{
	USkeletalMeshComponent* const SkelComp = CurrentHand->GetSkeletalMeshComponent();
	TMap<FString, FConstraintInstance*> BoneToConstraintMap;

	//Go through all bones 
	for (FString CurrentBone : BoneNames)
	{
		//Go through all constraints for every bone
		for (FConstraintInstance* NewConstraint : SkelComp->Constraints)
		{
			//if we found the constraint then add it to the map
			if (NewConstraint->ConstraintBone1.ToString() == CurrentBone) 
			{
				BoneToConstraintMap.Add(CurrentBone, NewConstraint);
				break;
			}
		}
	}

	if (bIsRightHand)
	{
		BoneToConstraintRight = BoneToConstraintMap;
	}
	else
	{
		BoneToConstraintLeft = BoneToConstraintMap;
	}
}
