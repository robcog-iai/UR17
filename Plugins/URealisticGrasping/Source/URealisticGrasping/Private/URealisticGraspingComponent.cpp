// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "URealisticGraspingComponent.h"
#include "Engine/Engine.h"
#include "HeadMountedDisplayFunctionLibrary.h"



// Sets default values for this component's properties
URealisticGraspingComponent::URealisticGraspingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}


// Called when the game starts
void URealisticGraspingComponent::BeginPlay()
{
	Super::BeginPlay();

	//Checks if we get the input component from the character
	UInputComponent* Input = GetOwner()->InputComponent;
	if (!Input)
	{
		UE_LOG(LogTemp, Error, TEXT("No Input Component! Check the auto process player option!"));
		return;
	}

	Input->BindAction("resetPosition", IE_Pressed, this, &URealisticGraspingComponent::ResetHeadPosition);

	// Hand control binding
	Input->BindAxis("GraspWithLeftHand", this, &URealisticGraspingComponent::GraspWithLeftHand);
	Input->BindAxis("GraspWithRightHand", this, &URealisticGraspingComponent::GraspWithRightHand);
	// Grasp-Switch Bottens 
	Input->BindAction("SwitchNextGraspRight", IE_Pressed, this, &URealisticGraspingComponent::ButtonNextRight);
	Input->BindAction("SwitchPreviousGraspRight", IE_Pressed, this, &URealisticGraspingComponent::ButtonPreviousRight);
	Input->BindAction("SwitchNextGraspLeft", IE_Pressed, this, &URealisticGraspingComponent::ButtonNextLeft);
	Input->BindAction("SwitchPreviousGraspLeft", IE_Pressed, this, &URealisticGraspingComponent::ButtonPreviousLeft);

	//Activate the motion controllers
	MotionControllerLeft->Activate(true);
	MotionControllerRight->Activate(true);

	//Create the PID Controllers for the Left and Right hand
	LeftPIDController = FPIDController3D(PGain, IGain, DGain, MaxOutput);
	RightPIDController = FPIDController3D(PGain, IGain, DGain, MaxOutput);

	

	//Set the right hand Mesh name in the animation controller and reset the position
	if (RightHand)
	{
		AnimationController->SetMeshName(RightHand->GetSkeletalMeshComponent()->SkeletalMesh->GetName(), true);
		RightHand->SetActorLocation(MotionControllerRight->GetComponentLocation(), false, nullptr, ETeleportType::ResetPhysics);
	}

	//Set the left hand Mesh name in the animation controller and reset the position
	if (LeftHand)
	{
		AnimationController->SetMeshName(LeftHand->GetSkeletalMeshComponent()->SkeletalMesh->GetName(), false);
		LeftHand->SetActorLocation(MotionControllerLeft->GetComponentLocation(),false,nullptr,ETeleportType::ResetPhysics);
	}

	GrapsTypesRight = AnimationController->GetAnimationNames(true);
	//GrapsTypsLeft = AnimationController->GetAnimationNames(false);

	//Setup the bindings
	EventSetup();

	//The right GraspingMenu is being created
	MenuRight = CreateWidget<UGraspingMenu>(GetWorld(), UGraspingMenu::StaticClass(), TEXT("MenuRight"));
	
	//All grip names for the right hand will be stored in this array
	GraspArrayRight = AnimationController->GetAnimationNames(true);
	
	//Has to be executed in this order
	//Check if there are grasps in the array
	if (GraspArrayRight.Num() > 0) {
		MenuRight->SetArray(GraspArrayRight);
		MenuRight->CreateMenu();
	}

	//Is set to minus one because a grasp should only be selected at a keystroke
	IndexGraspRight = -1;

	//The left GraspingMenu is being created
	MenuLeft = CreateWidget<UGraspingMenu>(GetWorld(), UGraspingMenu::StaticClass(), TEXT("MenuLeft"));
	
	//All grip names for the left hand will be stored in this array
	GraspArrayLeft = AnimationController->GetAnimationNames(false);

	//Has to be executed in this order
	//Check if there are grasps in the array
	if (GraspArrayLeft.Num() > 0) {
		MenuLeft->SetArray(GraspArrayLeft);
		MenuLeft->CreateMenu();
	}
	
	//Is set to minus one because a grasp should only be selected at a keystroke
	IndexGraspLeft = -1;
}


