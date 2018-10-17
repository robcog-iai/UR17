// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "UAnimationDataStructure.generated.h"

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

//This Struct represents one bone with all their values
USTRUCT()
struct FBoneData
{
	GENERATED_USTRUCT_BODY()
public:
	//Standard constructor
	FBoneData(){}

	/* 
	Constructor to set only the bonespace after calculation value
	@param BoneSpaceAfterCalc, the bonespace rotation after the calculation
	*/
	FBoneData(FRotator BoneSpaceAfterCalc)
	{
		this->BoneSpaceAfterCalc = BoneSpaceAfterCalc;
	}

	/*
	Constructor to set all three values
	@param BoneSpaceAfterCalc, the bonespace rotation after the calculation
	@param BoneSpaceBeforeCalc, the bonespace rotation before the calculation
	@param ComponentSpace, the componentspace rotation
	*/
	FBoneData(FRotator BoneSpaceAfterCalc, FRotator BoneSpaceBeforeCalc, FRotator ComponentSpace)
	{
		this->BoneSpaceAfterCalc = BoneSpaceAfterCalc;
		this->BoneSpaceBeforeCalc = BoneSpaceBeforeCalc;
		this->ComponentSpace = ComponentSpace;
	}

	//Create the operator == to check for equality
	FORCEINLINE bool operator==(const FBoneData &arg1) const
	{
		//Checks if the values are equal
		if (arg1.BoneSpaceBeforeCalc == BoneSpaceBeforeCalc && arg1.BoneSpaceAfterCalc == BoneSpaceAfterCalc 
			&& arg1.ComponentSpace == ComponentSpace) return true;
		return false;
	}

	//The bonespace rotation after the calculation
	FRotator BoneSpaceAfterCalc;

	//The bonespace rotation before the calculation
	FRotator BoneSpaceBeforeCalc;

	//The componentspace rotation
	FRotator ComponentSpace;

};

//This struct represents a finger with all its bone datas
USTRUCT()
struct FFingerData
{
	GENERATED_USTRUCT_BODY()
public:
	//Standard constructor
	FFingerData()
	{
		FingerParts = TMap<FString, FBoneData>();
	}

	FORCEINLINE bool operator==(const FFingerData &arg1) const
	{
		//Checks if both FFingerData structs have the same nummber of bones
		if (arg1.FingerParts.Num() != FingerParts.Num()) return false;

		//Go through all bones and check if every bone also exist in the other struct
		for (auto Elem : FingerParts)
		{
			//Checks if the bonename exist in the other struct
			bool bContains = arg1.FingerParts.Contains(Elem.Key);
			if (!bContains) return false;

			//Checks for equality
			const FBoneData ConstData = Elem.Value;
			bContains = *arg1.FingerParts.Find(Elem.Key) == ConstData;
			if (!bContains) return false;
		}
		return true;
	}

	/*
	This function adds a new bone and its bonedata to the map
	@param Bone, the bone name
	@param BoneData, the bonedata for this bone
	*/
	void AddData(FString Bone, FBoneData BoneData)
	{
		FingerParts.Add(Bone, BoneData);
	}

	/*
	This function return the bonedata for an specific bone
	@param Bone, the bone name
	@return the bonedata for this bone
	*/
	FBoneData* GetFingerRotator(FString Bone)
	{
		return FingerParts.Find(Bone);
	}

	/*
	This function returns the bone/bonedata map
	@return the bonename/bonedata map
	*/
	TMap<FString, FBoneData>* GetFingerMap()
	{
		return &FingerParts;
	}

	/*
	This function creates a new bonedata and add this to the map
	@param BoneName, the bone name
	@param BoneSpaceAfterCalc, the bonespace rotation after the calculation
	@param BoneSpaceBeforeCalc, the bonespace rotation before the calculation
	@param ComponentSpace, the componentspace rotation
	*/
	void AddNewBoneData(FString BoneName, FRotator OrientationAfterCalc, 
		FRotator OrientationBeforCalc, FRotator ComponentSpace)
	{
		FBoneData NewData = FBoneData(OrientationAfterCalc, OrientationBeforCalc, ComponentSpace);
		FingerParts.Add(BoneName, NewData);
	}

private:
	//Bone/bonedata map
	TMap<FString, FBoneData> FingerParts;


};


