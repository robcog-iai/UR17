// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "UGraspingController.h"
#include "UGraspingAnimationController.h"
#include "UReadWrite.h"
#include "Runtime/Engine/Classes/PhysicsEngine/ConstraintInstance.h"


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

void UGraspingController::UpdateGrasp(AGraspingHand* Hand, const float Input, bool bIsRightHand)
{
	Spring = 9000 * (1 + Input);

	//checks which hand to move
	if (bIsRightHand && GraspingDataR.GetNumberOfEpisodes() > 0) {
		if (Input > 0.0001)
		{
			bIsGraspingR = true;

			// Manipulate Orientation Drives
			FFingersData TargetOrientation;
			LerpHandOrientation(&TargetOrientation, GraspingDataR.GetPositionDataWithIndex(0), GraspingDataR.GetPositionDataWithIndex(4), Input);
			DriveToHandOrientationTarget(&TargetOrientation, Hand);
		}
		else {
			if (bIsGraspingR)
			{
				// Stop Grasp
				FFingersData Save = GraspingDataR.GetPositionDataWithIndex(0);
				DriveToHandOrientationTarget(&Save, Hand);
				bIsGraspingR = false;
			}
		}
	}
	else if(GraspingDataL.GetNumberOfEpisodes() > 0){
		if (Input > 0.0001)
		{
			bIsGraspingL = true;

			// Manipulate Orientation Drives
			FFingersData TargetOrientation;
			LerpHandOrientation(&TargetOrientation, GraspingDataL.GetPositionDataWithIndex(0), GraspingDataL.GetPositionDataWithIndex(GraspingDataL.GetNumberOfEpisodes()-1), Input);
			DriveToHandOrientationTarget(&TargetOrientation, Hand);
		}
		else {
			if (bIsGraspingL)
			{
				// Stop Grasp
				FFingersData Save = GraspingDataR.GetPositionDataWithIndex(0);
				DriveToHandOrientationTarget(&Save, Hand);
				bIsGraspingL = false;
			}
		}
	}
}

void UGraspingController::LerpHandOrientation(FFingersData* Target, FFingersData Initial, FFingersData Closed, const float Input)
{
	LerpFingerOrientation(ETypeOfFinger::Thumb, Target, Initial, Closed, Input);
	LerpFingerOrientation(ETypeOfFinger::Index, Target, Initial, Closed, Input);
	LerpFingerOrientation(ETypeOfFinger::Middle, Target, Initial, Closed, Input);
	LerpFingerOrientation(ETypeOfFinger::Ring, Target, Initial, Closed, Input);
	LerpFingerOrientation(ETypeOfFinger::Pinky, Target, Initial, Closed, Input);
}

void UGraspingController::LerpFingerOrientation(ETypeOfFinger Type, FFingersData* Target, FFingersData Initial, FFingersData Closed, const float Input)
{
	FFingerData InitialState = *Initial.GetFingerData(Type);
	FFingerData ClosedState = *Closed.GetFingerData(Type);
	FFingerData NewFinger;
	TArray<FString> TempArray;
	InitialState.GetFingerMap()->GenerateKeyArray(TempArray);
	for (FString s : TempArray) {
		NewFinger.AddData(s, FMath::LerpRange(
			*InitialState.GetFingerRotator(s),
			*ClosedState.GetFingerRotator(s), Input));
	}
	Target->AddNewFingerData(Type, NewFinger);
}

void UGraspingController::DriveToHandOrientationTarget(FFingersData* Target, AGraspingHand* Hand) 
{
	DriveToFingerOrientationTarget(ETypeOfFinger::Thumb, Target, Hand);
	DriveToFingerOrientationTarget(ETypeOfFinger::Index, Target, Hand);
	DriveToFingerOrientationTarget(ETypeOfFinger::Middle, Target, Hand);
	DriveToFingerOrientationTarget(ETypeOfFinger::Ring, Target, Hand);
	DriveToFingerOrientationTarget(ETypeOfFinger::Pinky, Target, Hand);
}

void UGraspingController::DriveToFingerOrientationTarget(ETypeOfFinger Type, FFingersData* Target, AGraspingHand* Hand) 
{
	FFingerData TargetState = *Target->GetFingerData(Type);
	FConstraintInstance* Constraint = nullptr;
	TArray<FString> TempArray;
	TargetState.GetFingerMap()->GenerateKeyArray(TempArray);
	for (FString s : TempArray) {
		Constraint = BoneNameToConstraint(s, Hand);
		if (Constraint) {
			Constraint->SetAngularOrientationTarget(TargetState.GetFingerMap()->Find(s)->Quaternion());
		}
	}
}

FConstraintInstance* UGraspingController::BoneNameToConstraint(FString BoneName, AGraspingHand* Hand) {
	FConstraintInstance* Constraint = nullptr;
	UActorComponent* component = Hand->GetComponentByClass(USkeletalMeshComponent::StaticClass());
	USkeletalMeshComponent* skeletalComponent = Cast<USkeletalMeshComponent>(component);

	//sets up the constraints so they can be moved 
	if (!BonesConstraintsSetUp) {
		for (FConstraintInstance* NewConstraint : skeletalComponent->Constraints) {
			if (NewConstraint->ConstraintBone1.ToString() == BoneName) {
				NewConstraint->SetOrientationDriveTwistAndSwing(false, false);
				NewConstraint->SetAngularVelocityDriveTwistAndSwing(false, false);
				NewConstraint->SetAngularVelocityDriveSLERP(false);
				NewConstraint->SetOrientationDriveSLERP(true);
				NewConstraint->SetAngularDriveParams(9000, 1000, 0);
			}
		}
		BonesConstraintsSetUp = true;
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

void UGraspingController::SetGraspingData(FHandAnimationData Data, bool bIsRightHand) {
	if (bIsRightHand) 
	{
		GraspingDataR = Data;
	}
	else {
		GraspingDataL = Data;
	}
}