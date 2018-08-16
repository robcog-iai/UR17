// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#include "GMovement.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
UGMovement::UGMovement()
	:CurrentSpeed(0)
	, SpeedLimit(5.0f)
	, SpeedUpValue(0.01f)
	, SpeedUpTime(0.05f)
	, bIsMoving(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MinMovementSpeed = 0.05f;
	MaxMovementSpeed = 0.25f;

	Character = nullptr;
}

// Called when the game starts
void UGMovement::BeginPlay()
{
	Super::BeginPlay();

	// Set to MinMovmentSpeed, for the speed up
	CurrentSpeed = 0;

	Character = Cast<ACharacter>(GetOwner()); // Setup player

	SetMovable(true);
}

void UGMovement::MoveForward(const float Val) {
	if (bCanMove == false) return;

	if ((Character->Controller != nullptr) && (Val != 0.0f))
	{
		// Find out which way is forward
		FRotator Rotation = Character->Controller->GetControlRotation();
		// Limit pitch when walking or falling
		if (Character->GetCharacterMovement()->IsMovingOnGround() || Character->GetCharacterMovement()->IsFalling())
		{
			Rotation.Pitch = 0.0f;
		}
		// Add movement in that direction
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);

		// Add movement in the current direction
		SpeedUp(Direction, Val);		
	}
}

void UGMovement::MoveRight(const float Val) {
	if (bCanMove == false) return;

	if ((Character->Controller != nullptr) && (Val != 0.0f))
	{
		// Find out which way is right
		const FRotator Rotation = Character->Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);

		// Add movement in the current direction
		SpeedUp(Direction, Val);
	}
}

void UGMovement::AddControllerPitchInput(const float Val) {
	if (bCanMove == false) return;

	if (Character != nullptr)
	{
		Character->AddControllerPitchInput(Val);
	}
}

void UGMovement::AddControllerYawInput(const float Val) {
	if (bCanMove == false) return;
	if (Character != nullptr)
	{
		Character->AddControllerYawInput(Val);
	}
}

// Called every frame
void UGMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// Check if the character is still moving. If not reset the CurrentSpeed and SpeedUpTime. (By Waldemar Zeitler)
	if (Character->GetVelocity() == FVector::ZeroVector && bIsMoving) {
		CurrentSpeed = 0;
		SpeedUpTime = 0.05;
		bIsMoving = false;
		UE_LOG(LogTemp, Warning, TEXT("Reset Speed!"));
	}
}


void UGMovement::SetupKeyBindings(UInputComponent * PlayerInputComponent)
{
	/* Setup Input
	/* In Project setings -> Input
	/* Axis Mappings:
	/*   MoveForward: W 1.0, S -1,0
	/*   MoveRight: D 1.0, A -1.0
	/*   CameraPitch: MouseY -1.0
	/*   CameraYaw: MouseX 1.0
	*/

	// Set up gameplay key bindings
	PlayerInputComponent->BindAxis("MoveForward", this, &UGMovement::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &UGMovement::MoveRight);
	// Default Camera view bindings
	PlayerInputComponent->BindAxis("CameraPitch", this, &UGMovement::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("CameraYaw", this, &UGMovement::AddControllerYawInput);
}

void UGMovement::SetMovable(bool bCanMove)
{
	this->bCanMove = bCanMove;
}

float UGMovement::CalculateNewSpeed(float TimeStep)
{
	// The variables are fixed and change the curve of the function, if changed
	int TVariable = 1;

	// Variables to hold the values from the function
	float Tmp = -(TimeStep / TVariable);
	float BracePart = (1 - FMath::Exp(Tmp));
	float NewSpeed = MaxMovementSpeed * BracePart;

	return NewSpeed;
}

void UGMovement::SpeedUp(const FVector Direction, const float Val)
{
	// Recalculate the current speed (by Waldemar Zeitler)
	if (!bIsMoving) {
		bIsMoving = true;
	}

	if (SpeedUpTime < 2) {
		SpeedUpTime += SpeedUpValue;
	}
	CurrentSpeed = CalculateNewSpeed(SpeedUpTime) + MinMovementSpeed;
	CurrentSpeed = CurrentSpeed * Val;

	Character->AddMovementInput(Direction, CurrentSpeed);
	UE_LOG(LogTemp, Warning, TEXT("Current Speed: %f"), CurrentSpeed);
}