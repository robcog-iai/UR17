// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#include "SlicingComponent.h"

#include "DrawDebugHelpers.h"
#include "TransformCalculus.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"

#include "Engine/StaticMesh.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


// Setting the text for the static names used in the editor
const FName USlicingComponent::SocketHandleName = "SlicingHandle";
const FName USlicingComponent::SocketBladeName = "SlicingBlade";
const FName USlicingComponent::SocketCuttingExitpointName = "SlicingCuttingExitpoint";
const FName USlicingComponent::TagCuttable = "Cuttable";

USlicingComponent::USlicingComponent()
{
	// Enables the usage of the TickComponent function
	PrimaryComponentTick.bCanEverTick = true;
	// Enables the usage of the InitializeComponent function
	bWantsInitializeComponent = true;
	// Enables the slicing itself
	bGenerateOverlapEvents = true;

	// Register the overlap events
	OnComponentBeginOverlap.AddDynamic(this, &USlicingComponent::OnBladeBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &USlicingComponent::OnBladeEndOverlap);
}

// Called before BeginPlay()
void USlicingComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

// Called when the game starts
void USlicingComponent::BeginPlay()
{
	Super::BeginPlay();

	SlicingObject = (UStaticMeshComponent*)(this->GetAttachmentRoot());
	SlicingLogicModule = &FModuleManager::Get().LoadModuleChecked<FSlicingLogicModule>("SlicingLogic");

	if (SlicingLogicModule->bEnableDebugConsoleOutput)
	{
		// Currently has no other usage (reasonable one at least)
		UE_LOG(LogTemp, Warning, TEXT("SLICING: The component has been loaded into the world"));
	}
}

// Called every frame
void USlicingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// The debugging is only needed when cutting
	if (!bIsCurrentlyCutting)
	{
		return;
	}

	/*
	// PROBLEM: Always true
	// Find out whether the CuttingExitpoint is overlapping -> abort slicing
	if (!bPulledOutCuttingObject)
	{
		const FVector CuttingExitpointPosition = SlicingObject->GetChildComponent(0)->GetComponentLocation();
		const FQuat CuttingExitpointRotation = SlicingObject->GetChildComponent(0)->GetComponentQuat();
		const FCollisionShape CuttingExitpointCollisionShape = ((UPrimitiveComponent*)SlicingObject->GetChildComponent(0))->GetCollisionShape();

		bPulledOutCuttingObject = CutComponent->OverlapComponent(
			CuttingExitpointPosition, CuttingExitpointRotation, CuttingExitpointCollisionShape);
	}
	*/

	if (SlicingLogicModule->bEnableDebugShowPlane)
	{
		USlicingComponent::DrawSlicingComponents();
		USlicingComponent::DrawSlicingPlane();
		USlicingComponent::DrawCuttingEntrancePoint();
	}

	if (SlicingLogicModule->bEnableDebugShowTrajectory)
	{
		USlicingComponent::DrawCuttingTrajectory();
	}
}

void USlicingComponent::OnBladeBeginOverlap(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherComp->ComponentHasTag(TagCuttable) || bIsCurrentlyCutting)
	{
		return;
	}
	//relLocation = OtherComp->GetComponentTransform().GetRelativeTransform(this->GetComponentTransform());
	
	/*
	Converting the given Component to Procedural Mesh Component
	*/
	UPrimitiveComponent* ReferencedComponent = OtherComp;
	SlicingObject->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
	bIsCurrentlyCutting = true;
	/*
		If Physics are on, the relative location and such will be seen relative to the world location.
	*/
	relLocation = OtherComp->GetComponentTransform().InverseTransformPosition(OverlappedComp->GetComponentLocation());
	//relLocation = OverlappedComp->GetComponentLocation() - OtherComp->GetComponentLocation();
	relRotation = OverlappedComp->GetComponentQuat() - OtherComp->GetComponentQuat();
	if (ReferencedComponent != nullptr  && ReferencedComponent != NULL)
	{
		// In case the Component is a StaticMeshComponent, uses following to make a ProceduralMeshComponent
		if (ReferencedComponent->GetClass() == UStaticMeshComponent::StaticClass()
			&& ((UStaticMeshComponent*)ReferencedComponent)->GetStaticMesh())
		{
			((UStaticMeshComponent*)ReferencedComponent)->GetStaticMesh()->bAllowCPUAccess = true;

			UProceduralMeshComponent* NewComponent = NewObject<UProceduralMeshComponent>(ReferencedComponent);
			NewComponent->SetRelativeTransform(ReferencedComponent->GetRelativeTransform());
			NewComponent->RegisterComponent();
			NewComponent->SetCollisionProfileName(FName("PhysicsActor"));
			NewComponent->bUseComplexAsSimpleCollision = false;
			NewComponent->SetEnableGravity(true);
			NewComponent->SetSimulatePhysics(true);
			NewComponent->bGenerateOverlapEvents = true;
			NewComponent->ComponentTags = ReferencedComponent->ComponentTags;

			UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(
				((UStaticMeshComponent*)ReferencedComponent), 0, NewComponent, true);
			bIsCurrentlyCutting = false;
			ReferencedComponent->DestroyComponent();
			return;
		}
		CutComponent = (UProceduralMeshComponent*) OtherComp;
	}
}

