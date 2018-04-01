// Copyright 2017-2018 Kannan Thambiah. All rights reserved.

#include "ScrewJointActor.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "UObject/UObjectGlobals.h"

// Constant variables
const float AScrewJointActor::Degree2Turns = 0.00277f;
const float AScrewJointActor::Turns2Degree = 360.0f;

// Sets default values
AScrewJointActor::AScrewJointActor() :
	bShowDebugLog(false), bIsOpen(false), Angle(45.0f), Turn(45.0f*Degree2Turns), bRightHanded(true),
	ThreadHeight(0.1), Lead(0.1), InitialForce(10000.0f), ForceThreshold(0.1f), StandardDamping(100.0f),
	bLidSet(false), bContainerSet(false), PrevSwing(0.0f), bPrevIsOpen(bIsOpen), bLidOnContainer(false)
{
	PrimaryActorTick.bCanEverTick = true;

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));

	// Spheres to detecting when lid is back on container
	LidSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LidSphere"));
	LidSphere->SetVisibility(false);
	LidSphere->bGenerateOverlapEvents = true;
	LidSphere->OnComponentBeginOverlap.AddDynamic(this, &AScrewJointActor::OnOverlapBegin);
	LidSphere->OnComponentEndOverlap.AddDynamic(this, &AScrewJointActor::OnOverlapEnd);

	ContainerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ContainerSphere"));
	ContainerSphere->SetVisibility(false);
}

// Called every frame
void AScrewJointActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShowDebugLog)
	{
		float swing1 = PhysicsConstraint->GetCurrentSwing1();
		FRotator rotation = Lid->GetActorRotation();

		// Output
		if (swing1 != PrevSwing || bIsOpen != bPrevIsOpen)
		{

			if (GEngine)
			{
				GEngine->ClearOnScreenDebugMessages();
			}
			float ZAxisOffset = PhysicsConstraint->ConstraintInstance.AngularRotationOffset.GetComponentForAxis(EAxis::Z);
			float AngularDamping = Lid->GetStaticMeshComponent()->GetAngularDamping();
			PrintDebugLog(FString::Printf(TEXT("Handednes: %s"), bRightHanded ? TEXT("right") : TEXT("left")));
			PrintDebugLog(FString::Printf(TEXT("Angle: %f"), Angle));
			PrintDebugLog(FString::Printf(TEXT("Offset: %f"), ZAxisOffset));
			PrintDebugLog(FString::Printf(TEXT("Swing: %f"), swing1));
			PrintDebugLog(FString::Printf(TEXT("Lid Rotation: %f - %f - %f"), rotation.Pitch, rotation.Yaw, rotation.Roll));
			PrintDebugLog(FString::Printf(TEXT("Damping: %f"), AngularDamping));
			PrintDebugLog(FString::Printf(TEXT("Open: %s"), bIsOpen ? TEXT("yes") : TEXT("no")));
		}

		// Only print something if changes happened
		PrevSwing = swing1;
		bPrevIsOpen = bIsOpen;
	}

	// If closed, check to open
	if (!bIsOpen)
	{
		float DeltaTurn = FMath::Abs(InitialSwing - PhysicsConstraint->GetCurrentSwing1());
		float Damping = Lid->GetStaticMeshComponent()->GetAngularDamping();

		// Apply/remove initial force
		if (DeltaTurn > Angle * ForceThreshold && Damping == InitialForce)
		{
			Lid->GetStaticMeshComponent()->SetAngularDamping(StandardDamping);
		}
		else if (DeltaTurn < Angle * ForceThreshold && Damping == StandardDamping)
		{
			Lid->GetStaticMeshComponent()->SetAngularDamping(InitialForce);
		}

		// Check if open
		if (DeltaTurn >= Angle)
		{
			bIsOpen = true;
			PhysicsConstraint->BreakConstraint();
		}
	}

	// If open, check to close
	if (bIsOpen && bLidOnContainer)
	{
		float CurrentZAxisRotation = Lid->GetActorRotation().GetComponentForAxis(EAxis::Z);
		float InitialZAxisRotation = InitialLidRotation.GetComponentForAxis(EAxis::Z);

		if (FMath::Abs(CurrentZAxisRotation - InitialZAxisRotation) >= 1)
		{
			bIsOpen = false;
			bLidOnContainer = false;
			InitializeConstraint();
		}
	}

	// Reposition collision spheres according to their "owners"
	PositionSphere(LidSphere, Lid, -0.5f);
	PositionSphere(ContainerSphere, Container, 0.5f);
}

