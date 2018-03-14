// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#include "SlicingBladeComponent.h"

#include "DrawDebugHelpers.h"
#include "TransformCalculus.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"

#include "Engine/StaticMesh.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Called before BeginPlay()
void USlicingBladeComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

// Called when the game starts
void USlicingBladeComponent::BeginPlay()
{
	Super::BeginPlay();

	if (SlicingLogicModule->bEnableDebugConsoleOutput)
	{
		// Currently has no other usage (reasonable one at least)
		UE_LOG(LogTemp, Warning, TEXT("SLICING: The blade component has been loaded into the world"));
	}
}

// Called every frame
void USlicingBladeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// The debugging is only needed when cutting
	if (!bIsCurrentlyCutting)
	{
		return;
	}

	if (SlicingLogicModule->bEnableDebugShowPlane)
	{
		USlicingBladeComponent::DrawSlicingComponents();
		USlicingBladeComponent::DrawSlicingPlane();
		USlicingBladeComponent::DrawCuttingEntrancePoint();
	}

	if (SlicingLogicModule->bEnableDebugShowTrajectory)
	{
		USlicingBladeComponent::DrawCuttingTrajectory();
	}
}

void USlicingBladeComponent::OnBladeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// This event is only important if the other object actually exists
	if (OtherComp == nullptr || OtherComp == NULL)
	{
		return;
	}

	// This event is only important if the other object can actually be cut or if another cut hasn't already started
	if (!OtherComp->ComponentHasTag(TagCuttable) || bIsCurrentlyCutting)
	{
		return;
	}

	// Collision should only be ignored with the currently cut object, not the object around it
	SlicingObject->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);

	// If physics are on, the relative location and such will be seen relative to the world location
	RelativeLocationToCutComponent = OtherComp->GetComponentTransform().InverseTransformPosition(OverlappedComp->GetComponentLocation());
	RelativeRotationToCutComponent = OverlappedComp->GetComponentQuat() - OtherComp->GetComponentQuat();

	// In case the component is a StaticMeshComponent it needs to be converted into a ProceduralMeshComponent
	if (OtherComp->GetClass() == UStaticMeshComponent::StaticClass()
		&& ((UStaticMeshComponent*)OtherComp)->GetStaticMesh())
	{
		FSlicingLogicModule::ConvertStaticToProceduralMeshComponent(OtherComp);

		// Retry the event with the new ProceduralMeshComponent
		return;
	}

	// The other object is a ProceduralMeshComponent and the cutting can now be continued
	bIsCurrentlyCutting = true;
	CutComponent = (UProceduralMeshComponent*)OtherComp;
}

void USlicingBladeComponent::OnBladeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// This event is only important if you are actually in the cutting process and not trying to abort it
	if (!bIsCurrentlyCutting || bPulledOutCuttingObject)
	{
		bPulledOutCuttingObject = false;
		return;
	}

	//##!! Wann wird das benötigt???
	FVector vector = UKismetMathLibrary::TransformLocation(CutComponent->GetComponentTransform(), RelativeLocationToCutComponent);
	if (OverlappedComp->OverlapComponent(vector, CutComponent->GetComponentQuat(), OverlappedComp->GetCollisionShape()))
	{
		// Collision should only be ignored with the currently cut object, not the object around it
		SlicingObject->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Block);
		bIsCurrentlyCutting = false;

		return;
	}

	//##!! Braucht man eigentlich nicht...
	if (!OtherComp->ComponentHasTag(TagCuttable) || OtherComp->GetClass() != UProceduralMeshComponent::StaticClass())
	{
		return;
	}

	SliceComponent(OtherComp);

	//##!! Braucht man das??
	SlicingObject->SetCollisionProfileName(FName("PhysicsActor"));

	bIsCurrentlyCutting = false;
	FlushPersistentDebugLines(this->GetWorld());
}

void USlicingBladeComponent::DrawSlicingComponents()
{
	// Drawing the blade
	DrawDebugBox(GetWorld(), GetComponentLocation(), GetScaledBoxExtent(), GetComponentRotation().Quaternion(),
		FColor::Green, true, 0.01f);

	// Drawing the handle


	// Drawing the tip

}

void USlicingBladeComponent::DrawSlicingPlane()
{
	FPlane SlicingPlane = FPlane(SlicingObject->GetComponentLocation(), GetUpVector());

	FVector ComponentPosition = UKismetMathLibrary::TransformLocation(CutComponent->GetComponentTransform(), RelativeLocationToCutComponent)
		- FVector(0, 5, 0);

	//FVector ComponentExtraPosition = UKismetSystemLibrary::GetComponentBounds(...)

	DrawDebugSolidPlane(GetWorld(), SlicingPlane, ComponentPosition, FVector2D(5, 4), FColor::Red, false);
}

void USlicingBladeComponent::DrawCuttingEntrancePoint()
{
	FVector ComponentPosition = UKismetMathLibrary::TransformLocation(CutComponent->GetComponentTransform(), RelativeLocationToCutComponent);

	DrawDebugBox(GetWorld(), ComponentPosition, FVector(3, 3, 3), CutComponent->GetComponentQuat(),
		FColor::Green, true, 1.0F);
}

void USlicingBladeComponent::DrawCuttingExitPoint()
{
	// Not yet implemented
}

void USlicingBladeComponent::DrawCuttingTrajectory()
{
	FPlane SlicingPlane = FPlane(SlicingObject->GetComponentLocation(), GetUpVector());

	DrawDebugPoint(GetWorld(), SlicingObject->GetSocketLocation(SocketBladeName),
		2, FColor::Purple, true, -1.0f, (uint8)'\100');
}

void USlicingBladeComponent::SliceComponent(UPrimitiveComponent* CuttableComponent)
{
	UProceduralMeshComponent* OutputProceduralMesh;

	UKismetProceduralMeshLibrary::SliceProceduralMesh(
		(UProceduralMeshComponent*)CuttableComponent,
		SlicingObject->GetSocketLocation(SocketBladeName),
		SlicingObject->GetUpVector(),
		true,
		OutputProceduralMesh,
		EProcMeshSliceCapOption::NoCap,
		CuttableComponent->GetMaterial(0)
	);

	OutputProceduralMesh->bGenerateOverlapEvents = true;
	OutputProceduralMesh->SetEnableGravity(true);
	OutputProceduralMesh->SetSimulatePhysics(true);
	OutputProceduralMesh->ComponentTags = CuttableComponent->ComponentTags;
}