// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "UAnimationDataStructure.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/LocalTimestampDirectoryVisitor.h"
#include "CoreMinimal.h"

/**
 * This Class is used to read and write animation into files
 */
class UREALISTICGRASPING_API UReadWrite
{
public:
	UReadWrite();
	~UReadWrite();

	/*
	This function write a animation into a file. 
	This File can be located in: GameDir/GraspAnimations/Data.SkeletalName/Data.AnimationName.ini
	@param Data, the data that should be saved
	*/
	void WriteFile(FHandAnimationData Data);

	/*
	This function reads a ini file. It should be located in: GameDir/GraspAnimations/SkeletalMeshName/Name.ini
	@param SkeletalMeshName, this represents the directory
	@param Name, the file name in this directory
	@return the animation 
	*/
	FHandAnimationData ReadFile(FString SkeletalMeshName, FString Name);

	/*
	This function reads all posibility animations for one skeleton 
	(e.g it reads all animations in GameDir/GraspAnimations/SkeletalMeshName/...)
	@param SkeletalMeshName, the directory that should be searched
	@return all animations
	*/
	TArray<FString> ReadNames(FString SkeletalMeshName);

private:

	//File Handler
	TSharedPtr<FConfigCacheIni> ConfigFileHandler;

	//Different names for the ini file e.g default names for every episode
	const FString GeneralInformation = "GeneralInformations";
	const FString BoneSpaceAfterCalcSec = "BoneSpaceAfterCalcStep";
	const FString BoneSpaceBeforeCalcSec = "BoneSpaceBeforeCalcStep";
	const FString ComponentSpaceSec = "ComponentSpaceStep";
	const FString BoneNameSection = "BoneNames";
	const FString BoneTypeSection = "BoneToFingerTypeInformations";
	const FString BoneFingerSection = "BoneTypeInformations";
	
};