//This structure represents one Episode with all their finger datas
USTRUCT()
struct FHandEpisodeData
{
	GENERATED_USTRUCT_BODY()
public:
	//Standard constructor
	FHandEpisodeData()
	{
		PositionData = TMap<ETypeOfFinger, FFingerData>();
	}

	//Checks for equality
	FORCEINLINE bool operator==(const FHandEpisodeData &arg1) const
	{
		//Go through all FingerDatas and check for equality
		for (auto Elem : PositionData)
		{
			//Checks if the other struct contains data for the current key
			bool bContains = arg1.PositionData.Contains(Elem.Key);
			if (!bContains) return false;

			//Checks for equality
			const FFingerData ConstData = Elem.Value;
			bContains = *arg1.PositionData.Find(Elem.Key) == ConstData;
			if (!bContains) return false;
		}
		return true;
	}

	/*
	Constructor to set all finger datas directly
	@param TumbData, the tumb bone/orientation datas
	@param IndexData, the index bone/orientation datas
	@param MiddleData, the middle bone/orientation datas
	@param RingData, the ring bone/orientation datas
	@param PinkyData, the pinky bone/orientation datas
	*/
	FHandEpisodeData(FFingerData TumbData, FFingerData IndexData, 
		FFingerData MiddleData, FFingerData RingData, FFingerData PinkyData)
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
	void SetAllData(FFingerData TumbData, FFingerData IndexData, 
		FFingerData MiddleData, FFingerData RingData, FFingerData PinkyData)
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

//This struct hold every general Informations for every bone 
//(e.g to witch finger this bone belongs)
USTRUCT()
struct FBoneFingerTypeNames
{
	GENERATED_USTRUCT_BODY()
public:

	//Standard constructor
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

//This struct represents one Animation. It holds the data for every episode 
//and also some general inforamtions
USTRUCT()
struct FHandAnimationData
{
	GENERATED_USTRUCT_BODY()
public:

	//Standard constructor
	FHandAnimationData()
	{
		BoneFingerNames = TArray<FBoneFingerTypeNames>();
		PositionEpisode = TArray<FHandEpisodeData>();
	}

	/*
	This function adds a new episode
	@param Data, the new episode
	*/
	void AddNewPositionData(FHandEpisodeData Data)
	{
		PositionEpisode.Add(Data);
	}

	/*
	This function returns the number of episodes in this animation
	@return the ammount of episodes for this animation
	*/
	int GetNumberOfEpisodes()
	{
		return PositionEpisode.Num();
	}

	/*
	This function replaces one Episode with another one
	@param OldData, the episode that should be replaced
	@param NewData, the new episode
	@return, @true when the old Data was replaced, @false if not
	*/
	bool ReplaceEpisode(const FHandEpisodeData OldData, const FHandEpisodeData NewData)
	{
		int32 Index = RemoveEpisode(OldData);
		if (Index < 0) return false;
		int32 IndexNew = PositionEpisode.Insert(NewData, Index);

		//Checks if it was added to the right position
		if (Index == IndexNew) return true;
		return false;

	}

	/*
	This function replaces one episode with another one. He also creates a new struct out of the map
	@param RemoveIndex, the episode that should be replaced
	@param BoneData, the new episode as a map
	@return, @true when the old Data was replaced, @false if not
	*/
	bool ReplaceEpisode(int32 RemoveIndex, TMap<FString, FBoneData> BoneData)
	{
		//Checks if the index is valid and also if the number of bones that are saved in BoneFingerNames
		//are equal with the new map (if not then there are missing bones)
		if (RemoveIndex < GetNumberOfEpisodes() && BoneData.Num() != BoneFingerNames.Num()) return false;
		PositionEpisode.RemoveAt(RemoveIndex);
		return AddOneEpisode(BoneData, RemoveIndex);
	}

