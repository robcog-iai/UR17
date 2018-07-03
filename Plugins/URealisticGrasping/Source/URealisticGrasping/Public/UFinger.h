// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "UFinger.generated.h"

//This Enum represents all possible types for a finger
UENUM(BlueprintType)
enum class ETypeOfFinger : uint8
{
	Thumb		UMETA(DisplayName = "Thumb"),
	Index		UMETA(DisplayName = "Index"),
	Middle		UMETA(DisplayName = "Middle"),
	Ring		UMETA(DisplayName = "Ring"),
	Pinky		UMETA(DisplayName = "Pinky"),
	None		UMETA(DisplayName = "None")
};

//This Enum shows what kind of bone it is
UENUM(BlueprintType)
enum class ETypeOfBone : uint8
{
	Metacarpal		UMETA(DisplayName = "Metacarpal"),
	Proximal		UMETA(DisplayName = "Proximal"),
	Intermediate	UMETA(DisplayName = "Intermediate"),
	Distal			UMETA(DisplayName = "Distal"),
	None            UMETA(DisplayName = "None")
};

//This Enum represents what hand it is
UENUM(BlueprintType)
enum class ETypeOfHand : uint8
{
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right"),
	None		UMETA(DisplayName = "None")
};

//This struct represents a finger with all its bones and their orientations.
USTRUCT()
struct FFingerData
{
	GENERATED_USTRUCT_BODY()
public:
	//Standard constructor
	FFingerData()
	{
		FingerParts = TMap<FString, FRotator>();
	}

	/*
	This function adds a new bone and its Orientation
	@param Bone, the bone name
	@param Rotation, the orientation for this bone
	*/
	void AddData(FString Bone, FRotator Rotation)
	{
		FingerParts.Add(Bone, Rotation);
	}

	/*
	This function return the orientation for an specific bone
	@param Bone, the bone name
	@return the orientation for this bone
	*/
	FRotator* GetFingerRotator(FString Bone)
	{
		return FingerParts.Find(Bone);
	}

	/*
	This function returns the bone/orientation map
	@return the bone/orientation map
	*/
	TMap<FString, FRotator>* GetFingerMap()
	{
		return &FingerParts;
	}

private:
	//Bone/orientation map
	TMap<FString, FRotator> FingerParts;
};


//This struct represents one step with all finger datas
USTRUCT()
struct FFingersData
{
	GENERATED_USTRUCT_BODY()
public:
	//Standard constructor
	FFingersData()
	{
		PositionData = TMap<ETypeOfFinger, FFingerData>();
	}

	/*
	Constructor to set all finger datas directly
	@param TumbData, the tumb bone/orientation datas
	@param IndexData, the index bone/orientation datas
	@param MiddleData, the middle bone/orientation datas
	@param RingData, the ring bone/orientation datas
	@param PinkyData, the pinky bone/orientation datas
	*/
	FFingersData(FFingerData TumbData, FFingerData IndexData, FFingerData MiddleData, FFingerData RingData, FFingerData PinkyData)
	{
		SetAllData(TumbData, IndexData, MiddleData, RingData, PinkyData);
	}

	/*
	function to set all finger datas
	@param TumbData, the tumb bone/orientation datas
	@param IndexData, the index bone/orientation datas
	@param MiddleData, the middle bone/orientation datas
	@param RingData, the ring bone/orientation datas
	@param PinkyData, the pinky bone/orientation datas
	*/
	void SetAllData(FFingerData TumbData, FFingerData IndexData, FFingerData MiddleData, FFingerData RingData, FFingerData PinkyData)
	{
		PositionData = TMap<ETypeOfFinger, FFingerData>();
		PositionData.Add(ETypeOfFinger::Thumb, TumbData);
		PositionData.Add(ETypeOfFinger::Index, IndexData);
		PositionData.Add(ETypeOfFinger::Middle , MiddleData);
		PositionData.Add(ETypeOfFinger::Ring, RingData);
		PositionData.Add(ETypeOfFinger::Pinky, PinkyData);

	}

	/*
	Function for setting one data for a finger
	@param Type, the type of this finger e.g thumb
	@param Data, the bone/orientation data for this finger
	*/
	void AddNewFingerData(ETypeOfFinger Type, FFingerData Data)
	{
		PositionData.Add(Type, Data);
	}

