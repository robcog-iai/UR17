// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "CoreMisc.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/ActorComponent.h"
#include "CReadWriteComponent.generated.h"

USTRUCT(BlueprintType)
struct FTransformStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReadWrite")
		FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReadWrite")
		FRotator Rotation;

};

USTRUCT(BlueprintType)
struct FSaveDataStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReadWrite")
		TArray<FTransformStruct> Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReadWrite")
		float Time;

};


/**
*
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UCReadWriteComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	UCReadWriteComponent();

	//Saves the data we get from motive
	TArray<FSaveDataStruct> MotivData;

	//Loads the saved data from the MotivData
	TArray<FSaveDataStruct> SaveDataArray;

	//Number of one data set
	int MaxDataCount;

	//Index of the last data set from ReceiveMotive
	int DataIndex;

	//Number of saved data
	int SizeData;

	//max time of one saved animation
	float MaxTime;

	//Number of emelemts of one data set
	int NumberOfElements;

	//imporves the recorded animation for slower / faster pc than the one it was recorded on.
	float NextTime;

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	This Function sets the Time for Writing into a File. when you want to record 10 seconds, then you have to give this function 10.
	@param Time, the Time to record
	@return tru if it worked or false if Time < 0 or when ther ist still some time left
	*/
	UFUNCTION(BlueprintCallable, Category = "ReadWrite")
		bool SetTime(float Time);

	/*
	With this Function you can store more and more Data into an Array and then you can write this array into a File with WriteData(). It uses Time
	for synchronization for slower and faster PC, when you read the Data later on
	@param Data, well the Data to save
	@param Time, the Tick Time
	@return false if there are no time left or the Array Data Lenght has changed
	*/
	UFUNCTION(BlueprintCallable, Category = "ReadWrite", meta = (AdvancedDisplay = "2"))
		bool SaveData(TArray<FTransformStruct> Data, float Time);

	/*
	With this Function you can write the stored Data from the saveData() Function into a File
	@param Name, The File name. It must be in the Data Folder
	*/
	UFUNCTION(BlueprintCallable, Category = "ReadWrite", meta = (AdvancedDisplay = "2"))
		void WriteData(FString Name);

	/*
	With this Function you can read from a File and give all the Data back into an Array
	@param Path, the Path of the File
	@param Count, the Number of DataSets you want to read. 1 DataSet are 21 Structs. So if you give this Function 1, then you will gent an array with 21 Structs.
	Thats because we allways Save 21 Data because we have 21 bones
	*/
	UFUNCTION(BlueprintCallable, Category = "ReadWrite", meta = (AdvancedDisplay = "2"))
		TArray<FSaveDataStruct> ReadFromFile(FString Path);
	/*
	With this Function you can read from a File and send it, it will simulate the ReciveMotiv Function. It works for slower and faster Pc's
	@Param DeltaTime, it is the Tick Rate from the Engine

	*/
	UFUNCTION(BlueprintCallable, Category = "ReadWrite", meta = (AdvancedDisplay = "2"))
		TArray<FTransformStruct> ReplayData(float DeltaTime);
	/*
	Helper Function to convert the Datas into an FileArchive.
	@Source https://wiki.unrealengine.com/Save_System,_Read_%26_Write_Any_Data_to_Compressed_Binary_Files
	*/
	void SaveLoadData(FArchive& Ar, TArray<FSaveDataStruct>& Data, int32 Count, int32 NumberOFElements);
	/*
	Helper Function to load Datas into an Array.
	*/
	bool LoadGameDataFromFileCompressed(
		const FString& FullFilePath,
		TArray<FSaveDataStruct>& Data,
		int32 Count,
		int32 NumberOFElements
	);

	/*
	Helper Function to save the Datas into an File. We take this Idee from the Internet
	@Quelle https://wiki.unrealengine.com/Save_System,_Read_%26_Write_Any_Data_to_Compressed_Binary_Files
	*/
	bool SaveGameDataToFileCompressed(const FString& FullFilePath,
		TArray<FSaveDataStruct>& Data
	);
	/*
	This is for the Recive Motiv Save Function. It will set a new File.
	@param FileName, the name of the File. It must be in the Data Folder
	*/
	UFUNCTION(BlueprintCallable, Category = "ReadWrite", meta = (AdvancedDisplay = "2"))
		void ReadDataFromFile(FString FileName);

	void CountDataIndexUP();
};