	/*
	This function removes one episode
	@param OldData, the data that should be removed
	@return the position of the removed struct or -1
	*/
	int32 RemoveEpisode(FHandEpisodeData OldData)
	{
		//Checks if the data exists 
		if (!PositionEpisode.Contains(OldData)) return -1;

		int32 Index = PositionEpisode.IndexOfByKey(OldData);
		PositionEpisode.RemoveAt(Index);
		return Index;
	}

	/*
	This function returns one episode for a specific index
	@param Index, the episode taht sould be returned
	@return the episode 
	*/
	FHandEpisodeData GetPositionDataWithIndex(int Index)
	{
		if (Index >= PositionEpisode.Num() || Index < 0) return FHandEpisodeData();
		return PositionEpisode[Index];
	}

	/*
	Adds a new episode
	@param BoneData, a map that containts for every bone the current bonedata
	@return @true if the episode was added successfully @false if not
	*/
	bool AddNewEpisode(TMap<FString, FBoneData> BoneData)
	{
		/*
		checks if the number of bones that are saved in BoneFingerNames
		are equal with the new map (if not then there are missing bones)
		*/
		if (BoneData.Num() != BoneFingerNames.Num()) return false;
		return AddOneEpisode(BoneData, -1);
	}

	/*
	This function returns the general finger information data for one bone
	@param Bone, the bone name for which the information should be returned
	@return the bone information data or null
	*/
	FBoneFingerTypeNames* GetFingerInformation(FString Bone)
	{
		//Go through all bone informations
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
	This function sets the general bone information datas
	@param BoneInfo, the Bone information datas (all bones needed that you wat to save)
	*/
	void SetNewFingerBoneInfo(TArray<FBoneFingerTypeNames> BoneInfo)
	{
		if (BoneInfo.Num() > 0)
		{
			BoneFingerNames = BoneInfo;

			//Reset all episodes
			PositionEpisode = TArray<FHandEpisodeData>();
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

	/*
	Function that returns all used bone names for this animation
	@return all bone names
	*/
	TArray<FString> GetAllBoneNames()
	{
		TArray<FString> BoneNames = TArray<FString>();

		//Go through all bone informations an collect all names
		for (auto Elem : BoneFingerNames)
		{
			BoneNames.Add(Elem.BoneName);
		}
		return BoneNames;
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

	//helper function to add a new episode
	bool AddOneEpisode(TMap<FString, FBoneData> BoneData, int32 Index)
	{
		//Create a new episode for all fingers
		FHandEpisodeData EpisodeData = FHandEpisodeData();
		EpisodeData.AddNewFingerData(ETypeOfFinger::Index, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Middle, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Pinky, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Ring, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::Thumb, FFingerData());
		EpisodeData.AddNewFingerData(ETypeOfFinger::None, FFingerData());

		//Go through all bonedatas
		for (auto& Elem : BoneData)
		{
			//Check if this bone actually exists
			FBoneFingerTypeNames* CurrentFinger = GetFingerInformation(Elem.Key);
			if (!CurrentFinger) return false;
			EpisodeData.GetFingerData(CurrentFinger->FingerType)->AddData(Elem.Key, Elem.Value);
		}

		//If the Index is not -1 insert this episode to this position
		if (Index >= 0 && Index <= PositionEpisode.Num())
		{
			PositionEpisode.Insert(EpisodeData,Index);
		}
		else
		{
			//Add the episode to the end
			PositionEpisode.Add(EpisodeData);
		}
		return true;
	}

	//All Bone informations
	TArray<FBoneFingerTypeNames> BoneFingerNames;

	//All episodes
	TArray<FHandEpisodeData> PositionEpisode;

};