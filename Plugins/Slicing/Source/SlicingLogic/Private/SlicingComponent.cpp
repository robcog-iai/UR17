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

	bGenerateOverlapEvents = true;

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

	if (SlicingLogicModule.bEnableDebugShowPlane)
	{
		DrawDebugBox(this->GetWorld(), this->GetComponentLocation(), this->GetScaledBoxExtent(), this->GetComponentRotation().Quaternion(), FColor::Green);

		DrawDebugSolidPlane(this->GetWorld(), FPlane(this->GetAttachmentRoot()->GetComponentLocation(), this->GetUpVector()),
			this->GetAttachmentRoot()->GetSocketLocation(FName("BladeBox")), FVector2D(5, 5), FColor::Red, false, 0.01f);
	}

	if (SlicingLogicModule.bEnableDebugConsoleOutput)
	{
		UE_LOG(LogTemp, Warning, TEXT("Test"));
	}

	if (SlicingLogicModule.bEnableDebugShowTrajectory)
	{
		DrawDebugSolidPlane(this->GetWorld(), FPlane(this->GetAttachmentRoot()->GetComponentLocation(), this->GetUpVector()),
			this->GetAttachmentRoot()->GetSocketLocation(FName("BladeBox")), FVector2D(5, 5), FColor::Blue, false, 1.0f);
	}
}

void USlicingComponent::OnBladeBeginOverlap(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherComp->ComponentHasTag(FName("Cuttable")))
	{
		return;
	}
	/*
	Converting the given Component to Procedural Mesh Component
	*/
	UPrimitiveComponent* ReferencedComponent = OtherComp;
	UStaticMeshComponent* Parent = (UStaticMeshComponent*)(this->GetAttachmentRoot());
	Parent->SetCollisionProfileName(FName("OverlapAll"));
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

			ReferencedComponent->DestroyComponent();
		}
	}
}

void USlicingComponent::OnBladeEndOverlap(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherComp->ComponentHasTag(FName("Cuttable")) || OtherComp->GetClass() != UProceduralMeshComponent::StaticClass())
	{
		return;
	}
	UProceduralMeshComponent* OutputProceduralMesh;

	UKismetProceduralMeshLibrary::SliceProceduralMesh(
		(UProceduralMeshComponent*)OtherComp,
		this->GetAttachmentRoot()->GetSocketLocation(FName("BladeBox")),
		this->GetAttachmentRoot()->GetUpVector(),
		true,
		OutputProceduralMesh,
		EProcMeshSliceCapOption::NoCap,
		OtherComp->GetMaterial(0)
	);

	OutputProceduralMesh->bGenerateOverlapEvents = true;
	OutputProceduralMesh->ComponentTags = OtherComp->ComponentTags;

	UStaticMeshComponent* Parent = (UStaticMeshComponent*)(this->GetAttachmentRoot());
	Parent->SetCollisionProfileName(FName("PhysicsActor"));
}