// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "UGraspingEditorCallback.h"

#include "Core.h"
#include "Editor.h"
#include "Engine/Selection.h"
#include "ISkeletalMeshEditor.h"
#include "ISkeletalMeshEditorModule.h"
#include "ISkeletonEditor.h"
#include "ISkeletonEditorModule.h"
#include "ModuleManager.h"
#include "Runtime/Engine/Classes/Animation/SkeletalMeshActor.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Animation/Skeleton.h"
#include "PersonaModule.h"
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorManager.h"
#include "Editor/UnrealEd/Public/Toolkits/ToolkitManager.h"

#define LOCTEXT_NAMESPACE "FGraspingEditorModule"

void UGraspingEditorCallback::ShowInstructions()
{
	FMessageDialog* Instructions = new FMessageDialog();
	//Sets the title of the instruction dialog window.
	const FText InstructionsTitle =
		LOCTEXT("GraspingInstructions_Title", "Instructions to create a grasping style");

	//Creates the dialog window.
	Instructions->Debugf(
		LOCTEXT("GraspingInstructions", "some instructions"),
		&InstructionsTitle
	);
}

TMap<FName, FRotator> UGraspingEditorCallback::GetBoneRotations(USkeletalMeshComponent * SkeletalComponent)
{
	TMap<FName, FRotator> BoneRotations;
	TArray<FName> BoneNames;
	SkeletalComponent->GetBoneNames(BoneNames);
	//Gets the rotations of every bone in the given mesh and saves them in a map with their bone name as key.
	for (FName BoneName : BoneNames) {
		int index = SkeletalComponent->GetBoneIndex(BoneName);
		FTransform Transform = SkeletalComponent->GetBoneTransform(index);
		FQuat Quat = Transform.GetRotation();
		BoneRotations.Add(BoneName, Quat.Rotator());
	}
	return BoneRotations;
}


#undef LOCTEXT_NAMESPACE