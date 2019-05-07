// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "SlicingBladeComponent.h"
#include "SlicingTipComponent.h"
#include "SlicingHelper.h"

#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"

#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintComponent.h"

// Called before BeginPlay()
void USlicingBladeComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

// Called when the game starts
void USlicingBladeComponent::BeginPlay()
{
	Super::BeginPlay();

	// Check for the tip component to possibly abort the cutting
	TipComponent = FSlicingHelper::GetSlicingComponent<USlicingTipComponent>(SlicingObject);

	// Create the Physics Constraint
	ConstraintOne = NewObject<UPhysicsConstraintComponent>();
	ConstraintOne->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	// Register the overlap events
	OnComponentBeginOverlap.AddDynamic(this, &USlicingBladeComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &USlicingBladeComponent::OnEndOverlap);
}

void USlicingBladeComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	// This event is only important if the other object can actually be cut or if another cut hasn't already started
	if (!OtherComp->ComponentHasTag(TagCuttable) || bIsCurrentlyCutting)
	{
		return;
	}
	// If we are trying to start cutting with the tip, the slicing process should never start
	else if (TipComponent != NULL && OtherComp == TipComponent->OverlappedComponent)
	{
		return;
	}

	// Collision should only be ignored with the currently cut object, not the objects around it
	SlicingObject->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);

	// If physics are on, the relative location and such will be seen relative to the world location
	RelativeLocationToCutComponent = OtherComp->GetComponentTransform().InverseTransformPosition(OverlappedComp->GetComponentLocation());
	RelativeRotationToCutComponent = OverlappedComp->GetComponentQuat() - OtherComp->GetComponentQuat();

	CutComponent = OtherComp;
	CutComponent->SetNotifyRigidBodyCollision(true);

	// Have to check for null, as it randomly sets it to null for no reason...
	if (CutComponent == NULL)
	{
		return;
	}

	// The cutting process has now started
	bIsCurrentlyCutting = true;
	OnBeginSlicing.Broadcast(OverlappedComp->GetAttachmentRootActor(), OverlappedComp->GetOwner(), CutComponent->GetAttachmentRootActor(), GetWorld()->GetTimeSeconds());

	// Makes the Cutting with Constraints possible, by somwehat disabling Gravity and Physics in a sense without actually deactivating them.
	CutComponent->SetLinearDamping(100.f);
	CutComponent->SetAngularDamping(100.f);

	// Called incase the CutComponent has a tag !!! TODO: Build in Security !!!
	if (CutComponent->ComponentHasTag(FName("Resistance")) && (CutComponent->ComponentTags.IndexOfByKey("Resistance") + 1))
	{
		// A value in between 0 and 100.
		int32 resistancePercentage = FCString::Atoi( *CutComponent->ComponentTags[(CutComponent->ComponentTags.IndexOfByKey("Resistance") + 1)].ToString());
		((UStaticMeshComponent*)this->GetAttachParent())->SetLinearDamping(100000000000.f * ((resistancePercentage) / 100));
	}

	SetUpConstrains(CutComponent);
}

void USlicingBladeComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// The slicing should only happen if you are actually in the cutting process
	if (!bIsCurrentlyCutting)
	{
		ResetResistance();
		ResetState();
		return;
	}
	// If you are touching and exiting another object while cutting, ignore the event
	else if (OtherComp != CutComponent)
	{
		return;
	}
	// If the SlicingObject is pulled out, the cutting should not be continued
	else if (TipComponent != NULL && TipComponent->bEnteredCurrentlyCutObject)
	{
		// Broadcast end of sliing attempt
		OnEndSlicingFail.Broadcast(OverlappedComp->GetAttachmentRootActor(), CutComponent->GetAttachmentRootActor(), GetWorld()->GetTimeSeconds());
		ResetResistance();
		ResetState();
		return;
	}

	// Abort the cutting if you stop cutting at the same point you started at
	FVector CutComponentPosition =
		UKismetMathLibrary::TransformLocation(CutComponent->GetComponentTransform(), RelativeLocationToCutComponent);
	if (OverlappedComp->OverlapComponent(CutComponentPosition, CutComponent->GetComponentQuat(), OverlappedComp->GetCollisionShape()))
	{
		// Broadcast end of slicing attempt
		OnEndSlicingFail.Broadcast(OverlappedComp->GetAttachmentRootActor(), CutComponent->GetAttachmentRootActor(), GetWorld()->GetTimeSeconds());
		ResetResistance();
		ResetState();
		return;
	}

	ResetResistance();

	// After everything is checked, the actual slicing happens here
	SliceComponent(OtherComp);

	ResetState();
}

