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
	FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "\\Plugins\\URealisticGrasping\\Config\\" + Data.SkeletalName + "\\"+ Data.AnimationName +".ini";
	FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*Path);
	ConfigFileHandler->SetString(*GeneralInformation, TEXT("Name"), *Data.AnimationName, Path);
	ConfigFileHandler->SetString(*GeneralInformation, TEXT("SkeletalName"), *Data.SkeletalName, Path);

	//Save the general bone informations
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
		FFingersData CurrentData = Data.GetPositionDataWithIndex(Counter);
		FString CurrentPosition = StepName + FString::FromInt(Counter);

		//Save one step and go through all fingers
		for (auto It = CurrentData.GetMap()->CreateConstIterator(); It; ++It)
		{
			FFingerData FingerData = It->Value;

			//Go through all bones
			for (auto ItFinger = FingerData.GetFingerMap()->CreateConstIterator(); ItFinger; ++ItFinger)
			{
				ConfigFileHandler->SetRotator(*CurrentPosition, *ItFinger->Key, ItFinger->Value, Path);
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
	FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "\\Plugins\\URealisticGrasping\\Config\\" + SkeletalMeshName + "\\"+ Name + ".ini";

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

	//Read all Steps from the file
	for (int EpisodeCounter = 0; EpisodeCounter < NumberOfEpisodes; EpisodeCounter++)
	{
		//Create a new step
		FString Section = StepName + FString::FromInt(EpisodeCounter);
		FFingersData EpisodeData;
		EpisodeData.AddNewFingerData(ETypeOfFinger::Index, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Middle, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Pinky, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Ring, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Thumb, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::None, FFingerData());

		//Read every value out of the file with the bone name
		for (auto& Elem : BoneInformations)
		{
			FRotator CurrentRot;
			ConfigFileHandler->GetRotator(*Section, *Elem.BoneName, CurrentRot, Path);
			FFingerData* CurrentFingerData = EpisodeData.GetFingerData(Elem.FingerType);
			CurrentFingerData->AddData(Elem.BoneName,CurrentRot);
		}
		HandData.AddNewPositionData(EpisodeData);
	}
	return HandData;
}
//https://answers.unrealengine.com/questions/212791/how-to-get-file-list-in-a-directory.html
TArray<FString> UReadWrite::ReadNames(FString SkeletalMeshName)
{
	FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "\\Plugins\\URealisticGrasping\\Config\\" + SkeletalMeshName;
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