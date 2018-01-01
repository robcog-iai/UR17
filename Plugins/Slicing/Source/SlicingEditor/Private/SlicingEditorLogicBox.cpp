#include "SlicingEditorLogicBox.h"

#include "Tickable.h"

#include "SlicingEditorActionCallbacks.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

void SlicingEditorLogicBox::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void SlicingEditorLogicBox::OnBladeBeginOverlap(
	class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult & SweepResult)
{
	/*
		Converting the given Component to Procedural Mesh Component
	*/
	UPrimitiveComponent* ReferencedComponent = OtherComp;
	if (ReferencedComponent != nullptr )
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

void SlicingEditorLogicBox::OnBladeEndOverlap(
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