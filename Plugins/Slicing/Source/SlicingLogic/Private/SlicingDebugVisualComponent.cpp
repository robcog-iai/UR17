// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "SlicingDebugVisualComponent.h"

#include "DrawDebugHelpers.h"
#include "TransformCalculus.h"
#include "KismetProceduralMeshLibrary.h"

#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintComponent.h"

#include "Engine/StaticMesh.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Called when the game starts
void USlicingDebugVisualComponent::BeginPlay()
{
	//Super::BeginPlay();

	//if (SlicingLogicModule->bEnableDebugConsoleOutput)
	//{
	//	// Currently has no other usage (reasonable one at least)
	//	UE_LOG(LogTemp, Warning, TEXT("SLICING: The blade component has been loaded into the world"));
	//}

	//// Check for the tip component to possibly abort the cutting
	//TipComponent = FSlicingLogicModule::GetSlicingComponent<USlicingTipComponent>(SlicingObject);

	//// Create the Physics Constraint
	//ConstraintOne = NewObject<UPhysicsConstraintComponent>();
	//ConstraintOne->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	//// Register the overlap events
	//OnComponentBeginOverlap.AddDynamic(this, &USlicingBladeComponent::OnBeginOverlap);
	//OnComponentEndOverlap.AddDynamic(this, &USlicingBladeComponent::OnEndOverlap);
}

// Called every frame (only used for the debugging-visuals)
void USlicingDebugVisualComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//// The debugging is only needed when cutting
	//if (!bIsCurrentlyCutting)
	//{
	//	return;
	//}

	//if (SlicingLogicModule->bEnableDebugShowPlane)
	//{
	//	USlicingBladeComponent::DrawSlicingPlane();
	//	USlicingBladeComponent::DrawCuttingEntrancePoint();
	//	USlicingBladeComponent::DrawCuttingExitPoint();
	//}

	//if (SlicingLogicModule->bEnableDebugShowTrajectory)
	//{
	//	USlicingBladeComponent::DrawCuttingTrajectory();
	//}
}

void USlicingDebugVisualComponent::DrawSlicingPlane()
{
	//FPlane SlicingPlane = FPlane(SlicingObject->GetComponentLocation(), GetUpVector());

	//// This size is actually about double the size of the component, but this is just the amount we need
	//float BladeComponentSize;
	//// Both of those variables are unused and not needed here
	//FVector BladeComponentOrigin, BladeComponentExtends;
	//UKismetSystemLibrary::GetComponentBounds(this, BladeComponentOrigin, BladeComponentExtends, BladeComponentSize);

	//DrawDebugSolidPlane(GetWorld(), SlicingPlane, CutComponent->GetComponentLocation(), BladeComponentSize,
	//	FColor::Red, false);
}

void USlicingDebugVisualComponent::DrawCuttingEntrancePoint()
{
	//FVector ComponentPosition = UKismetMathLibrary::TransformLocation(CutComponent->GetComponentTransform(), RelativeLocationToCutComponent);

	//DrawDebugBox(GetWorld(), ComponentPosition, FVector(3, 3, 3), CutComponent->GetComponentQuat(), FColor::Green);
}

void USlicingDebugVisualComponent::DrawCuttingExitPoint()
{
	//// Not yet implemented
	//FVector EndPosition = UKismetMathLibrary::TransformLocation(this->GetComponentTransform(), FVector(0, 1000, 0));
	//FHitResult Hits;
	//CutComponent->LineTraceComponent(Hits, EndPosition, this->GetComponentLocation(), FCollisionQueryParams::DefaultQueryParam);
	//DrawDebugBox(GetWorld(), Hits.Location, FVector(3, 3, 3), CutComponent->GetComponentQuat(), FColor::Red, true, 1.0F);
}

void USlicingDebugVisualComponent::DrawCuttingTrajectory()
{
	//FPlane SlicingPlane = FPlane(SlicingObject->GetComponentLocation(), GetUpVector());

	//DrawDebugPoint(GetWorld(), SlicingObject->GetSocketLocation(SocketBladeName),
	//	2, FColor::Purple, true, -1.0f, (uint8)'\100');
}