void USlicingBladeComponent::SliceComponent(UPrimitiveComponent* CuttableComponent)
{
	TArray<FStaticMaterial> ComponentMaterials;
	UMaterialInterface* InsideCutMaterialInterface = nullptr;
	
	// In case the component is a StaticMeshComponent it needs to be converted into a ProceduralMeshComponent
	if (CuttableComponent->IsA(UStaticMeshComponent::StaticClass()))
	{
		CuttableComponent =	FSlicingHelper::ConvertStaticToProceduralMeshComponent(
			(UStaticMeshComponent*)CuttableComponent, ComponentMaterials
		);
	}

	// Check the Materials for the cut material
	for (int index = 0; index < ComponentMaterials.Num(); index++)
	{
		FStaticMaterial Material = ComponentMaterials[index];

		if (Material.MaterialSlotName.Compare(FName("InsideCutMaterial")) == 0)
		{
			// Found the needed material, do not need to search further
			InsideCutMaterialInterface = Material.MaterialInterface;
			break;
		}
	}
	
	UProceduralMeshComponent* OutputProceduralMesh;
	UKismetProceduralMeshLibrary::SliceProceduralMesh(
		(UProceduralMeshComponent*)CuttableComponent,
		SlicingObject->GetSocketLocation(SocketBladeName),
		SlicingObject->GetUpVector(),
		true,
		OutputProceduralMesh,
		EProcMeshSliceCapOption::CreateNewSectionForCap,
		InsideCutMaterialInterface
	);
	OutputProceduralMesh->SetGenerateOverlapEvents(true);
	OutputProceduralMesh->SetEnableGravity(true);
	OutputProceduralMesh->SetSimulatePhysics(true);
	OutputProceduralMesh->ComponentTags = CuttableComponent->ComponentTags;
	OutputProceduralMesh->SetLinearDamping(0.f);
	OutputProceduralMesh->SetAngularDamping(0.f);

	CuttableComponent->SetLinearDamping(0.f);
	CuttableComponent->SetAngularDamping(0.f);

	// Convert both seperated procedural meshes into static meshes for best compatibility
	auto transformedObject = FSlicingHelper::ConvertProceduralComponentToStaticMeshActor((UProceduralMeshComponent*)CuttableComponent,
		ComponentMaterials);
	auto newSlice = FSlicingHelper::ConvertProceduralComponentToStaticMeshActor(OutputProceduralMesh, ComponentMaterials);

	// Broadcat creation of Slice and transformed object
	OnObjectCreation.Broadcast(transformedObject, newSlice, GetWorld()->GetTimeSeconds());
	
	// Broadcast Successful slicing
	OnEndSlicingSuccess.Broadcast(SlicingObject->GetAttachmentRootActor(), CutComponent->GetAttachmentRootActor(),
								  newSlice, GetWorld()->GetTimeSeconds());

	// Delete old original static mesh
	CutComponent->GetOwner()->Destroy();

	// Broadcast destruction of object
	OnObjectDestruction.Broadcast(CutComponent->GetAttachmentRootActor(), GetWorld()->GetTimeSeconds());

}

// Resets everything to the state the component was in before the dampening was set
void USlicingBladeComponent::ResetResistance()
{
	// Reset the dampening
	if (CutComponent)
	{
		CutComponent->SetLinearDamping(0.f);
		CutComponent->SetAngularDamping(0.f);
	}
	SlicingObject->SetLinearDamping(0.f);
	SlicingObject->SetAngularDamping(0.f);
}

// Resets everything to the state the component was in before the cutting-process began
void USlicingBladeComponent::ResetState()
{
	bIsCurrentlyCutting = false;
	CutComponent = nullptr;

	ConstraintOne->BreakConstraint();

	// Collision should turn back to normal again
	SlicingObject->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Block);
}

// Connects the given Component, normally the CuttableComponent, with either the Blade OR the Hand if it's welded.
void USlicingBladeComponent::SetUpConstrains(UPrimitiveComponent* CuttableComponent)
{
	// Connect the CuttableObject and Blade/Welded Hand as bones with the Constraint
	ConstraintOne->SetConstrainedComponents(this, FName("Blade"), CuttableComponent, FName("Object"));

	// High number may not be needed. Adjust
	ConstraintOne->ConstraintInstance.SetAngularBreakable(false, 10000.f);
	ConstraintOne->ConstraintInstance.SetLinearBreakable(false, 10000.f);
	ConstraintOne->ConstraintInstance.SetLinearDriveParams(1000.f, 1000.f, 1000.f);
	
	// It's enough here to set Limited, Locked is too limiting for the purpose of realism
	ConstraintOne->ConstraintInstance.SetLinearXLimit(ELinearConstraintMotion::LCM_Free, 1.f);
	ConstraintOne->ConstraintInstance.SetLinearYLimit(ELinearConstraintMotion::LCM_Free, 1.f);
	ConstraintOne->ConstraintInstance.SetLinearZLimit(ELinearConstraintMotion::LCM_Limited, 0.1f);

	// IT's enough here to just set Swing2 and Twist
	ConstraintOne->ConstraintInstance.SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Free, 1.f);
	ConstraintOne->ConstraintInstance.SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, 1.f);
	ConstraintOne->ConstraintInstance.SetAngularTwistLimit(EAngularConstraintMotion::ACM_Limited, 1.f);

	// May not be needed
	ConstraintOne->SetLinearPositionDrive(false, false, true);
	ConstraintOne->SetAngularVelocityDrive(true, true);

	// Deactivates that the parent isn't affected by the constraint, which in our case is the knife
	ConstraintOne->ConstraintInstance.DisableParentDominates();

	ConstraintOne->UpdateConstraintFrames();
}