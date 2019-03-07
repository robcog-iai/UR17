// Copyright 2018, Institute for Artificial Intelligence

#include "SlicingHelper.h"

#include "StaticMeshResources.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "RawMesh.h"

#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/ConvexElem.h"

#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"

UProceduralMeshComponent* FSlicingHelper::ConvertStaticToProceduralMeshComponent(
	UStaticMeshComponent* StaticMeshComponent, TArray<FStaticMaterial> &StaticMaterials)
{
	// Needed so that the component can be cut/changed in runtime
	StaticMeshComponent->GetStaticMesh()->bAllowCPUAccess = true;

	UProceduralMeshComponent* ProceduralMeshComponent = NewObject<UProceduralMeshComponent>(StaticMeshComponent);
	ProceduralMeshComponent->bUseComplexAsSimpleCollision = false;
	CorrectProperties(ProceduralMeshComponent, StaticMeshComponent);

	// Copies the mesh, collision and currently used materials from the StaticMeshComponent
	UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(
		StaticMeshComponent, 0, ProceduralMeshComponent, true);

	// Give out a copy of the static material from the original component, as procedural meshes do not have one
	// and therefore lose out on information about the materials
	StaticMaterials = StaticMeshComponent->GetStaticMesh()->StaticMaterials;

	// Remove the old static mesh
	StaticMeshComponent->DestroyComponent();

	return ProceduralMeshComponent;
}

AStaticMeshActor* FSlicingHelper::ConvertProceduralComponentToStaticMeshActor(
	UProceduralMeshComponent* ProceduralMeshComponent, TArray<FStaticMaterial> &StaticMaterials)
{
	// Generate the static mesh from the data scanned from the procedural mesh
	int32 MaterialIndex = 0;
	for (FStaticMaterial Material : StaticMaterials)
	{
		MaterialIndex++;
		if (Material.MaterialSlotName.Compare(FName("InsideCutMaterial"))) break;
	}
	UStaticMesh* StaticMesh = GenerateStaticMesh(ProceduralMeshComponent, MaterialIndex);
	// Set the static materials gotten from the old static mesh
	StaticMesh->StaticMaterials = StaticMaterials;

	AStaticMeshActor* StaticMeshActor = SpawnStaticMeshActor(ProceduralMeshComponent);

	// Edit the StaticMeshComponent to have the same properties as the old ProceduralMeshComponent
	UStaticMeshComponent* NewStaticMeshComponent = StaticMeshActor->GetStaticMeshComponent();
	NewStaticMeshComponent->SetStaticMesh(StaticMesh);
	CorrectProperties(NewStaticMeshComponent, ProceduralMeshComponent);

	// Move actor's location to current mesh origin
	StaticMeshActor->SetActorLocation(ProceduralMeshComponent->Bounds.Origin);
	
	// Remove the old component
	ProceduralMeshComponent->DestroyComponent();

	return StaticMeshActor;
}

template<class ComponentType>
ComponentType* FSlicingHelper::GetSlicingComponent(UStaticMeshComponent* SlicingObject)
{
	TArray<USceneComponent*> SlicingComponents;
	SlicingObject->GetChildrenComponents(true, SlicingComponents);

	for (USceneComponent* Component : SlicingComponents)
	{
		if (ComponentType* TypedComponent = Cast<ComponentType>(Component))
		{
			// Only one slicing component of each type should exist
			return TypedComponent;
		}
	}
	return nullptr;
}

//* Sets the correct properties for the newly created component with the help of the old component
void FSlicingHelper::CorrectProperties(UPrimitiveComponent* NewComponent, UPrimitiveComponent* OldComponent)
{
	NewComponent->SetRelativeTransform(OldComponent->GetRelativeTransform());
	NewComponent->RegisterComponent();
	NewComponent->SetCollisionProfileName(FName("PhysicsActor"));
	NewComponent->SetEnableGravity(true);
	NewComponent->SetSimulatePhysics(true);
	NewComponent->SetGenerateOverlapEvents(true);
	NewComponent->ComponentTags = OldComponent->ComponentTags;
}

