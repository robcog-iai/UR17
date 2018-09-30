// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PIDController3D.h"
#include "AGraspingHand.h"
#include "UGraspingAnimationController.h"
#include "UGraspingController.h"
#include "MotionControllerComponent.h"
#include "ForceBaseIndicatorInterface.h"
#include "GraspingMenu.h"
#include "WidgetComponent.h"
#include "Runtime/UMG/Public/Slate/WidgetRenderer.h"
#include "URealisticGraspingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UREALISTICGRASPING_API URealisticGraspingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URealisticGraspingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	virtual void InitializeComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//HMD Camera
	UCameraComponent* Camera;

	//Motion Controller Left
	UMotionControllerComponent* MotionControllerLeft;

	//Motion Controller Right
	UMotionControllerComponent* MotionControllerRight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float PGain = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float IGain = 1.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float DGain = 5.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MaxOutput = 350000.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float RotationBoost = 8000.f;

	//Left hand of the character
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		AGraspingHand* LeftHand;

	//The right hand of the character
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		AGraspingHand* RightHand;

	// Offset for the left hand Roation
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator OffsetL = FRotator(90, 0, 0);;

	// Offset for the right hand Roation
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FRotator OffsetR = FRotator(90, 0, 180);

	// The class that should be used to visualize the force
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<UObject> ForceIndicatorInterface;

	// Set this to true if you want to visualize the force 
	//(you also have to set a class see ForceIndicatorInterface)
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bUseForceIndicator = false;

	// Used to activate TighteningGrasp:
	// While active, after grasping for a certain amount of time the fingers that are not at their desired position get additional force added to them,
	// by icnreasing the target rotation of their bones
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bEnableTightening = false;
	
	//Offset for the visibility time of the menu
	UPROPERTY(EditAnywhere)
		float MenuTime = 1.0f;

	//Moves the player to the cameras world position
	void ResetHeadPosition();

	//tells the grasping controller to grasp with left hand
	void GraspWithLeftHand(const float Input);

	//tells the grasping controller to grasp with right hand
	void GraspWithRightHand(const float Input);

	// Update hand positions
	FORCEINLINE void UpdateHandLocationAndRotation(
		UMotionControllerComponent* MC,
		FPIDController3D& PIDController,
		USkeletalMeshComponent* SkelMesh,
		FRotator Offset,
		const float DeltaTime);

private:

	//The class taht should visualize the force
	IForceBaseIndicatorInterface* ForceIndicator;

	//If the head was reseted using the motion controller
	bool bHeadReset = false;

	// Left hand controller
	FPIDController3D LeftPIDController;

	// Right hand controller
	FPIDController3D RightPIDController;

	//Grasping controller for the left and right hand
	UGraspingController* GraspingControllerR;
	UGraspingController* GraspingControllerL;

	//The Animation Controller that handles all animations for both hands
	UGraspingAnimationController* AnimationController;

	//Array of grasping types of the right hand
	TArray<FString> GrapsTypesRight;

	//Array of grasping types of the left hand
	TArray<FString> GrapsTypesLeft;

	//Active grasp
	FString Grasp;

	//Index numbers of grasp types for the GrapsTypes-Array
	//Defines the current grip
	//For the left hand and for the right hand
	int IndexGraspRight;
	int IndexGraspLeft;

	//Menu for the right hand
	UGraspingMenu* MenuRight;

	//Menu for the left hand
	UGraspingMenu*MenuLeft;

	//Component in the world where the menu is set
	//This makes it possible to place the menu anywhere in the world
	UWidgetComponent* WidgetMenuComponent;

	//Stored grasp names of the right hand
	TArray<FString> GraspArrayRight;

	//Stored grasp names of the left hand
	TArray<FString> GraspArrayLeft;

	//Help variable for the delay function
	//Should not be set
	FTimerHandle MenuInvisibleTime;


	//adds the function responsible for loading grasping data into the grasping controller to the delegate
	//that broadcasts everytime the player changes grasping types
	void EventSetup();

	//Function to set the next animation for the ForceBaseIndicator Class
	UFUNCTION()
	void SetGraspingDataRight(FHandAnimationData Data);

	//Function to set the next animation for the ForceBaseIndicator Class
	UFUNCTION()
	void SetGraspingDataLeft(FHandAnimationData Data);

	//Changes the index of the left or right hand (In this case plus one)
	//@param GrapsTypesArray, array with the stored grasp names from the right or left hand
	//@param Index, index of the current grasp from the right or left hand
	int SetIndexGraspNext(TArray<FString> GrapsTypesArray, int Index);

	//Changes the index of the left or right hand (In this case minus one)
	//@param GrapsTypesArray, array with the stored grasp names from the right or left hand
	//@param Index, index of the current grasp from the right or left hand
	int  SetIndexGraspPrevious(TArray<FString> GrapsTypesArray, int Index);

	//Makes all the adjustment to the WidgetComponent
	//@param Menu, for which WidgetComponent the settings are made
	void CreateMenuComponent(UWidgetComponent* *Menu);

	//Switches to the next button and grasp for the right hand (In this case down)
	//Function is called by pressing the "A-Button" of the right Motion-Controller
	void ButtonNextRight();

	//Switches to the previous button and grasp for the right hand (In this case up)
	//Function is called by pressing the "B-Button" of the right Motion-Controller
	void ButtonPreviousRight();

	//Switches to the next button and grasp for the left hand (In this case down)
	//Function is called by pressing the "A-Button" of the left Motion-Controller
	void ButtonNextLeft();

	//Switches to the previous button and grasp for the left hand (In this case up)
	//Function is called by pressing the "B-Button" of the left Motion-Controller
	void ButtonPreviousLeft();

	//Delay function (duration for how long the menu is visible)
	void OnTimerEnd();

	//Logic for the Button switch
	//@param WidgetCompnent, needs a previously created UWidgetComponent
	//@param Menu, for which menu the settings are made
	//@param GraspIndex, index of the current grasp from the right or left hand
	//@param PreviousOrNext, indicates in which direction is changed
	void ActionFunctionsMenu(UWidgetComponent *WidgetCompnent, UGraspingMenu *Menu, int GraspIndex);

	//Logic for the Grasp switch
	//@param GraspIndex, index of the current grasp from the right or left hand
	//@param GraspArray, array with the stored Grasps from the right or left hand
	//@param Side, indicates for which hand the selected grip should be set
	//@param PreviousOrNext, indicates in which direction is changed
	void ActionFunctionsGrasp(int *GraspIndex, TArray<FString> GraspArray, bool Side, bool PreviousOrNext);
};
