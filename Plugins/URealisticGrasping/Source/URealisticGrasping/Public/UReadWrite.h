// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "UFinger.h"
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
	@param Data, the data that should be saved
	*/
	void WriteFile(FHandAnimationData Data);

	/*
	This function reads a file. It should be located in: GameDir/Plugins/URealisticGrasping/Config/SkeletalMeshName/Name.ini
	@param SkeletalMeshName, this represents the directory
	@param Name, the file name in this directory
	@return the animation 
	*/
	FHandAnimationData ReadFile(FString SkeletalMeshName, FString Name);

	/*
	This function reads all posibility animations for one skeleton 
	@param SkeletalMeshName, the directory that should be searched
	@return all animations
	*/
	TArray<FString> ReadNames(FString SkeletalMeshName);

private:

	TSharedPtr<FConfigCacheIni> ConfigFileHandler;

	const FString GeneralInformation = "GeneralInformations";

	const FString StepName = "Episode";

	const FString BoneNameSection = "BoneNames";

	const FString BoneTypeSection = "BoneToFingerTypeInformations";

	const FString BoneFingerSection = "BoneTypeInformations";
	
};