UStaticMesh* FSlicingHelper::GenerateStaticMesh(UProceduralMeshComponent* ProceduralMeshComponent, int32 MaterialIndex)
{
	///																   ///
	/// COPIED OVER FROM "ProceduralMeshComponentDetails.cpp l.118-212 ///
	///																   ///
	// Raw mesh data we are filling in
	FRawMesh RawMesh;
	// Materials to apply to new mesh
	TArray<UMaterialInterface*> MeshMaterials;

	// Current Mesh Origin
	FVector Origin = ProceduralMeshComponent->Bounds.Origin;
	FVector OriginLocal = ProceduralMeshComponent->GetComponentTransform().InverseTransformPosition(Origin);

	const int32 NumSections = ProceduralMeshComponent->GetNumSections();
	int32 VertexBase = 0;
	for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
	{
		FProcMeshSection* ProcSection = ProceduralMeshComponent->GetProcMeshSection(SectionIdx);

		// Copy verts
		for (FProcMeshVertex& Vert : ProcSection->ProcVertexBuffer)
		{
			// Move mesh vertices from current origin to Actor's location (Remove offset)
			Vert.Position -= OriginLocal;
			RawMesh.VertexPositions.Add(Vert.Position);
		}

		// Copy 'wedge' info
		int32 NumIndices = ProcSection->ProcIndexBuffer.Num();
		for (int32 IndexIdx = 0; IndexIdx < NumIndices; IndexIdx++)
		{
			int32 Index = ProcSection->ProcIndexBuffer[IndexIdx];

			RawMesh.WedgeIndices.Add(Index + VertexBase);

			FProcMeshVertex& ProcVertex = ProcSection->ProcVertexBuffer[Index];

			FVector TangentX = ProcVertex.Tangent.TangentX;
			FVector TangentZ = ProcVertex.Normal;
			FVector TangentY = (TangentX ^ TangentZ).GetSafeNormal() * (ProcVertex.Tangent.bFlipTangentY ? -1.f : 1.f);

			RawMesh.WedgeTangentX.Add(TangentX);
			RawMesh.WedgeTangentY.Add(TangentY);
			RawMesh.WedgeTangentZ.Add(TangentZ);

			RawMesh.WedgeTexCoords[0].Add(ProcVertex.UV0);
			RawMesh.WedgeColors.Add(ProcVertex.Color);
		}

		// copy face info
		// Current Goal: The given InnerMaterial Index.
		int32 NumTris = NumIndices / 3;
		for (int32 TriIdx = 0; TriIdx < NumTris; TriIdx++)
		{
			if (SectionIdx > 0)
			{
				RawMesh.FaceMaterialIndices.Add(MaterialIndex);
			}
			else
			{
				RawMesh.FaceMaterialIndices.Add(0);
			}

			RawMesh.FaceSmoothingMasks.Add(0); // Assume this is ignored as bRecomputeNormals is false
		}

		// Remember material
		MeshMaterials.Add(ProceduralMeshComponent->GetMaterial(SectionIdx));

		// Update offset for creating one big index/vertex buffer
		VertexBase += ProcSection->ProcVertexBuffer.Num();
	}

	// The new StaticMesh that is going to be filled with the scanned info
	UStaticMesh* StaticMesh = NewObject<UStaticMesh>();

	// If we got some valid data build the new static mesh
	if (RawMesh.VertexPositions.Num() >= 3 && RawMesh.WedgeIndices.Num() >= 3)
	{
		StaticMesh->InitResources();

		StaticMesh->LightingGuid = FGuid::NewGuid();

		// Add source to new StaticMesh
		FStaticMeshSourceModel* SrcModel = new (StaticMesh->SourceModels) FStaticMeshSourceModel();
		SrcModel->BuildSettings.bRecomputeNormals = false;
		SrcModel->BuildSettings.bRecomputeTangents = false;
		SrcModel->BuildSettings.bRemoveDegenerates = false;
		SrcModel->BuildSettings.bUseHighPrecisionTangentBasis = false;
		SrcModel->BuildSettings.bUseFullPrecisionUVs = false;
		SrcModel->BuildSettings.bGenerateLightmapUVs = true;
		SrcModel->BuildSettings.SrcLightmapIndex = 0;
		SrcModel->BuildSettings.DstLightmapIndex = 1;
		SrcModel->RawMeshBulkData->SaveRawMesh(RawMesh);

		// Copy materials to new mesh
		for (UMaterialInterface* Material : MeshMaterials)
		{
			StaticMesh->StaticMaterials.Add(FStaticMaterial(Material));
		}

		//Set the Imported version before calling the build
		StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;

		// Build mesh from source
		StaticMesh->Build();
		StaticMesh->PostEditChange();

		///
		/// END OF COPY
		///
		
		// Get the correct (simple) collision from the procedural mesh
		FKAggregateGeom CollGeom = ProceduralMeshComponent->GetBodySetup()->AggGeom;
		
		// Move collision vertices from current origin to Actor's location (Remove offset)
		int32 totalElems = CollGeom.ConvexElems.Num();
		for (int32 idx = 0; idx < totalElems; idx++) {

			TArray<FVector> *vertexs = &CollGeom.ConvexElems[idx].VertexData;
			int32 totalVertexs = vertexs->Num();
			for (int32 v_idx = 0; v_idx < totalVertexs; v_idx++) {
				(*vertexs)[v_idx] -= OriginLocal;
			}
		}
	
		StaticMesh->BodySetup->AddCollisionFrom(CollGeom);
		StaticMesh->Build();
		StaticMesh->PostEditChange();
	}

	return StaticMesh;
}

AStaticMeshActor* FSlicingHelper::SpawnStaticMeshActor(UProceduralMeshComponent* ProceduralMeshComponent)
{
	FVector Location = ProceduralMeshComponent->GetAttachmentRootActor()->GetActorLocation();
	FRotator Rotation = ProceduralMeshComponent->GetAttachmentRootActor()->GetActorRotation();
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Template = ProceduralMeshComponent->GetAttachmentRootActor();
	AStaticMeshActor* StaticMeshActor =
		ProceduralMeshComponent->GetWorld()->SpawnActor<AStaticMeshActor>(Location, Rotation, SpawnInfo);

	return StaticMeshActor;
}