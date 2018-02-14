// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#include "SlicingComponent.h"
#include "SlicingLogicModule.h"

#include "DrawDebugHelpers.h"
#include "TransformCalculus.h"

#include "Engine/StaticMesh.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Setting the text for the static names used in the editor
const FName USlicingComponent::SocketHandleName = "SlicingHandle";
const FName USlicingComponent::SocketBladeName = "SlicingBlade";
const FName USlicingComponent::SocketCuttingExitpointName = "SlicingCuttingExitpoint";
const FName USlicingComponent::TagCuttable = "Cuttable";

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
	Parent = (UStaticMeshComponent*)(this->GetAttachmentRoot());
}

void USlicingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bIsCutting) return;
	//* Needed for the debug option booleans
	FSlicingLogicModule& SlicingLogicModule =
		FModuleManager::Get().LoadModuleChecked<FSlicingLogicModule>("SlicingLogic");

	if (SlicingLogicModule.bEnableDebugShowPlane)
	{
		DrawDebugBox(this->GetWorld(), this->GetComponentLocation(), this->GetScaledBoxExtent(), this->GetComponentRotation().Quaternion(), FColor::Green, true, 0.01f);
		TArray<USceneComponent*> Parents;
		this->GetParentComponents(Parents);
		DrawDebugSolidPlane(this->GetWorld(), FPlane(this->GetAttachmentRoot()->GetComponentLocation(), this->GetUpVector()),
			Parents[0]->GetSocketLocation(SocketBladeName), FVector2D(5, 5), FColor::Red, false, 0.01f);

		// TODO: DEBUG ENTRANCE POINT
		//DrawDebugBox(this->GetWorld(), UKismetMathLibrary::TransformLocation(OComponent->GetComponentTransform(), FVector(0,0,0)), FVector(4, 4, 4), FColor::Blue, true, 1.0f);

		DrawDebugBox(this->GetWorld(),
			UKismetMathLibrary::TransformLocation(OComponent->GetComponentTransform(), relLocation), 
			FVector(3, 3, 3), OComponent->GetComponentQuat(), FColor::Green, true, 1.0F);
	}

	if (SlicingLogicModule.bEnableDebugConsoleOutput)
	{
		UE_LOG(LogTemp, Warning, TEXT("CONSOLE OUTPUT WORKS"));
	}

	if (SlicingLogicModule.bEnableDebugShowTrajectory)
	{
		TArray<USceneComponent*> Parents;
		this->GetParentComponents(Parents);
		DrawDebugSolidPlane(this->GetWorld(), FPlane(this->GetAttachmentRoot()->GetComponentLocation(), this->GetUpVector()),
			Parents[0]->GetSocketLocation(SocketBladeName), FVector2D(5, 5), FColor::Blue, false, 1.0f);
	}
}

void USlicingComponent::OnBladeBeginOverlap(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherComp->ComponentHasTag(TagCuttable) || bIsCutting)
	{
		return;
	}
	//relLocation = OtherComp->GetComponentTransform().GetRelativeTransform(this->GetComponentTransform());
	
	/*
	Converting the given Component to Procedural Mesh Component
	*/
	UPrimitiveComponent* ReferencedComponent = OtherComp;
	Parent->SetCollisionProfileName(FName("OverlapAll"));
	bIsCutting = true;
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
			bIsCutting = false;
			ReferencedComponent->DestroyComponent();
			return;
		}
		OComponent = (UProceduralMeshComponent*) OtherComp;
	}
}

void USlicingComponent::OnBladeEndOverlap(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bIsCutting) 
	{
		return;
	}
	if (OverlappedComp->OverlapComponent(UKismetMathLibrary::TransformLocation(OComponent->GetComponentTransform(), relLocation),
		OComponent->GetComponentQuat(),
		OverlappedComp->GetCollisionShape())) 
	{
		//UStaticMeshComponent* Parent = (UStaticMeshComponent*)(this->GetAttachmentRoot());
		Parent->SetCollisionProfileName(FName("PhysicsActor"));
		bIsCutting = false;

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

	//UStaticMeshComponent* Parent = (UStaticMeshComponent*)(this->GetAttachmentRoot());
	Parent->SetCollisionProfileName(FName("PhysicsActor"));
	bIsCutting = false;
}