// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "UGraspingController.h"


// Sets default values for this component's properties
UGraspingController::UGraspingController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGraspingController::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UGraspingController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGraspingController::UpdateGrasp(AGraspingHand* Hand, const float Input)
{
	if (Hand == nullptr)
	{
		return;
	}

	if (GraspingData.GetNumberOfEpisodes() <= 1)
	{
		return;
	}

	Spring = 9000 * (1 + Input);

	if (bEnableTightening) {
		UpdateGraspTighten(Hand, Input);
		return;
	}

	// Checks if input is relevant
	if (Input > 0.001)
	{
		if (!bIsGrasping) {
			bIsGrasping = true;
		}

		// Calculations for moving in multiple steps
		// StepSize defines how much of the 0-1 input is between each step
		// For 3 Steps for example there is a input of 0,5 seperating each 
		float StepSize = 1 / ((float)GraspingData.GetNumberOfEpisodes() - 1);
		// Calculates how many steps we have passed, given then current input
		// When rounded down we know which step came before with this input
		float StepIteratorCountFloat = Input / StepSize;

		// We will be rounding down and a input of exactly 1 causes problems there
		if (Input >= 1)
		{
			StepIteratorCountFloat = 0.999999 / StepSize;
		}

		// Uses the float to int cast to round down
		int StepIteratorCountInt = (int)StepIteratorCountFloat;

		// We calculate how far the input is past the step that came before it
		float NewInput = StepIteratorCountFloat - (float)StepIteratorCountInt;

		// Manipulate Orientation Drives
		FHandEpisodeData TargetOrientation;
		LerpHandOrientation(&TargetOrientation, GraspingData.GetPositionDataWithIndex(StepIteratorCountInt), GraspingData.GetPositionDataWithIndex(StepIteratorCountInt + 1), NewInput);
		DriveToHandOrientationTarget(&TargetOrientation, Hand);

	}
	else {
		if (bIsGrasping)
		{
			// Stop grasping
			StopGrasping(Hand);
		}
	}
}

void UGraspingController::StopGrasping(AGraspingHand* Hand)
{
	// Stop Grasp
	FHandEpisodeData Save = GraspingData.GetPositionDataWithIndex(0);
	DriveToHandOrientationTarget(&Save, Hand);
	if (bIsInQueue)
	{
		GraspingData = GraspQueue;
		bIsInQueue = false;
	}
	bCurrentlyTightening = false;
	bTighteningInit = false;
	bIsGrasping = false;
}

void UGraspingController::LerpHandOrientation(FHandEpisodeData* Target, FHandEpisodeData Initial, FHandEpisodeData Closed, const float Input)
{
	// We handle each finger separately, so we can reuse the function for other porpuses
	// Also filters out data about bones that don't belong to any finger, but still were saved for some reason
	LerpFingerOrientation(ETypeOfFinger::Thumb, Target, Initial, Closed, Input);
	LerpFingerOrientation(ETypeOfFinger::Index, Target, Initial, Closed, Input);
	LerpFingerOrientation(ETypeOfFinger::Middle, Target, Initial, Closed, Input);
	LerpFingerOrientation(ETypeOfFinger::Ring, Target, Initial, Closed, Input);
	LerpFingerOrientation(ETypeOfFinger::Pinky, Target, Initial, Closed, Input);
}

void UGraspingController::LerpFingerOrientation(ETypeOfFinger Type, FHandEpisodeData* Target, FHandEpisodeData Initial, FHandEpisodeData Closed, const float Input)
{
	// Look at "FMath::LerpRange()" if you want to see the math behing this
	FFingerData InitialState = *Initial.GetFingerData(Type);
	FFingerData ClosedState = *Closed.GetFingerData(Type);
	FFingerData NewFinger;
	TArray<FString> TempArray;
	InitialState.GetFingerMap()->GenerateKeyArray(TempArray);
	for (FString s : TempArray) {
		NewFinger.AddData(s, FMath::LerpRange(
			InitialState.GetFingerRotator(s)->BoneSpaceAfterCalc,
			ClosedState.GetFingerRotator(s)->BoneSpaceAfterCalc, Input));
	}
	Target->AddNewFingerData(Type, NewFinger);
}

