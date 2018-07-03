// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"

/**
 * 
 */
class UGraspingEditorCallback
{
public:
	//Should show an instruction dialog on how to create a grasp.
	static void ShowInstructions();

	//Creates a map with the names of the bones as FName and their current rotations as FRotator for a given USkeletalMeshComponent.
	static TMap<FName, FRotator> GetBoneRotations(USkeletalMeshComponent*  SkeletalComponent);
};