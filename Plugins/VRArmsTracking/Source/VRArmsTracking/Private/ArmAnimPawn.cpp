// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#include "ArmAnimPawn.h"
#include "HeadMountedDisplayFunctionLibrary.h"


// Sets default values
AArmAnimPawn::AArmAnimPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Default Root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//Create the camera root component
	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	CameraRoot->SetupAttachment(RootComponent);

	//Create default camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraRoot);
	Camera->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, UserHeight), FRotator(0.0f, 0.0f, 0.0f));

	//Create default physical animation component
	AnimationComponent = CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("Physical Animation Component"));

	//Create the mesh component 
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetRelativeRotation(MeshRotation);
	Mesh->SetRelativeLocation(FVector(0.0f,0.0f,0.0f));
	Mesh->SetSkeletalMesh(LoadObject<USkeletalMesh>(Mesh, *SkeletalPath));

	//Setup the physical animation for this mesh
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->bMultiBodyOverlap = true;
	Mesh->bGenerateOverlapEvents = 1;
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	//Create default motion controllers
	MotionControllerLeft = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerLeft"));
	MotionControllerRight = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerRight"));
	MotionControllerLeft->SetupAttachment(RootComponent);
	MotionControllerRight->SetupAttachment(RootComponent);
	MotionControllerLeft->Hand = EControllerHand::Left;
	MotionControllerRight->Hand = EControllerHand::Right;

	//Set default values
	RightArmBoneName = FName("upperarm_r");
	LeftArmBoneName = FName("upperarm_l");
	BlendWeight = 1.0f;

	//player one will use this
	AutoPossessPlayer = EAutoReceiveInput::Player0;

}

// Called when the game starts or when spawned
void AArmAnimPawn::BeginPlay()
{
	Super::BeginPlay();

	//Setup the physical animation and crate animation data with magical numbers
	AnimationComponent->SetSkeletalMeshComponent(Mesh);

	HMDName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName().ToString();

	//adjust relative position of camera and mesh to fit HMD
	if (HMDName == "OculusHMD") {
		Camera->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
		Mesh->SetRelativeLocation(FVector(ShouldersLocalX, 0, -UserHeight));
		SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f, UserHeight));
	}
	else if (HMDName == "SteamVR") {
		Camera->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
		Mesh->SetRelativeLocation(FVector(ShouldersLocalX, 0, -UserHeight));
		SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f, UserHeight));
	}

}

// Called every frame
void AArmAnimPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Mesh follows HMD if player position etc have been reset have been reset
	if (bHeadReset)
	{
		Mesh->SetWorldLocation(Camera->GetComponentLocation() + FVector(0, 0, -160));

		//if mesh is currently turning stop here
		if (bIsTurning)
		{
			return;
		}

		//calculate how far the head and hands have moved
		float ResultHead = HeadYawStart.Yaw - Camera->GetComponentRotation().Yaw;
		float ResultLHand = ControllerLYawStart.Yaw - MotionControllerLeft->GetComponentRotation().Yaw;
		float ResultRHand = ControllerRYawStart.Yaw - MotionControllerRight->GetComponentRotation().Yaw;

		//if both head and hands have moved more than the threshold start turning in that direction
		//divides the amount to turn into multiple small steps and applys these steps in fixed intervals using a timer 
		if (ResultHead < -TurnThreshold &&
			ResultLHand < -TurnThreshold &&
			ResultRHand < -TurnThreshold)
		{
			if (ResultHead < -180)
			{
				RotationStep = FRotator(0, -(TurnThreshold / MaxTurnSteps), 0);
			}
			else
			{
				RotationStep = FRotator(0, TurnThreshold / MaxTurnSteps, 0);
			}

			TurnStepCounter = 0;
			bIsTurning = true;
			GetOwner()->GetWorldTimerManager().SetTimer(TurnTimer, this, &AArmAnimPawn::TurnInSteps, 0.01, true);
		}
		else if (ResultHead > TurnThreshold &&
			ResultLHand > TurnThreshold &&
			ResultRHand > TurnThreshold)
		{
			if (ResultHead > 180)
			{
				RotationStep = FRotator(0, TurnThreshold / MaxTurnSteps, 0);
			}
			else
			{
				RotationStep = FRotator(0, -(TurnThreshold / MaxTurnSteps), 0);
			}

			TurnStepCounter = 0;
			bIsTurning = true;
			GetOwner()->GetWorldTimerManager().SetTimer(TurnTimer, this, &AArmAnimPawn::TurnInSteps, 0.01, true);
		}
	}

}

// Called to bind functionality to input
void AArmAnimPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("resetPosition", IE_Pressed, this, &AArmAnimPawn::ResetHeadPosition);
}


//Reset HMD position and make mesh follow HMD
void AArmAnimPawn::ResetHeadPosition()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(0, EOrientPositionSelector::OrientationAndPosition);
	FTimerHandle PositionResetTimer;
	GetWorldTimerManager().SetTimer(PositionResetTimer, this, &AArmAnimPawn::SetMovementValues, 0.5, false);
}

//saves current state of players head and hands so differences can be calculated later
void AArmAnimPawn::SetMovementValues()
{
	MeshStartLocation = Mesh->GetComponentLocation();
	HeadStartLocation = Camera->GetComponentLocation();
	HeadYawStart = Camera->GetComponentRotation();
	ControllerRYawStart = MotionControllerRight->GetComponentRotation();
	ControllerLYawStart = MotionControllerLeft->GetComponentRotation();
	Mesh->SetRelativeRotation(MeshRotation);
	bHeadReset = true;
}

void AArmAnimPawn::TurnInSteps()
{
	//if there are still steps to turn do so
	if (TurnStepCounter < MaxTurnSteps)
	{
		FVector MeshForwardBefore = Mesh->GetForwardVector();
		MeshStartLocation = MeshStartLocation - (MeshForwardBefore *ShouldersLocalX);
		Mesh->SetWorldRotation((Mesh->GetComponentRotation()+RotationStep));
		HeadYawStart += RotationStep;
		ControllerLYawStart += RotationStep;
		ControllerRYawStart += RotationStep;
		FVector MeshForwardAfter = Mesh->GetForwardVector();
		MeshStartLocation = MeshStartLocation + (MeshForwardAfter *ShouldersLocalX);
		TurnStepCounter++;
	}
	//else tell the timer to stop turning
	else 
	{
		GetWorldTimerManager().SetTimer(TurnTimer, this, &AArmAnimPawn::TurnInSteps, -1, false);
		bIsTurning = false;
	}
}