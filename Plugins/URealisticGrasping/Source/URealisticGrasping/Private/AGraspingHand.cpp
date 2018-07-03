// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "AGraspingHand.h"

// Sets default values
AGraspingHand::AGraspingHand()
{
// 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGraspingHand::BeginPlay()
{
	Super::BeginPlay();
	USkeletalMeshComponent* const SkelComp = GetSkeletalMeshComponent();
	SkelComp->SetSimulatePhysics(true);
	SkelComp->SetEnableGravity(false);

	float Spring = 9000.0f;
	float Damping = 1000.0f;
	float ForceLimit = 0.0f;
	if (SkelComp->GetPhysicsAsset())
	{
		// Hand joint velocity drive
		SkelComp->SetAllMotorsAngularPositionDrive(true, true);

		// Set drive parameters
		SkelComp->SetAllMotorsAngularDriveParams(Spring, Damping, ForceLimit);
	}
}

//// Called every frame
void AGraspingHand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
//
}