void URealisticGraspingComponent::InitializeComponent()
{
	Super::InitializeComponent();

	//Get all Actor components and iterate through it
	TSet<UActorComponent*> ActorComponents = GetOwner()->GetComponents();
	for (UActorComponent* Component : ActorComponents) {

		//If we found a camera component then set it
		if (UCameraComponent* CameraComponent = Cast<UCameraComponent>(Component))
		{
			Camera = CameraComponent;
		}

		//If we found a motion controller component then set it
		else if (UMotionControllerComponent* MotionControllerComponent = Cast<UMotionControllerComponent>(Component))
		{
			//Looking for the left and right controller
			if (MotionControllerComponent->MotionSource == FName("Left"))
			{
				MotionControllerLeft = MotionControllerComponent;
			}
			else if (MotionControllerComponent->MotionSource == FName("Right"))
			{
				MotionControllerRight = MotionControllerComponent;
			}
		}
	}

	//IF we didn't found a left motion controller create one and add it to the character
	if (!MotionControllerLeft)
	{
		MotionControllerLeft = NewObject<UMotionControllerComponent>(GetOwner(), UMotionControllerComponent::StaticClass(), TEXT("MotionControllerLeft"));

		//If the motion controller was created successfully add it
		if (MotionControllerLeft)
		{
			MotionControllerLeft->RegisterComponent();
			MotionControllerLeft->AttachToComponent(GetOwner()->GetRootComponent(), 
				FAttachmentTransformRules::KeepRelativeTransform);

			MotionControllerLeft->MotionSource = FName("Left");
		}
	}

	//If we didn't found a left motion controller create one and add it to the character
	if (!MotionControllerRight)
	{
		MotionControllerRight = NewObject<UMotionControllerComponent>(GetOwner(), 
			UMotionControllerComponent::StaticClass(), TEXT("MotionControllerRight"));

		//If the motion controller was created successfully add it
		if (MotionControllerRight)
		{
			MotionControllerRight->RegisterComponent();
			MotionControllerRight->AttachToComponent(GetOwner()->GetRootComponent(), 
				FAttachmentTransformRules::KeepRelativeTransform);

			MotionControllerRight->MotionSource = FName("Right");
		}
	}

	//If we didn't found a Camera create one and add it to the character
	if (!Camera)
	{
		Camera = NewObject<UCameraComponent>(GetOwner(), UCameraComponent::StaticClass(), TEXT("Camera"));
		//If the Camera was created successfully add it
		if (Camera)
		{
			Camera->SetupAttachment(GetOwner()->GetRootComponent());
			Camera->SetRelativeLocation(FVector(0, 0, 170));
			Camera->RegisterComponent();
		}
	}

	//Create our grasping controllers for the left and right hand and register it
	GraspingControllerR = NewObject<UGraspingController>(GetOwner(), UGraspingController::StaticClass(), TEXT("GraspingControllerR"));
	GraspingControllerL = NewObject<UGraspingController>(GetOwner(), UGraspingController::StaticClass(), TEXT("GraspingControllerL"));
	GraspingControllerR->RegisterComponent();
	GraspingControllerL->RegisterComponent();
	GraspingControllerR->bEnableTightening = bEnableTightening;
	GraspingControllerL->bEnableTightening = bEnableTightening;

	//Create our animation controller and register it
	AnimationController = NewObject<UGraspingAnimationController>(GetOwner(), UGraspingAnimationController::StaticClass(), TEXT("AnimationController"));
	AnimationController->RegisterComponent();

	//Checks if we want to debug the force. You have to set the boolean an select a debug class in the editor (see documentation)
	if (bUseForceIndicator && ForceIndicatorInterface.Get())
	{
		//Create the debug class and check if this class implements our interface
		UObject* DebugClass = NewObject<UObject>(this, ForceIndicatorInterface, TEXT("Force Indicator"));
		bool bIsImplemented = DebugClass->GetClass()->ImplementsInterface(UForceBaseIndicatorInterface::StaticClass());

		//Set the debug class if this class implements our interface
		if (bIsImplemented)
		{
			ForceIndicator = Cast<IForceBaseIndicatorInterface>(DebugClass);
		}
		else
		{
			bUseForceIndicator = false;
		}
	}

	//WidgetComponent in which to set the menu 
	WidgetMenuComponent = NewObject<UWidgetComponent>(GetOwner(), UWidgetComponent::StaticClass(), TEXT("Widget"));
	CreateMenuComponent(&WidgetMenuComponent);
}

