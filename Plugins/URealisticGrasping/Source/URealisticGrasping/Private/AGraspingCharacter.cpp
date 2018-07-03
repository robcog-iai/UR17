// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "AGraspingCharacter.h"
#include "UReadWrite.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "HeadMountedDisplayFunctionLibrary.h"


// Sets default values
AGraspingCharacter::AGraspingCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create the camera root component
	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	CameraRoot->SetupAttachment(this->GetRootComponent());

	//Create default camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraRoot);

	//Create default motion controllers
	MotionControllerLeft = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerLeft"));
	MotionControllerRight = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerRight"));
	MotionControllerLeft->SetupAttachment(this->GetRootComponent());
	MotionControllerRight->SetupAttachment(this->GetRootComponent());
	MotionControllerLeft->Hand = EControllerHand::Left;
	MotionControllerRight->Hand = EControllerHand::Right;

	GraspingController = CreateDefaultSubobject<UGraspingController>(TEXT("GraspingController"));

	AnimationController = CreateDefaultSubobject<UGraspingAnimationController>(TEXT("AnimationController"));


	//player one will use this
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Init TextComponent
	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextComponent"));
	TextComponent->SetupAttachment(Camera);
	TextComponent->SetRelativeRotation(FRotator(0, -180, 0));
	TextComponent->SetRelativeLocation(FVector(20, 0, 7));
	TextComponent->SetText(FText::FromString("Press A or B to choose a Grasp"));
	TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextComponent->SetTextRenderColor(FColor::Black);
	TextComponent->SetXScale(0.1f);
	TextComponent->SetYScale(0.1f);
	TextComponent->SetWorldSize(10.0f);

}

// Called when the game starts or when spawned
void AGraspingCharacter::BeginPlay()
{
	Super::BeginPlay();
	MotionControllerLeft->Activate(true);
	MotionControllerRight->Activate(true);

	LeftPIDController = FPIDController3D(PGain, IGain, DGain, MaxOutput);
	RightPIDController = FPIDController3D(PGain, IGain, DGain, MaxOutput);
	AnimationController->SetMeshName(RightHand->GetSkeletalMeshComponent()->SkeletalMesh->GetName(),true);
	AnimationController->SetMeshName(LeftHand->GetSkeletalMeshComponent()->SkeletalMesh->GetName(), false);

	GrapsTypesRight = AnimationController->GetAnimationNames(true);
	//GrapsTypsLeft = AnimationController->GetAnimationNames(false);

	IndexGraspType = -1;
	
	EventSetup();
}

// Called every frame
void AGraspingCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LeftHand)
	{
		UpdateHandLocationAndRotation(MotionControllerLeft, LeftPIDController, LeftHand->GetSkeletalMeshComponent(), OffsetL, DeltaTime);
	}

	if (RightHand)
	{
		UpdateHandLocationAndRotation(MotionControllerRight, RightPIDController, RightHand->GetSkeletalMeshComponent(), OffsetR, DeltaTime);
	}

}

// Called to bind functionality to input
void AGraspingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("resetPosition", IE_Pressed, this, &AGraspingCharacter::ResetHeadPosition);
	PlayerInputComponent->BindAction("NextGraspRight", IE_Pressed, this, &AGraspingCharacter::NextGraspRightHand);
	PlayerInputComponent->BindAction("NextGraspLeft", IE_Pressed, this, &AGraspingCharacter::NextGraspLeftHand);

	// Hand control binding
	PlayerInputComponent->BindAxis("NewGraspWithLeftHand", this, &AGraspingCharacter::GraspWithLeftHand);
	PlayerInputComponent->BindAxis("NewGraspWithRightHand", this, &AGraspingCharacter::GraspWithRightHand);
	// Grasp-Switch Bottens 
	PlayerInputComponent->BindAction("SwitchNextGrasp", IE_Pressed, this, &AGraspingCharacter::SwitchToNextGraspType);
	PlayerInputComponent->BindAction("SwitchPreviousGrasp", IE_Pressed, this, &AGraspingCharacter::SwitchToPreviousGraspType);
}

//Reset HMD position and make mesh follow HMD
void AGraspingCharacter::ResetHeadPosition() {
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(0, EOrientPositionSelector::OrientationAndPosition);
}

void AGraspingCharacter::GraspWithLeftHand(const float Input){
	GraspingController->UpdateGrasp(LeftHand, Input, false);
}

void AGraspingCharacter::GraspWithRightHand(const float Input) {
	GraspingController->UpdateGrasp(RightHand, Input, true);
}

void AGraspingCharacter::NextGraspRightHand()
{
	FHandAnimationData Animation = AnimationController->GetNextAnimation(true);
}

void AGraspingCharacter::NextGraspLeftHand()
{
	FHandAnimationData Animation = AnimationController->GetNextAnimation(true);
}

void AGraspingCharacter::UpdateHandLocationAndRotation(UMotionControllerComponent * MC, FPIDController3D& PIDController, USkeletalMeshComponent * SkelMesh, FRotator Offset, const float DeltaTime)
{
	//// Location
	const FVector Error = MC->GetComponentLocation() - SkelMesh->GetComponentLocation();
	const FVector LocOutput = PIDController.UpdateAsPD(Error, DeltaTime);
	SkelMesh->AddForceToAllBodiesBelow(LocOutput, NAME_None, true, true);
	
	const FQuat TargetQuat = MC->GetComponentRotation().Quaternion();
	FQuat CurrQuat = SkelMesh->GetComponentQuat() * Offset.Quaternion();

	// Dot product to get cos theta
	const float CosTheta = TargetQuat | CurrQuat;
	// Avoid taking the long path around the sphere
	if (CosTheta < 0)
	{
		CurrQuat *= -1.f;
	}
	// Use the xyz part of the quat as the rotation velocity
	const FQuat OutputFromQuat = TargetQuat * CurrQuat.Inverse();
	const FVector RotOutput = FVector(OutputFromQuat.X, OutputFromQuat.Y, OutputFromQuat.Z) * RotationBoost;
	SkelMesh->SetAllPhysicsAngularVelocityInDegrees(RotOutput);
}

//Pressing the A or B button on the Motion Controller will move to the next or previous grasp type
// Switch to the next grasping type
void AGraspingCharacter::SwitchToNextGraspType()
{

	if (GrapsTypesRight.Num() == 0) {
		return;
	}

	if (IndexGraspType == -1)
	{
		IndexGraspType = 0;
	} 
	else if (IndexGraspType >= GrapsTypesRight.Num()-1)
	{
		IndexGraspType = 0;
	}
	else
	{
		IndexGraspType++;
	}

		
	Grasp = GrapsTypesRight[IndexGraspType];
	
	AnimationController->SetNewAnimation(Grasp, true);

	TextComponent->SetText(FText::FromString(Grasp));


}

// Switch to the previous grasping type
void AGraspingCharacter::SwitchToPreviousGraspType()
{
	if (GrapsTypesRight.Num() == 0) {
		return;
	}

	if (IndexGraspType == -1)
	{
		IndexGraspType = 0;
	}
	else if (IndexGraspType == 0)
	{
		IndexGraspType = GrapsTypesRight.Num() -1;
	}
	else
	{
		IndexGraspType--;
	}
	
	Grasp = GrapsTypesRight[IndexGraspType];
	
	AnimationController->SetNewAnimation(Grasp, true);

	TextComponent->SetText(FText::FromString(Grasp));

}

void AGraspingCharacter::EventSetup()
{
	AnimationController->OnNextAnimation.AddDynamic(GraspingController, &UGraspingController::SetGraspingData);
}