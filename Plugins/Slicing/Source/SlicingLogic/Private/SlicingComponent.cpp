// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#include "SlicingComponent.h"
#include "SlicingLogicModule.h"

#include "DrawDebugHelpers.h"

#include "Engine/StaticMesh.h"
#include "Components/BoxComponent.h"
#include "ProceduralMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

USlicingComponent::USlicingComponent()
{
	// Needed if one wants to use the TickComponent function
	PrimaryComponentTick.bCanEverTick = true;
	// Needed if one wants to use the InitializeComponent function
	bWantsInitializeComponent = true;

	// Register the logic functions
	OnComponentBeginOverlap.AddDynamic(this, &USlicingComponent::OnBladeBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &USlicingComponent::OnBladeEndOverlap);
}

void USlicingComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void USlicingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USlicingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//* Needed for the debug option booleans
	FSlicingLogicModule& SlicingLogicModule =
		FModuleManager::Get().LoadModuleChecked<FSlicingLogicModule>("SlicingLogic");

	UE_LOG(LogTemp, Warning, TEXT("Ticking?"));
	if (SlicingLogicModule.bEnableDebugShowPlane)
	{
		DrawDebugBox(this->GetWorld(), this->GetComponentLocation(), this->GetScaledBoxExtent(), FColor::Green);

		DrawDebugSolidPlane(this->GetWorld(), FPlane(this->GetAttachmentRoot()->GetUpVector()),
			this->GetAttachmentRoot()->GetComponentLocation(), FVector2D(3, 3), FColor::Red, false, 0.1f);
	}

	if (SlicingLogicModule.bEnableDebugConsoleOutput)
	{
		UE_LOG(LogTemp, Warning, TEXT("Test"));
	}

	if (SlicingLogicModule.bEnableDebugShowTrajectory)
	{

	}
}

void USlicingComponent::OnBladeBeginOverlap(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*
	Converting the given Component to Procedural Mesh Component
	*/
	UPrimitiveComponent* ReferencedComponent = OtherComp;
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

			UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(
				((UStaticMeshComponent*)ReferencedComponent), 0, NewComponent, true);

			ReferencedComponent->DestroyComponent();
			ReferencedComponent = NewComponent;
		}
		UStaticMeshComponent* Parent = (UStaticMeshComponent*)(this->GetAttachmentRoot());
		Parent->SetSimulatePhysics(false);
		Parent->SetCollisionProfileName(FName("OverlapAll"));
	}
}

void USlicingComponent::OnBladeEndOverlap(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UProceduralMeshComponent* OutputProceduralMesh;

	UKismetProceduralMeshLibrary::SliceProceduralMesh(
		(UProceduralMeshComponent*)OtherComp,
		this->GetAttachmentRoot()->GetComponentLocation(),
		this->GetAttachmentRoot()->GetUpVector(),
		true,
		OutputProceduralMesh,
		EProcMeshSliceCapOption::NoCap,
		OtherComp->GetMaterial(0)
	);

	UStaticMeshComponent* Parent = (UStaticMeshComponent*)(this->GetAttachmentRoot());
	Parent->SetSimulatePhysics(true);
	Parent->SetCollisionProfileName(FName("PhysicsActor"));
}