void USlicingComponent::OnBladeEndOverlap(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bIsCurrentlyCutting || bPulledOutCuttingObject) 
	{
		bPulledOutCuttingObject = false;
		return;
	}

	if (OverlappedComp->OverlapComponent(UKismetMathLibrary::TransformLocation(CutComponent->GetComponentTransform(), relLocation),
		CutComponent->GetComponentQuat(),
		OverlappedComp->GetCollisionShape())) 
	{
		SlicingObject->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Block);
		bIsCurrentlyCutting = false;

		return;
	}

	if (!OtherComp->ComponentHasTag(TagCuttable) || OtherComp->GetClass() != UProceduralMeshComponent::StaticClass())
	{
		return;
	}
	UProceduralMeshComponent* OutputProceduralMesh;
	TArray<USceneComponent*> Parents;
	this->GetParentComponents(Parents);

	UKismetProceduralMeshLibrary::SliceProceduralMesh(
		(UProceduralMeshComponent*)OtherComp,
		Parents[0]->GetSocketLocation(SocketBladeName),
		Parents[0]->GetUpVector(),
		true,
		OutputProceduralMesh,
		EProcMeshSliceCapOption::NoCap,
		OtherComp->GetMaterial(0)
	);

	OutputProceduralMesh->bGenerateOverlapEvents = true;
	OutputProceduralMesh->SetEnableGravity(true);
	OutputProceduralMesh->SetSimulatePhysics(true);
	OutputProceduralMesh->ComponentTags = OtherComp->ComponentTags;

	SlicingObject->SetCollisionProfileName(FName("PhysicsActor"));
	bIsCurrentlyCutting = false;
	FlushPersistentDebugLines(this->GetWorld());
}

void USlicingComponent::DrawSlicingComponents()
{
	// Drawing the blade
	DrawDebugBox(GetWorld(), GetComponentLocation(), GetScaledBoxExtent(), GetComponentRotation().Quaternion(),
		FColor::Green, true, 0.01f);

	// Drawing the handle


	// Drawing the tip

}

void USlicingComponent::DrawSlicingPlane()
{
	FPlane SlicingPlane = FPlane(GetAttachmentRoot()->GetComponentLocation(), GetUpVector());

	FVector ComponentPosition = UKismetMathLibrary::TransformLocation(CutComponent->GetComponentTransform(), relLocation)
		+ FVector(0,5,0);

	//FVector ComponentExtraPosition = UKismetSystemLibrary::GetComponentBounds(...)

	DrawDebugSolidPlane(GetWorld(), SlicingPlane, ComponentPosition, FVector2D(5, 4), FColor::Red, false);
}

void USlicingComponent::DrawCuttingEntrancePoint()
{
	DrawDebugBox(this->GetWorld(),
		UKismetMathLibrary::TransformLocation(CutComponent->GetComponentTransform(), relLocation),
		FVector(3, 3, 3), CutComponent->GetComponentQuat(), FColor::Green, true, 1.0F);
}

void USlicingComponent::DrawCuttingExitPoint()
{
	// Not yet implemented
}

void USlicingComponent::DrawCuttingTrajectory()
{
	TArray<USceneComponent*> Parents;
	this->GetParentComponents(Parents);
	DrawDebugSolidPlane(this->GetWorld(), FPlane(this->GetAttachmentRoot()->GetComponentLocation(), this->GetUpVector()),
		Parents[0]->GetSocketLocation(SocketBladeName), FVector2D(0.3, 5), FColor::Blue, true);
}