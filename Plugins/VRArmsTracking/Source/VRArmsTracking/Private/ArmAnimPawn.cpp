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
	Mesh->SetRelativeRotation(FRotator(0.0f,0.0f,0.0f));
	Mesh->SetRelativeLocation(FVector(0.0f,0.0f,0.0f));
	FString Path = "SkeletalMesh'/VRArmsTracking/ArmMesh/SK_ArmsMesh.SK_ArmsMesh'";
	Mesh->SetSkeletalMesh(LoadObject<USkeletalMesh>(Mesh, *Path));

	//Setup the physical animation for this mesh
	//Mesh->SetAllBodiesSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->bGenerateOverlapEvents = 1;
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	//static ConstructorHelpers::FObjectFinder<UAnimBlueprintGeneratedClass> MeshAnimClass(TEXT("AnimBlueprint'/VRArmsTracking/ABP_Arms.ABP_Arms'"));//AnimBlueprint'/VRArmsTracking/ABP_Arms.ABP_Arms'
	//Mesh->SetAnimInstanceClass(MeshAnimClass.Object);

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
	ConstraintsProfileName = FName("ArmsMovement");
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
	FPhysicalAnimationData AnimationData = FPhysicalAnimationData();
	AnimationData.bIsLocalSimulation = true;
	AnimationData.AngularVelocityStrength = 100;
	AnimationData.OrientationStrength = 1000;
	AnimationData.PositionStrength = 1000;
	AnimationData.VelocityStrength = 100;

	//Applay this to all bowns below the left and the right hand bone name variabels
	AnimationComponent->ApplyPhysicalAnimationSettingsBelow(LeftArmBoneName,AnimationData,false);
	AnimationComponent->ApplyPhysicalAnimationSettingsBelow(RightArmBoneName, AnimationData, false);
	Mesh->SetAllBodiesBelowSimulatePhysics(LeftArmBoneName,true,false);
	Mesh->SetAllBodiesBelowSimulatePhysics(RightArmBoneName, true, false);
	Mesh->SetAllBodiesBelowPhysicsBlendWeight(LeftArmBoneName, BlendWeight,false,false);
	Mesh->SetAllBodiesBelowPhysicsBlendWeight(RightArmBoneName, BlendWeight, false, false);
	//Set the constraints profile
	Mesh->SetConstraintProfileForAll(ConstraintsProfileName,true);
	HMDName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName().ToString();
	//adjust relative position of camera and mesh to fit HMD
	if (HMDName == "OculusHMD") {
		Camera->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
		Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, -UserHeight));
		SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f, UserHeight));
	}
	else if (HMDName == "SteamVR") {
		//should be default values
	}
}

// Called every frame
void AArmAnimPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//mesh will follow the HMD
	//Mesh->SetWorldLocation(FVector(Camera->GetComponentLocation().X, Camera->GetComponentLocation().Y, 0.0f), true);

}

// Called to bind functionality to input
void AArmAnimPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

