// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#include "UReadWrite.h"
#include "ConfigCacheIni.h"

UReadWrite::UReadWrite()
{
}

UReadWrite::~UReadWrite()
{
}

void UReadWrite::WriteFile(FHandAnimationData Data)
{
	ConfigFileHandler = MakeShareable(new FConfigCacheIni(EConfigCacheType::DiskBacked));

	//Save only when we have a valid directory name, file name, and episodes to save
	if (!ConfigFileHandler.IsValid() || Data.AnimationName.Len() == 0 || Data.SkeletalName.Len() == 0 || Data.GetNumberOfEpisodes() == 0)
	{
		return;
	}

	//Create the path and save already the animation name and the skeletal mesh name
	FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "\\GraspAnimations\\" + Data.SkeletalName + "\\"+ Data.AnimationName +".ini";
	FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*Path);
	ConfigFileHandler->SetString(*GeneralInformation, TEXT("Name"), *Data.AnimationName, Path);
	ConfigFileHandler->SetString(*GeneralInformation, TEXT("SkeletalName"), *Data.SkeletalName, Path);

	//Save the general bone informations in different sections
	int BoneCounter = 0;
	for (auto Iterator = Data.GetBoneInformations()->CreateConstIterator(); Iterator; Iterator++, BoneCounter++)
	{
		//e.g BoneName0 = thumb_1 , BoneName1 = thumb_2 ...
		FString CurrentBone = "BoneName" + FString::FromInt(BoneCounter);
		ConfigFileHandler->SetString(*BoneNameSection, *CurrentBone, *Iterator->BoneName, Path);
		ConfigFileHandler->SetInt(*BoneTypeSection, *Iterator->BoneName, static_cast<int>(Iterator->FingerType), Path);
		ConfigFileHandler->SetInt(*BoneFingerSection, *Iterator->BoneName, static_cast<int>(Iterator->BoneType), Path);
	}

	//Save the number of steps and the number of bones. They are needed to pass everything in
	int NumberOfEpisodes = Data.GetNumberOfEpisodes();
	ConfigFileHandler->SetInt(*GeneralInformation, TEXT("NumberOfEpisodes"), NumberOfEpisodes, Path);
	ConfigFileHandler->SetInt(*GeneralInformation, TEXT("NumberOfBones"), BoneCounter, Path);

	//Go through all steps and save everyting
	for (int Counter = 0; Counter < NumberOfEpisodes; Counter++)
	{
		//Get the current Data and create the current section names e.g ComponentSpaceStep0
		FHandEpisodeData CurrentData = Data.GetPositionDataWithIndex(Counter);
		FString CurrentOrientationAfterPosition = BoneSpaceAfterCalcSec + FString::FromInt(Counter);
		FString CurrentOrientationBeforPosition = BoneSpaceBeforeCalcSec + FString::FromInt(Counter);
		FString CurrentComponentSpacePosition = ComponentSpaceSec + FString::FromInt(Counter);

		//Save one step and go through all fingers
		for (auto It = CurrentData.GetMap()->CreateConstIterator(); It; ++It)
		{
			FFingerData FingerData = It->Value;

			//Go through all bones
			for (auto ItFinger = FingerData.GetFingerMap()->CreateConstIterator(); ItFinger; ++ItFinger)
			{
				//Save all three values
				ConfigFileHandler->SetRotator(*CurrentOrientationAfterPosition, *ItFinger->Key, ItFinger->Value.BoneSpaceAfterCalc, Path);
				ConfigFileHandler->SetRotator(*CurrentOrientationBeforPosition, *ItFinger->Key, ItFinger->Value.BoneSpaceBeforeCalc, Path);
				ConfigFileHandler->SetRotator(*CurrentComponentSpacePosition, *ItFinger->Key, ItFinger->Value.ComponentSpace, Path);
			}
		}
	}

	ConfigFileHandler->Flush(true, Path);

}

