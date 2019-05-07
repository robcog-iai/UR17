// Copyright 2018, Institute for Artificial Intelligence

#pragma once

#include "CoreMinimal.h"

class UProceduralMeshComponent;
class UStaticMeshComponent;
class UStaticMesh;
struct FStaticMaterial;

class FSlicingHelper
{
public:
	/**
	 * Convert a static mesh component to a procedural mesh component, while retaining the previous static materials
	 * 
	 * @param StaticMeshComponent - The staic mesh component that will be used to create the procedural mesh
	 * @param StaticMaterials - The static materials that get extracted from the original static mesh
	 * @return A reference to the newly created procedural mesh
	 */
	static UProceduralMeshComponent* ConvertStaticToProceduralMeshComponent(
		UStaticMeshComponent* StaticMeshComponent, TArray<FStaticMaterial> &StaticMaterials
	);

	/**
	* Convert a procedural mesh component to a static mesh component
	* 
	* @param ProceduralMeshComponent - The procedural mesh component that will be used to create the static mesh
	* @param StaticMaterials - The old static materials used to properly recreate the static mesh
	*/
	static AStaticMeshActor* ConvertProceduralComponentToStaticMeshActor(
		UProceduralMeshComponent* ProceduralMeshComponent, TArray<FStaticMaterial> &StaticMaterials
	);

	/**
	 * Get a specific slicing component from a static mesh component
	 * 
	 * @param SlicingObject - The static mesh component on which the slicing components are attached to
	 * @return The first found slicing component of the specified type
	 */
	template<class ComponentType>
	static FORCEINLINE ComponentType* GetSlicingComponent(UStaticMeshComponent* SlicingObject);

private:
	static void CorrectProperties(UPrimitiveComponent* NewComponent, UPrimitiveComponent* OldComponent);
	static UStaticMesh* GenerateStaticMesh(UProceduralMeshComponent* ProceduralMeshComponent, int32 MaterialIndex);
	static AStaticMeshActor* SpawnStaticMeshActor(UProceduralMeshComponent* ProceduralMeshComponent);
};