void UGraspingController::DriveToHandOrientationTarget(FHandEpisodeData* Target, AGraspingHand* Hand) 
{
	// We handle each finger separately, so we can reuse the function for other porpuses
	// Also filters out data about bones that don't belong to any finger, but still were saved for some reason
	DriveToFingerOrientationTarget(ETypeOfFinger::Thumb, Target, Hand);
	DriveToFingerOrientationTarget(ETypeOfFinger::Index, Target, Hand);
	DriveToFingerOrientationTarget(ETypeOfFinger::Middle, Target, Hand);
	DriveToFingerOrientationTarget(ETypeOfFinger::Ring, Target, Hand);
	DriveToFingerOrientationTarget(ETypeOfFinger::Pinky, Target, Hand);
}

void UGraspingController::DriveToFingerOrientationTarget(ETypeOfFinger Type, FHandEpisodeData* Target, AGraspingHand* Hand) 
{
	FFingerData TargetState = *Target->GetFingerData(Type);
	FConstraintInstance* Constraint = nullptr;
	TArray<FString> TempArray;
	TargetState.GetFingerMap()->GenerateKeyArray(TempArray);
	for (FString s : TempArray) {
		Constraint = BoneNameToConstraint(s, Hand);
		if (Constraint) {
			Constraint->SetAngularOrientationTarget(TargetState.GetFingerMap()->Find(s)->BoneSpaceAfterCalc.Quaternion());
		}
	}
}

FConstraintInstance* UGraspingController::BoneNameToConstraint(FString BoneName, AGraspingHand* Hand) {
	FConstraintInstance* Constraint = nullptr;
	UActorComponent* component = Hand->GetComponentByClass(USkeletalMeshComponent::StaticClass());
	USkeletalMeshComponent* skeletalComponent = Cast<USkeletalMeshComponent>(component);

	//sets up the constraints so they can be moved 
	if (!bBonesConstraintsSetUp) {
		for (FConstraintInstance* NewConstraint : skeletalComponent->Constraints) {
			if (NewConstraint->ConstraintBone1.ToString() == BoneName) {
				NewConstraint->SetOrientationDriveTwistAndSwing(false, false);
				NewConstraint->SetAngularVelocityDriveTwistAndSwing(false, false);
				NewConstraint->SetAngularVelocityDriveSLERP(false);
				NewConstraint->SetOrientationDriveSLERP(true);
				NewConstraint->SetAngularDriveParams(Spring, 1000, 0);
			}
		}
		bBonesConstraintsSetUp = true;
	}

	//finds the constraint responsible for moving this bone
	for (FConstraintInstance* NewConstraint : skeletalComponent->Constraints) 
	{
		NewConstraint->SetAngularDriveParams(Spring, 1000, 0);
		if (NewConstraint->ConstraintBone1.ToString() == BoneName) {
			Constraint = NewConstraint;
		}
	}
	return Constraint;
}

void UGraspingController::SetGraspingData(FHandAnimationData Data) {
	// if player is grasping put new grasp in queue, else change grasp immediately
	if (bIsGrasping)
	{
		GraspQueue = Data;
		bIsInQueue = true;
		return;
	}
	GraspingData = Data;
}