FHandAnimationData UReadWrite::ReadFile(FString SkeletalMeshName, FString Name)
{
	ConfigFileHandler = MakeShareable(new FConfigCacheIni(EConfigCacheType::DiskBacked));

	//Create the path only when the directory name and the file name are valid
	if (!ConfigFileHandler.IsValid() || SkeletalMeshName.Len() == 0 || Name.Len() == 0) return FHandAnimationData();
	FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "\\GraspAnimations\\" + SkeletalMeshName + "\\"+ Name + ".ini";

	//Save the Animation name and the skeletal mesh name
	FHandAnimationData HandData = FHandAnimationData();
	HandData.AnimationName = Name;
	HandData.SkeletalName = SkeletalMeshName;

	int NumberOfEpisodes = 0;
	int NumberOfBones = 0;

	//Read the number of bones and steps out of the file
	bool bSuccsses = ConfigFileHandler->GetInt(*GeneralInformation, TEXT("NumberOfEpisodes"), NumberOfEpisodes, Path);
	if (!bSuccsses)return FHandAnimationData();
	bSuccsses = ConfigFileHandler->GetInt(*GeneralInformation, TEXT("NumberOfBones"), NumberOfBones, Path);
	if (!bSuccsses)return FHandAnimationData();

	TArray<FBoneFingerTypeNames> BoneInformations = TArray<FBoneFingerTypeNames>();

	//Read all bone informations from the file
	for (int Counter = 0; Counter < NumberOfBones; Counter++)
	{
		FString CurrentBone = "BoneName" + FString::FromInt(Counter);
		FBoneFingerTypeNames CurrentFingerType;
		FString Result;

		//Get the Bone Name
		bSuccsses = ConfigFileHandler->GetString(*BoneNameSection, *CurrentBone, Result, Path);
		if (!bSuccsses)return FHandAnimationData();
		CurrentFingerType.BoneName = Result;
		int GetEnumInfo;

		//Get the type of finger information
		bSuccsses = ConfigFileHandler->GetInt(*BoneTypeSection, *Result, GetEnumInfo, Path);
		if (!bSuccsses)return FHandAnimationData();
		CurrentFingerType.FingerType = static_cast<ETypeOfFinger>(GetEnumInfo);

		//Get the type of bone information 
		bSuccsses = ConfigFileHandler->GetInt(*BoneFingerSection, *Result, GetEnumInfo, Path);
		if (!bSuccsses)return FHandAnimationData();
		CurrentFingerType.BoneType = static_cast<ETypeOfBone>(GetEnumInfo);
		BoneInformations.Add(CurrentFingerType);
	}
	HandData.SetNewFingerBoneInfo(BoneInformations);

	//Read all Episodes from the file
	for (int EpisodeCounter = 0; EpisodeCounter < NumberOfEpisodes; EpisodeCounter++)
	{
		//Create a new Episode section name e.g ComponentSpaceStep0
		FString OrientationAfterSection = BoneSpaceAfterCalcSec + FString::FromInt(EpisodeCounter);
		FString OrientationBevorSection = BoneSpaceBeforeCalcSec + FString::FromInt(EpisodeCounter);
		FString CurrentComponentSpacePosition = ComponentSpaceSec + FString::FromInt(EpisodeCounter);

		//Create a new struct for this episode
		FHandEpisodeData EpisodeData;
		EpisodeData.AddNewFingerData(ETypeOfFinger::Index, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Middle, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Pinky, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Ring, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Thumb, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::None, FFingerData());

		//Read every value out of the file with the bone name
		for (auto& Elem : BoneInformations)
		{
			//Get the bone data
			FBoneData CurrentBone;
			bSuccsses = ConfigFileHandler->GetRotator(*OrientationAfterSection, *Elem.BoneName, CurrentBone.BoneSpaceAfterCalc, Path);
			if (!bSuccsses)return FHandAnimationData();
			bSuccsses = ConfigFileHandler->GetRotator(*OrientationBevorSection, *Elem.BoneName, CurrentBone.BoneSpaceBeforeCalc, Path);
			if (!bSuccsses)return FHandAnimationData();
			bSuccsses = ConfigFileHandler->GetRotator(*CurrentComponentSpacePosition, *Elem.BoneName, CurrentBone.ComponentSpace, Path);
			if (!bSuccsses)return FHandAnimationData();

			//Add the bone data to the right finger 
			FFingerData* CurrentFingerData = EpisodeData.GetFingerData(Elem.FingerType);
			CurrentFingerData->AddData(Elem.BoneName,CurrentBone);
		}
		HandData.AddNewPositionData(EpisodeData);
	}
	return HandData;
}
//https://answers.unrealengine.com/questions/212791/how-to-get-file-list-in-a-directory.html
TArray<FString> UReadWrite::ReadNames(FString SkeletalMeshName)
{
	FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "\\GraspAnimations\\" + SkeletalMeshName;
	// Get all files in directory
	TArray<FString> DirectoriesToSkip;
	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FLocalTimestampDirectoryVisitor Visitor(PlatformFile, DirectoriesToSkip, DirectoriesToSkip, false);
	PlatformFile.IterateDirectory(*Path, Visitor);
	TArray<FString> Files = TArray<FString>();

	for (TMap<FString, FDateTime>::TIterator TimestampIt(Visitor.FileTimes); TimestampIt; ++TimestampIt)
	{
		FString FilePath = TimestampIt.Key();
		FString FileName = FPaths::GetCleanFilename(FilePath);

		//Removes the .ini
		FileName.RemoveAt(FileName.Len() - 4, 4, true);
		Files.Add(FileName);
	}
	return Files;
}