// Called every frame
void URealisticGraspingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (LeftHand)
	{
		//Update the PID Controller position and rotation
		UpdateHandLocationAndRotation(MotionControllerLeft, LeftPIDController, LeftHand->GetSkeletalMeshComponent(), OffsetL, DeltaTime);

		//Checks if you want to debug the force and if everything is correctly set up
		if (bUseForceIndicator && ForceIndicator)
		{
			//Send a notification to the force indicator class that he should update the visualisation
			ForceIndicator->CreateNewHandDebug(LeftHand, false, DeltaTime);
		}
	}

	if (RightHand)
	{
		//Update the PID Controller position and rotation
		UpdateHandLocationAndRotation(MotionControllerRight, RightPIDController, RightHand->GetSkeletalMeshComponent(), OffsetR, DeltaTime);

		//Checks if you want to debug the force and if everything is correctly set up
		if (bUseForceIndicator && ForceIndicator)
		{
			//Send a notification to the force indicator class that he should update the visualisation
			ForceIndicator->CreateNewHandDebug(RightHand, true, DeltaTime);
		}
	}

	// ...
}

void URealisticGraspingComponent::ResetHeadPosition()
{
	//Resets the position for the right hand
	if (RightHand)
	{
		RightHand->SetActorLocation(MotionControllerRight->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	}

	//Resets the position for the left hand
	if (LeftHand)
	{
		LeftHand->SetActorLocation(MotionControllerLeft->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	}
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(0, EOrientPositionSelector::OrientationAndPosition);
}

void URealisticGraspingComponent::GraspWithLeftHand(const float Input)
{
	GraspingControllerL->UpdateGrasp(LeftHand, Input);
}

void URealisticGraspingComponent::GraspWithRightHand(const float Input)
{
	GraspingControllerR->UpdateGrasp(RightHand, Input);
}

void URealisticGraspingComponent::UpdateHandLocationAndRotation(UMotionControllerComponent * MC, FPIDController3D & PIDController, USkeletalMeshComponent * SkelMesh, FRotator Offset, const float DeltaTime)
{
	// Location
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

void URealisticGraspingComponent::EventSetup()
{
	//Create the bindings for changing the animation
	AnimationController->OnNextAnimationR.AddDynamic(GraspingControllerR, &UGraspingController::SetGraspingData);
	AnimationController->OnNextAnimationL.AddDynamic(GraspingControllerL, &UGraspingController::SetGraspingData);

	//Checks if you want to debug the force and if everything is correctly set up
	if (bUseForceIndicator && ForceIndicator) 
	{
		AnimationController->OnNextAnimationR.AddDynamic(this, &URealisticGraspingComponent::SetGraspingDataRight);
		AnimationController->OnNextAnimationL.AddDynamic(this, &URealisticGraspingComponent::SetGraspingDataLeft);
	}
}

void URealisticGraspingComponent::SetGraspingDataRight(FHandAnimationData Data)
{
	ForceIndicator->SetGraspingData(Data, true);
}

void URealisticGraspingComponent::SetGraspingDataLeft(FHandAnimationData Data)
{
	ForceIndicator->SetGraspingData(Data, false);
}

void URealisticGraspingComponent::ButtonNextRight()
{
     //Check if there are grasps in the array
	if (GraspArrayRight.Num() == 0) 
	{
		return;
	}
	ActionFunctionsGrasp(&IndexGraspRight, GraspArrayRight, true, true);
	ActionFunctionsMenu(WidgetMenuComponent, MenuRight, IndexGraspRight);
}

void URealisticGraspingComponent::ButtonPreviousRight()
{
     //Check if there are grasps in the array
	if (GraspArrayRight.Num() == 0) 
	{
		return;
	}
	
	ActionFunctionsGrasp(&IndexGraspRight, GraspArrayRight, true, false);
	ActionFunctionsMenu(WidgetMenuComponent, MenuRight, IndexGraspRight);
}


void URealisticGraspingComponent::ButtonNextLeft()
{
     //Check if there are grasps in the array
	if (GraspArrayLeft.Num() == 0) 
	{
		return;
	}
	
	ActionFunctionsGrasp(&IndexGraspLeft, GraspArrayLeft, false, true);
	ActionFunctionsMenu(WidgetMenuComponent, MenuLeft, IndexGraspLeft);
}

void URealisticGraspingComponent::ButtonPreviousLeft()
{
     //Check if there are grasps in the array
	if (GraspArrayLeft.Num() == 0) 
	{
		return;
	}
	
	ActionFunctionsGrasp(&IndexGraspLeft, GraspArrayLeft, false, false);
	ActionFunctionsMenu(WidgetMenuComponent, MenuLeft, IndexGraspLeft);
}

//GraspLogic
void URealisticGraspingComponent::ActionFunctionsGrasp(int *GraspIndex, TArray<FString> GraspArray, bool Side, bool PreviousOrNext)
{
	//Check if there are grasps in the array
	if (GraspArray.Num() == 0) 
	{
		return;
	}

	if (PreviousOrNext)
	{
		*GraspIndex = SetIndexGraspNext(GraspArray, *GraspIndex);
	}
	else
	{
		*GraspIndex = SetIndexGraspPrevious(GraspArray, *GraspIndex);
	}
	
	Grasp = GraspArray[*GraspIndex];
	
	//Set the grip so that it can be used in the game
	AnimationController->SetNewAnimation(Grasp, Side);
}

int URealisticGraspingComponent::SetIndexGraspNext(TArray<FString> GrapsTypesArray, int Index)
{
	//At the beginning of the game, the index is set to zero
	//Index is increased by 1
	//If the Index value is equal to the number of elements in the array minus one , the index value is set to zero (This creates a loop)
	if (GrapsTypesArray.Num() == 0) 
	{
		return Index;
	}
	if (Index == -1)
	{
		Index = 0;
	}
	else if (Index >= GrapsTypesArray.Num() - 1)
	{
		Index = 0;
	}
	else
	{
		Index++;
	}
	return Index;
}

int URealisticGraspingComponent::SetIndexGraspPrevious(TArray<FString> GrapsTypesArray, int Index)
{
	//At the beginning of the game, the index is set to zero
	//Index is reduced by minus 1
	//If the Index value is equal to zero, the index is set  to the number of elements in the array minus 1 (This creates a loop)
	if (GrapsTypesArray.Num() == 0) 
	{
		return Index;
	}

	if (Index == -1)
	{
		Index = 0;
	}
	else if (Index == 0)
	{
		Index = GrapsTypesArray.Num() - 1;
	}
	else
	{
		Index--;
	}
	return Index;
}

//MenuLogic
void URealisticGraspingComponent::CreateMenuComponent(UWidgetComponent* *Menu)
{
	//Must be rotated 180 degrees because otherwise it is invisible
	//Location settings chosen so, that it is in the middle of the field of view
	//Scales to a manageable size (depending on the distance)
	//Sets the WidgetClass of the Component 
	//Sets the Component in the register
	(*Menu)->SetVisibility(false, false);
	(*Menu)->SetRelativeRotation(FRotator(0, 180, 0));
	(*Menu)->SetRelativeLocation(FVector(30, 0, -7));
	(*Menu)->SetRelativeScale3D(FVector(0.05, 0.05, 0.05));
	(*Menu)->SetWidgetClass(UGraspingMenu::StaticClass());
	(*Menu)->RegisterComponent();
	
	//The component can be attached to any element in the world
	(*Menu)->AttachToComponent(Camera, FAttachmentTransformRules::KeepRelativeTransform);
}

void URealisticGraspingComponent::OnTimerEnd()
{
	WidgetMenuComponent->SetVisibility(false, false);
}

void URealisticGraspingComponent::ActionFunctionsMenu(UWidgetComponent *WidgetCompnent, UGraspingMenu *Menu, int GraspIndex)
{
	//The widget component is made visible and the corresponding menu is set. This allows you to switch between the menus 
	WidgetCompnent->SetVisibility(true, true);
	WidgetCompnent->SetWidget(Menu);
	Menu->SwitchGraspType(&GraspIndex);
	GetWorld()->GetTimerManager().SetTimer(MenuInvisibleTime, this, &URealisticGraspingComponent::OnTimerEnd, MenuTime, false);
}