void UGraspingController::CreateNewTightenTarget() {
	if (GraspingData.GetNumberOfEpisodes() > 0)
	{
		// create new grasp from scratch, based on the final position of the currently loaded grasp
		// the value that will be used to move the bones gets increased by a certain percentage
		FHandEpisodeData SaveR = GraspingData.GetPositionDataWithIndex(GraspingData.GetNumberOfEpisodes() - 1);
		FHandEpisodeData* newSaveR = new FHandEpisodeData();

		for (auto MapFinger : *SaveR.GetMap()) {
			FFingerData Finger;
			for (auto MapBone : *MapFinger.Value.GetFingerMap()) {
				FBoneData Bone = FBoneData(MapBone.Value.BoneSpaceAfterCalc*(1+(IncreasePercentage/100)), MapBone.Value.BoneSpaceBeforeCalc, MapBone.Value.ComponentSpace);
				Finger.AddData(MapBone.Key, Bone);
			}
			newSaveR->AddNewFingerData(MapFinger.Key, Finger);
		}

		TightenTarget = newSaveR;
	}
}

void UGraspingController::UpdateGraspTighten(AGraspingHand* Hand, const float Input)
{

	// Checks if input is relevant
	if (Input > 0.001)
	{
		if (bCurrentlyTightening) return;

		if (!bIsGrasping) {
			bIsGrasping = true;
		}

		// Calculations for moving in multiple steps
		// StepSize defines how much of the 0-1 input is between each step
		// For 3 Steps for example there is a input of 0,5 seperating each 
		float StepSize = 1 / ((float)GraspingData.GetNumberOfEpisodes() - 1);
		// Calculates how many steps we have passed, given then current input
		// When rounded down we know which step came before with this input
		float StepIteratorCountFloat = Input / StepSize;

		// We will be rounding down and a input of exactly 1 causes problems there
		if (Input >= 1)
		{
			StepIteratorCountFloat = 0.999999 / StepSize;
		}

		// Uses the float to int cast to round down
		int StepIteratorCountInt = (int)StepIteratorCountFloat;

		// We calculate how far the input is past the step that came before it
		float NewInput = StepIteratorCountFloat - (float)StepIteratorCountInt;

		// Manipulate Orientation Drives
		FHandEpisodeData TargetOrientation;
		LerpHandOrientation(&TargetOrientation, GraspingData.GetPositionDataWithIndex(StepIteratorCountInt), GraspingData.GetPositionDataWithIndex(StepIteratorCountInt + 1), NewInput);
		DriveToHandOrientationTarget(&TargetOrientation, Hand);

		// Tightening is only activated, if the input is high enough and is still high enough after the timer runs out
		// This is to ensure, that the bones have time to move into the correct position
		if (Input > 0.9) {
			if (!bTighteningInit)
			{
				HandToTighten = Hand;
				CreateNewTightenTarget();
				bTighteningInit = true;
				FTimerHandle TighteningTimer;
				GetWorld()->GetTimerManager().SetTimer(TighteningTimer, this, &UGraspingController::TightenGrip, TimeUntilIncrease, false);
			}
		}
		else {
			bCurrentlyTightening = false;
			bTighteningInit = false;
		}
	}
	else {
		if (bIsGrasping)
		{
			// Stop grasping
			StopGrasping(Hand);
		}
	}
}

void UGraspingController::TightenGrip()
{
	if (bTighteningInit)
	{
		bCurrentlyTightening = true;

		// Checks and efffects each finger separately
		if (CalculateIfBlocked(ETypeOfFinger::Thumb))
		{
			DriveToFingerOrientationTarget(ETypeOfFinger::Thumb, TightenTarget, HandToTighten);
		}
		if (CalculateIfBlocked(ETypeOfFinger::Index))
		{
			DriveToFingerOrientationTarget(ETypeOfFinger::Index, TightenTarget, HandToTighten);
		}
		if (CalculateIfBlocked(ETypeOfFinger::Middle))
		{
			DriveToFingerOrientationTarget(ETypeOfFinger::Middle, TightenTarget, HandToTighten);
		}
		if (CalculateIfBlocked(ETypeOfFinger::Ring))
		{
			DriveToFingerOrientationTarget(ETypeOfFinger::Ring, TightenTarget, HandToTighten);
		}
		if (CalculateIfBlocked(ETypeOfFinger::Pinky))
		{
			DriveToFingerOrientationTarget(ETypeOfFinger::Pinky, TightenTarget, HandToTighten);
		}
	}
}

