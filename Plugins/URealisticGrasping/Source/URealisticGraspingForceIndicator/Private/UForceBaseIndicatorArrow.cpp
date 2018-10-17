// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "UForceBaseIndicatorArrow.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"

UForceBaseIndicatorArrow::UForceBaseIndicatorArrow()
{
}

UForceBaseIndicatorArrow::~UForceBaseIndicatorArrow()
{
}


void UForceBaseIndicatorArrow::CreateNewHandDebug(ASkeletalMeshActor * CurrentHand, bool bIsRightHand, float DeltaTime)
{
	//Checks if the current hand is valid
	if (!CurrentHand) return;

	USkeletalMeshComponent* const SkelComp = CurrentHand->GetSkeletalMeshComponent();

	if (bIsRightHand)
	{
		//If no bones are set return
		if (RightBoneNames.Num() <= 0) return;

		TArray<float> BoneForceArray;
		TArray<FVector> BoneLocation;
		TArray<FVector> ForceLocation;

		//If the constraints map wasn't created, create it
		if (!BoneToConstraintRight.Num()) SetNewConstraints(CurrentHand, RightBoneNames, bIsRightHand);

		//Go through all bones
		for (int Index = 0; Index < RightBoneNames.Num(); Index++)
		{
			//Get the constraints instance and get the location of this bone
			FConstraintInstance* CurrentInstance = *BoneToConstraintRight.Find(RightBoneNames[Index]);
			FVector Location = SkelComp->GetBoneLocation(FName(*RightBoneNames[Index]), EBoneSpaces::WorldSpace);

			FVector LinearForce;
			FVector AngularForce;

			//Get the current force vector and calculate the force with the helper function
			CurrentInstance->GetConstraintForce(LinearForce, AngularForce);
			float Force = GetForce(CurrentInstance->GetConstraintLocation(), AngularForce);

			//If the current force value is higher than the old one, overwrite it
			if (CurrentMaxForce < Force) CurrentMaxForce = Force;

			BoneForceArray.Add(Force);
			BoneLocation.Add(Location);
			ForceLocation.Add(Location + AngularForce);
		}

		//Go through all bones and draw the arrows
		for (int Index = 0; Index < RightBoneNames.Num(); Index++)
		{
			if (!GetWorld()) return;

			//Draw the arrow and use the helper function to interpolate between two colors
			DrawDebugDirectionalArrow(GetWorld(), BoneLocation[Index], ForceLocation[Index], 5.0f,
				LinearColorInterpolate(FColor::Green, FColor::Red, CurrentMaxForce / BoneForceArray[Index]), false, DeltaTime * 2,1,1);
		}

	}
	else
	{
		//If no bones are set return
		if (LeftBoneNames.Num() <= 0) return;

		TArray<float> BoneForceArray;
		TArray<FVector> BoneLocation;
		TArray<FVector> ForceLocation;

		//If the constraints map wasn't created, create it
		if (!BoneToConstraintLeft.Num()) SetNewConstraints(CurrentHand, LeftBoneNames, bIsRightHand);

		//Go through all bones
		for (int Index = 0; Index < LeftBoneNames.Num(); Index++)
		{
			//Get the constraints instance and get the location of this bone
			FConstraintInstance* CurrentInstance = *BoneToConstraintLeft.Find(LeftBoneNames[Index]);
			FVector Location = SkelComp->GetBoneLocation(FName(*LeftBoneNames[Index]), EBoneSpaces::WorldSpace);

			FVector LinearForce;
			FVector AngularForce;

			//Get the current force vector and calculate the force with the helper function
			CurrentInstance->GetConstraintForce(LinearForce, AngularForce);
			float Force = GetForce(CurrentInstance->GetConstraintLocation(), AngularForce);

			//If the current force value is higher than the old one, overwrite it
			if (CurrentMaxForce < Force) CurrentMaxForce = Force;

			BoneForceArray.Add(Force);
			BoneLocation.Add(Location);
			ForceLocation.Add(Location + AngularForce);
		}

		//Go through all bones and draw the arrows
		for (int Index = 0; Index < LeftBoneNames.Num(); Index++)
		{
			if (!GetWorld()) return;

			//Draw the arrow and use the helper function to interpolate between two colors
			DrawDebugDirectionalArrow(GetWorld(), BoneLocation[Index], ForceLocation[Index], 5.0f,
				LinearColorInterpolate(FColor::Green, FColor::Red, CurrentMaxForce / BoneForceArray[Index]), false, DeltaTime * 2,1, 1);
		}
	}
}

void UForceBaseIndicatorArrow::SetGraspingData(FHandAnimationData Data, bool bIsRightHand)
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

void UForceBaseIndicatorArrow::SetNewConstraints(ASkeletalMeshActor * CurrentHand, TArray<FString> BoneNames, bool bIsRightHand)
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