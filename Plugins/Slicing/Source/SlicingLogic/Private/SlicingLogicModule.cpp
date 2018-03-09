// Copyright 2017, Institute for Artificial Intelligence

#include "SlicingLogicModule.h"

#include "StaticMeshResources.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

#include "Engine/StaticMesh.h"

#define LOCTEXT_NAMESPACE "FSlicingLogicModule"

void FSlicingLogicModule::StartupModule()
{
	// Empty as of right now
}

void FSlicingLogicModule::ShutdownModule()
{
	// Empty as of right now
}

void FSlicingLogicModule::ConvertStaticToProceduralMeshComponent(UPrimitiveComponent* StaticMeshComponent)
{
	// Needed so that the component can be cut/changed in runtime
	((UStaticMeshComponent*)StaticMeshComponent)->GetStaticMesh()->bAllowCPUAccess = true;

	UProceduralMeshComponent* ProceduralMeshComponent = NewObject<UProceduralMeshComponent>(StaticMeshComponent);
	ProceduralMeshComponent->SetRelativeTransform(StaticMeshComponent->GetRelativeTransform());
	ProceduralMeshComponent->RegisterComponent();
	ProceduralMeshComponent->SetCollisionProfileName(FName("PhysicsActor"));
	ProceduralMeshComponent->bUseComplexAsSimpleCollision = false;
	ProceduralMeshComponent->SetEnableGravity(true);
	ProceduralMeshComponent->SetSimulatePhysics(true);
	ProceduralMeshComponent->bGenerateOverlapEvents = true;
	ProceduralMeshComponent->ComponentTags = StaticMeshComponent->ComponentTags;

	// Copies the materials from the StaticMeshComponent
	UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(
		((UStaticMeshComponent*)StaticMeshComponent), 0, ProceduralMeshComponent, true);

	// Remove the old static mesh
	StaticMeshComponent->DestroyComponent();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSlicingLogicModule, Slicing)