bool UGraspingController::CalculateIfBlocked(ETypeOfFinger Type)
{
	USkeletalMeshComponent* SkeletalComponent = HandToTighten->GetSkeletalMeshComponent();

	// Checks each bone on the finger seperately and returns true if just one surpasses the threshold
	for (FConstraintInstance* Constraint : SkeletalComponent->Constraints)
	{
		if (TightenTarget->GetFingerData(Type)->GetFingerMap()->Contains(Constraint->ConstraintBone1.ToString()))
		{
			// This is a slightly altered version of "AAGraspingStyleManager::GetBoneDataForStep()"
			// We calculate how much the bones have been rotated relative to their start rotation
			// First we get the current rotation in component space
			FQuat QuatBone1 = SkeletalComponent->GetBoneQuaternion(Constraint->ConstraintBone1, EBoneSpaces::ComponentSpace);
			FQuat QuatBone2 = SkeletalComponent->GetBoneQuaternion(Constraint->ConstraintBone2, EBoneSpaces::ComponentSpace);

			// Second we get the start rotation in component space. We don't save it on runtime, but we can just check the GraspingData for the initial step of our grasp
			// We always have data about bone1 since those are the bones that get moved when changing the angular drive on a constraint
			FQuat QuatBone1Start = GraspingData.GetPositionDataWithIndex(0).GetFingerData(Type)->GetFingerMap()->Find(Constraint->ConstraintBone1.ToString())->ComponentSpace.Quaternion();
		
			// For bone2 we don't always have data since it might be a handbone that is not part of any finger and therefore is never a bone1 for any constraint
			// Since these bones are never moved by us we can assume that they are still in their starting rotation
			FQuat QuatBone2Start;
			if (GraspingData.GetPositionDataWithIndex(0).GetFingerData(Type)->GetFingerMap()->Contains(Constraint->ConstraintBone2.ToString()))
			{
				QuatBone2Start = GraspingData.GetPositionDataWithIndex(0).GetFingerData(Type)->GetFingerMap()->Find(Constraint->ConstraintBone2.ToString())->ComponentSpace.Quaternion();
			}
			else
			{
				QuatBone2Start = QuatBone2;
			}

			// Calculate the difference between start and current rotation of the individual bones
			FTransform Transform1Difference = FTransform(QuatBone1).GetRelativeTransform(FTransform(QuatBone1Start));
			FQuat Quat1Difference = Transform1Difference.GetRotation();
			FTransform Transform2Difference = FTransform(QuatBone2).GetRelativeTransform(FTransform(QuatBone2Start));
			FQuat Quat2Difference = Transform2Difference.GetRotation();

			// Substract the change in bone2's rotation, so movements of parent bones are filtered out of bone1's rotation 
			FQuat CurrentCalculatedRotationQuat = Quat1Difference * Quat2Difference.Inverse();

			// TargetRotation has the goal rotations. We then calculate how much our current change in rotation differs from it
			FRotator CurrentCalculatedRotation = CurrentCalculatedRotationQuat.Rotator();
			FRotator TargetRotation = GraspingData.GetPositionDataWithIndex(GraspingData.GetNumberOfEpisodes()-1).GetFingerData(Type)->GetFingerMap()->Find(Constraint->ConstraintBone1.ToString())->BoneSpaceAfterCalc;
			FRotator RelativeRotation = FTransform(CurrentCalculatedRotation).GetRelativeTransform(FTransform(TargetRotation)).GetRotation().Rotator();

			// Adds all the differences between current and goal change in rotation
			float AngularDifference = FGenericPlatformMath::Abs(RelativeRotation.Yaw) + FGenericPlatformMath::Abs(RelativeRotation.Pitch) + FGenericPlatformMath::Abs(RelativeRotation.Roll);
			if (AngularDifference >= BoneOrientationTolerance)
			{
				return true;
			}
		}
	}
	return false;
}