void AScrewJointActor::InitializeConstraint()
{
	// Set up components
	PhysicsConstraint->ConstraintActor1 = Container;
	PhysicsConstraint->ConstraintActor2 = Lid;
	Lid->GetStaticMeshComponent()->SetAngularDamping(InitialForce);

	// Position constraint on top of container
	FBox ContainerBoundingBox = Container->GetComponentsBoundingBox();
	FVector position = ContainerBoundingBox.GetCenter();

	position.Z = position.Z + 0.5 * ContainerBoundingBox.GetSize().Z;
	PhysicsConstraint->SetWorldLocation(position);

	// Set constraint
	PhysicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, Angle);
	PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0);
	PhysicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);
	PhysicsConstraint->ConstraintInstance.ProfileInstance.ConeLimit.bSoftConstraint = false;	// Disable SwingLimits->SoftConstraint			

	if (bRightHanded)
	{
		PhysicsConstraint->ConstraintInstance.AngularRotationOffset = FRotator::FRotator(0, Angle, 0);
	}
	else
	{
		PhysicsConstraint->ConstraintInstance.AngularRotationOffset = FRotator::FRotator(0, -Angle, 0);
	}

	// Set up and position spheres
	SetSphereRadius();
	PositionSphere(LidSphere, Lid, -0.5f);
	PositionSphere(ContainerSphere, Container, 0.5f);

	// Initialize constraint
	PhysicsConstraint->InitComponentConstraint();
	PrintDebugLog(TEXT("Constraint initialized"));
}

#if WITH_EDITOR
void AScrewJointActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	// Get all screw joint components
	TArray<UActorComponent*> ScrewJointComponents;
	GetComponents(ScrewJointComponents);

	// Get name of the property that was changed
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// GET_MEMBER_NAME_CHECKED prevents future property name changes, as they will result in compilation error
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AScrewJointActor, Lid))
	{
		Lid->SetMobility(EComponentMobility::Movable);
		Lid->GetStaticMeshComponent()->SetSimulatePhysics(true);

		bLidSet = true;
		PrintDebugLog(TEXT("Lid object set"));
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(AScrewJointActor, Container))
	{
		Container->SetMobility(EComponentMobility::Movable);
		Container->GetStaticMeshComponent()->SetSimulatePhysics(true);

		bContainerSet = true;
		PrintDebugLog(TEXT("Container object set"));
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(AScrewJointActor, Angle))
	{
		Turn = Angle * Degree2Turns;
		PrintDebugLog(FString::Printf(TEXT("Angle changed, NoOfTurns calculated: %f"), Turn));
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(AScrewJointActor, Turn))
	{
		Angle = Turn * 360;
		PrintDebugLog(FString::Printf(TEXT("NoOfTurns changed, Angle calculated: %f"), Angle));
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(AScrewJointActor, ThreadHeight))
	{
		Lead = ThreadHeight / Turn;
		PrintDebugLog(FString::Printf(TEXT("Thread height changed, lead calculated: %f"), Lead));
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(AScrewJointActor, Lead))
	{
		ThreadHeight = Turn * Lead;
		PrintDebugLog(FString::Printf(TEXT("Lead changed, ThreadHeight calculated: %f"), ThreadHeight));
	}

	if (bLidSet && bContainerSet && !bIsOpen)
	{
		InitializeConstraint();
		PrintDebugLog(TEXT("(Re)Initialize constraint!"));
	}
}
#endif

void AScrewJointActor::BeginPlay()
{
	// Save initial lid swing
	InitialSwing = PhysicsConstraint->GetCurrentSwing1();
	Super::BeginPlay();
}

void AScrewJointActor::SetSphereRadius()
{
	float approximateRadius = Lid->GetComponentsBoundingBox().GetSize().X;
	LidSphere->SetSphereRadius(0.1f * approximateRadius);
	ContainerSphere->SetSphereRadius(0.1f * approximateRadius);;
}

void AScrewJointActor::PositionSphere(USphereComponent* Sphere, const AStaticMeshActor* SMActor, const float ZAxisFactor)
{
	FVector Position = SMActor->GetComponentsBoundingBox().GetCenter();
	Position.Z = Position.Z + ZAxisFactor * SMActor->GetComponentsBoundingBox().GetSize().Z;
	Sphere->SetWorldLocation(Position);
	Sphere->SetVisibility(true);
}

void AScrewJointActor::PrintDebugLog(FString string) const
{
	if (GEngine && bShowDebugLog)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, string);
	}
}

void AScrewJointActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	if (OtherComp == ContainerSphere && bIsOpen)
	{
		PrintDebugLog(TEXT("Lid back on Container!"));
		InitialLidRotation = Lid->GetActorRotation();
		bLidOnContainer = true;
	}
}

void AScrewJointActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherComp == ContainerSphere)
	{
		PrintDebugLog(TEXT("Lid removed from Container!"));
		bLidOnContainer = false;
	}
}
