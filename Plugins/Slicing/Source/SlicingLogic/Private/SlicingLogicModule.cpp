// Copyright 2017, Institute for Artificial Intelligence

#include "SlicingLogicModule.h"

#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

#include "DrawDebugHelpers.h"

#define LOCTEXT_NAMESPACE "FSlicingLogicModule"

void FSlicingLogicModule::StartupModule()
{
	// Empty as of right now
}

void FSlicingLogicModule::ShutdownModule()
{
	// Empty as of right now
}

void FSlicingLogicModule::SetMaterialForSection(UMaterialInterface* InputMaterial)
{
	if (InputMaterial == NULL) return;
	MaterialReferenceNewSection = InputMaterial;
}

void FSlicingLogicModule::ConvertToProceduralMeshComponent(UPrimitiveComponent* ReferencedComponent)
{
	// In case the Component is a StaticMeshComponent, uses following to make a ProceduralMeshComponent
	if (ReferencedComponent != nullptr && ReferencedComponent->GetClass() == UStaticMeshComponent::StaticClass())
	{
		if (((UStaticMeshComponent*)ReferencedComponent)->GetStaticMesh())
		{
			((UStaticMeshComponent*) ReferencedComponent)->GetStaticMesh()->bAllowCPUAccess = true;

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
		}
	}
}

void FSlicingLogicModule::CutGivenComponent(UPrimitiveComponent* InputComponent, FVector PlanePosition, FVector PlaneNormal)
{
	UProceduralMeshComponent* OutputProceduralMesh;

	/*
	Input SliceProcedural Mesh
	1. The Component which will be cut
	2. Planeposition - Start Position des schneidenen Planes
	3. PlaneNormal - Richtung in welcher die Flache Fläche zeigt
	4. CreateOtherHalf - Erstellt eine neue Hälfte nach dem Schnitt falls true
	5. ProceduralMeshCompoenent Reference auf welchen die zweite Hälfte drauf gepackt wird.
	6. EProcmeshSliceCapOption Optionen für die neuen Flächen
	7. Texturen der neuen Flächen falls richtige Option in 6
	*/
	UKismetProceduralMeshLibrary::SliceProceduralMesh(
		(UProceduralMeshComponent*)InputComponent,
		PlanePosition,
		PlaneNormal,
		true,
		OutputProceduralMesh,
		EProcMeshSliceCapOption::NoCap,
		MaterialReferenceNewSection
	);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSlicingLogicModule, Slicing)