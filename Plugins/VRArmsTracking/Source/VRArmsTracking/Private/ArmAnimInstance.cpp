// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#include "ArmAnimInstance.h"
#include "Runtime/AnimGraphRuntime/Public/BoneControllers/AnimNode_Fabrik.h"
#include "Runtime/AnimGraphRuntime/Public/BoneControllers/AnimNode_TwoBoneIK.h"
#include "Runtime/AnimGraphRuntime/Public/BoneControllers/AnimNode_ModifyBone.h"
#include "Runtime/Engine/Classes/Engine/Engine.h "
#include "HeadMountedDisplayFunctionLibrary.h"

void UArmAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	//Cache the owning pawn for use in Tick. Cast to own Pawn class
	OwningPawn = Cast<AArmAnimPawn>(TryGetPawnOwner());

	//Default Values for the head and hands (Preview only, overwritten on start)
	HeadWorldTransform = FTransform(FRotator(0.0f,0.0f,90.0f),FVector(0.0f,0.0f,160.0f), FVector(1.0f,1.0f,1.0f));
	LeftHandWorldLocation = FVector(-50.0f,-20.0f,100.0f);
	RightHandWorldLocation = FVector(50.0f, -20.0f, 100.0f);
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(0, EOrientPositionSelector::OrientationAndPosition);
}

void UArmAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	//Check it Pawn == null
	if (!OwningPawn)
	{
		return;
	}
	//Get current Positions from the Motioncontrollers and the HMD
	HeadWorldTransform = FTransform(OwningPawn->Camera->GetComponentRotation(), OwningPawn->Camera->GetComponentLocation(), FVector(1.0f,1.0f,1.0f));
	LeftHandWorldRotation = OwningPawn->MotionControllerLeft->GetComponentRotation();
	RightHandWorldRotation = OwningPawn->MotionControllerRight->GetComponentRotation();

	//Calculate the Effector Location and check for collisions
	RightHandWorldLocation = CalculatePosition(true, RightHandWorldRotation, OwningPawn->RightHandRotationOffset, OwningPawn->MotionControllerRight);
	LeftHandWorldLocation = CalculatePosition(false, LeftHandWorldRotation, OwningPawn->LeftHandRotationOffset, OwningPawn->MotionControllerLeft);

}


FVector UArmAnimInstance::CalculateOffset(bool bIsRightHand, AArmAnimPawn* CurrentPawn, FVector EffectorLocation)
{
	FName Lowerarm;
	FName SocketHandName;

	//Switch between hands
	if (bIsRightHand)
	{
		Lowerarm = FName("lowerarm_r");
		SocketHandName = FName("hand_rSocket");
	}
	else
	{
		Lowerarm = FName("lowerarm_l");
		SocketHandName = FName("hand_lSocket");
	}

	//Prepare the RayTrace
	const FName TraceTag("TraceTag");
	OwningPawn->GetWorld()->DebugDrawTraceTag = TraceTag;
	FCollisionQueryParams TraceParams(FName(TEXT("Trace")), true, OwningPawn);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.AddIgnoredActor(OwningPawn);
	TraceParams.TraceTag = TraceTag;

	//Startposition ellbow to the end Position (EffectorLocation)
	FVector StartTrace = CurrentPawn->Mesh->GetSocketLocation(Lowerarm);
	FVector EndTrace = EffectorLocation;

	FHitResult HitObject;

	//Raytrace as a Sphere
	OwningPawn->GetWorld()->SweepSingleByChannel(HitObject, StartTrace, EndTrace, CurrentPawn->Mesh->GetSocketQuaternion(Lowerarm), ECollisionChannel::ECC_Visibility,FCollisionShape::MakeSphere(10));

	//Check if we hit something
	if (!HitObject.IsValidBlockingHit())
	{
		return EffectorLocation;
	}

	//Calculate the distance to the Effectorlocation and to the HitObject (maybe not needed)
	float DistanceHandEff = (CurrentPawn->Mesh->GetSocketLocation(Lowerarm) - EffectorLocation).Size();
	float DistanceHandHit = (CurrentPawn->Mesh->GetSocketLocation(Lowerarm) - HitObject.Location).Size();

	//Compare the distance
	if (DistanceHandEff > DistanceHandHit)
	{
		return HitObject.Location;
	}

	return EffectorLocation;

}

FVector UArmAnimInstance::CalculatePosition(bool bIsRightHand, FRotator HandWorldRotation, FRotator HandRotationOffset, UMotionControllerComponent* CurrentMotionController)
{
	FName SocketName;
	FName HandName;
	AActor* CurrentHand;
	if (bIsRightHand)
	{
		SocketName = FName("hand_rSocket");
		HandName = FName("hand_r");
		CurrentHand = OwningPawn->RightHand;
	}
	else
	{
		SocketName = FName("hand_lSocket");
		HandName = FName("hand_l");
		CurrentHand = OwningPawn->LeftHand;
	}

	FVector SocketLocation = OwningPawn->Mesh->GetSocketLocation(SocketName);
	FVector HandLocation = OwningPawn->Mesh->GetSocketLocation(HandName);
	FRotator HandRotation = OwningPawn->Mesh->GetSocketRotation(HandName);
	if (CurrentHand != nullptr) {
		TArray<USkeletalMeshComponent*> Components;
		CurrentHand->GetComponents<USkeletalMeshComponent>(Components);
		FVector PositionOffset;
		if (Components.Num())
		{
			PositionOffset = CurrentHand->GetActorLocation() - Components[0]->GetSocketLocation(SocketName);
		}
		CurrentHand->SetActorLocation(SocketLocation + PositionOffset);

		if (bIsRightHand) 
		{
			CurrentHand->SetActorRotation(FRotator(HandWorldRotation.Roll, HandWorldRotation.Yaw, -HandWorldRotation.Pitch) + HandRotationOffset);
		} 
		else
		{
			CurrentHand->SetActorRotation(FRotator(-HandWorldRotation.Roll, HandWorldRotation.Yaw, HandWorldRotation.Pitch) + HandRotationOffset);
		}
	}
	FVector LocalAdjX = CurrentMotionController->GetForwardVector()*OwningPawn->LocalHandAdjustmentX;
	FVector MotionController = CurrentMotionController->GetComponentLocation() + LocalAdjX;
	return CalculateOffset(bIsRightHand, OwningPawn, MotionController);
}