	/*
	Function to return one finger data
	@param Type, the finger type from which the data should be returned
	@return the finger data or null
	*/
	FFingerData* GetFingerData(ETypeOfFinger Type)
	{
		return PositionData.Find(Type);
	}

	/*
	Function that returns the complete map with all fingers
	and their datas.
	@return the finger/data map
	*/
	TMap<ETypeOfFinger, FFingerData>* GetMap()
	{
		return &PositionData;
	}

private:
	//map with all data for all fingers
	TMap<ETypeOfFinger, FFingerData> PositionData;

};

//This struct hold every general Informations for every bone in one animation 
USTRUCT()
struct FBoneFingerTypeNames
{
	GENERATED_USTRUCT_BODY()
public:
	FBoneFingerTypeNames()
	{}

	//The Bone name
	UPROPERTY()
		FString BoneName;

	//Indicator to which finger the bone belongs
	UPROPERTY()
		ETypeOfFinger FingerType;

	//Indicator what type of bone this is
	UPROPERTY()
		ETypeOfBone BoneType;

};

//This struct represents for one Animation. It holds the data for every step 
//and also some general inforamtions
USTRUCT()
struct FHandAnimationData
{
	GENERATED_USTRUCT_BODY()
public:
	FHandAnimationData()
	{
		BoneFingerNames = TArray<FBoneFingerTypeNames>();
		PositionEpisode = TArray<FFingersData>();
	}

	/*
	This function adds a new step
	@param Data, the new step
	*/
	void AddNewPositionData(FFingersData Data)
	{
		PositionEpisode.Add(Data);
	}

	/*
	This function returns the ammount of steps in this animation
	@return the ammount of steps for this animation
	*/
	int GetNumberOfEpisodes()
	{
		return PositionEpisode.Num();
	}

	/*
	This function returns one step for a specific index
	@param Index, the step taht sould be returned
	@return the step 
	*/
	FFingersData GetPositionDataWithIndex(int Index)
	{
		if (Index >= PositionEpisode.Num() || Index < 0) return FFingersData();
		return PositionEpisode[Index];
	}

	/*
	This function add a new step to the animation (helper function)
	@param EpisondeMap, this map should contain all bones and their orientations
	@return true if the new step was added, false if their are one bone that didn't 
				exist in the bone informations
	*/
	bool AddNewEpisode(TMap<FString, FRotator> EpisodeMap)
	{
		FFingersData EpisodeData;
		EpisodeData.AddNewFingerData(ETypeOfFinger::Index, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Middle, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Pinky, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Ring, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Thumb, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::None, FFingerData());
		for (auto& Elem : EpisodeMap)
		{
			//Check if this bone actually exists
			FBoneFingerTypeNames* CurrentFinger = GetFingerInformation(Elem.Key);
			if (!CurrentFinger)
			{
				return false;
			}
			EpisodeData.GetFingerData(CurrentFinger->FingerType)->AddData(Elem.Key, Elem.Value);
		}
		PositionEpisode.Add(EpisodeData);
		return true;
	}

	/*
	This function returns a general finger information data for one bone
	@param Bone, the bone name for which the information should be returned
	@return the bone information data or null
	*/
	FBoneFingerTypeNames* GetFingerInformation(FString Bone)
	{
		for (auto& Elem : BoneFingerNames)
		{
			if (Elem.BoneName == Bone)
			{
				return &Elem;
			}
		}
		return nullptr;
	}

	/*
	This function sets als general bone information datas
	@param BoneInfo, the Bone information datas (all bones needed that you wat to save)
	*/
	void SetNewFingerBoneInfo(TArray<FBoneFingerTypeNames> BoneInfo)
	{
		if (BoneInfo.Num() > 0)
		{
			BoneFingerNames = BoneInfo;
			PositionEpisode = TArray<FFingersData>();
		}
	}

	/*
	Function that returns all Bone informations
	@return all bone informations
	*/
	TArray<FBoneFingerTypeNames>* GetBoneInformations()
	{
		return &BoneFingerNames;
	}

	//The name for this Animation
	UPROPERTY()
		FString AnimationName;

	//The name for the used skeletal mesh 
	UPROPERTY()
		FString SkeletalName;

	//Indicator witch hand
	UPROPERTY()
		ETypeOfHand HandType;

private:

	//All Bone informations
	TArray<FBoneFingerTypeNames> BoneFingerNames;

	//All steps
	TArray<FFingersData> PositionEpisode;

};