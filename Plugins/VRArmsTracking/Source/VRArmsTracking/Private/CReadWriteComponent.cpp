// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#include "CReadWriteComponent.h"
#include "Engine.h"


UCReadWriteComponent::UCReadWriteComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaxDataCount = 0;
	DataIndex = 0;
	MaxTime = -1;
	NumberOfElements = -1;
	NextTime = 0;
	// ...
}


void UCReadWriteComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UCReadWriteComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UCReadWriteComponent::SetTime(float Time)
{
	if (MaxTime > 0 || Time < 0) {
		return false;
	}
	MaxTime = Time;
	return true;
}

bool UCReadWriteComponent::SaveData(TArray<FTransformStruct> Data, float Time)
{
	if (MaxTime < 0 || Time < 0) {
		return false;
	}
	if (NumberOfElements == -1) {
		NumberOfElements = Data.Num();
	}
	else if (NumberOfElements != Data.Num()) {
		return false;
	}
	MaxTime -= Time;
	MaxDataCount++;
	FSaveDataStruct NewDataStruct;
	NewDataStruct.Time = Time;
	NewDataStruct.Data = Data;
	MotivData.Add(NewDataStruct);
	return true;
}

void UCReadWriteComponent::WriteData(FString Name)
{
	if (Name.Len() <= 0 || MotivData.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("Wrong Path or no Data to write!"));
		return;
	}
	FString endName = FPaths::ConvertRelativePathToFull(FPaths::GameDir()) + "Data/" + Name + "(" + FString::FromInt(NumberOfElements) + "," + FString::FromInt(MaxDataCount) + ").save";
	SaveGameDataToFileCompressed(endName, MotivData);
	MotivData = TArray<FSaveDataStruct>();
	MaxDataCount = 0;
	NumberOfElements = -1;
}

TArray<FSaveDataStruct> UCReadWriteComponent::ReadFromFile(FString Path)
{
	TArray<FSaveDataStruct> Data;
	if (Path.Len() <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("Wrong Path!"));
		return Data;
	}
	const TCHAR* Delims[] = { TEXT("(") };
	const TCHAR* Delims2[] = { TEXT(",") };
	const TCHAR* Delims3[] = { TEXT(")") };
	TArray<FString> Parsed;
	TArray<FString> Parsed2;
	TArray<FString> Parsed3;
	try {
		Path.ParseIntoArray(Parsed, Delims, true);
		Parsed[1].ParseIntoArray(Parsed2, Delims2, true);
		FString SaveNumer = Parsed2[0];
		Parsed2[1].ParseIntoArray(Parsed3, Delims3, true);
		FString SaveMaxNumber = Parsed3[0];
		int32 Number;
		int32 MaxNumber;
		Number = FCString::Atoi(*Parsed2[0]);
		MaxNumber = FCString::Atoi(*Parsed3[0]);
		LoadGameDataFromFileCompressed(Path, Data, MaxNumber, Number);
	}
	catch (...) {
		UE_LOG(LogTemp, Error, TEXT("Maybe your Fiele isn't in the right format."));
	}
	return Data;
}

TArray<FTransformStruct> UCReadWriteComponent::ReplayData(float DeltaTime)
{
	if (SaveDataArray.Num() <= 0 || DeltaTime <= 0) {
		return TArray<FTransformStruct>();
	}
	float AddingTime = SaveDataArray[DataIndex].Time;
	float DeltaPlusNext = DeltaTime + NextTime;
	if (DeltaPlusNext < AddingTime) { //faster Pc
		NextTime += DeltaTime;
	}
	else if (DeltaPlusNext> AddingTime) { //slower Pc
		int32 RoundCounter = 0;
		int32 SaveDataIndex = DataIndex;
		while (AddingTime < DeltaPlusNext) {
			CountDataIndexUP();
			AddingTime += SaveDataArray[DataIndex].Time;
			RoundCounter++;
		}
		NextTime = DeltaPlusNext - AddingTime;
	}
	return SaveDataArray[DataIndex].Data;
}

void UCReadWriteComponent::SaveLoadData(FArchive& Ar,
	TArray<FSaveDataStruct>& Data,
	int32 Count,
	int32 NumberOFElements

)
{
	try {
		for (int32 counter = 0; counter < Count; counter++) {
			for (int32 CounterElements = 0; CounterElements < NumberOFElements; CounterElements++) {
				Ar << Data[counter].Data[CounterElements].Location;
				Ar << Data[counter].Data[CounterElements].Rotation;

			}
			Ar << Data[counter].Time;
		}
	}
	catch (...) {
		UE_LOG(LogTemp, Error, TEXT("Not enought Data. Your File is broken"));
	}
}

bool UCReadWriteComponent::LoadGameDataFromFileCompressed(
	const FString& FullFilePath,
	TArray<FSaveDataStruct>& Data,
	int32 Count,
	int32 NumberOFElements
) {

	//Load the Compressed data array
	TArray<uint8> CompressedData;
	if (!FFileHelper::LoadFileToArray(CompressedData, *FullFilePath))
	{
		//Optimize("FFILEHELPER:>> Invalid File");
		return false;
		//~~
	}

	// Decompress File 
	FArchiveLoadCompressedProxy Decompressor =
		FArchiveLoadCompressedProxy(CompressedData, ECompressionFlags::COMPRESS_ZLIB);

	//Decompression Error?
	if (Decompressor.GetError())
	{
		//Optimize("FArchiveLoadCompressedProxy>> ERROR : File Was Not Compressed ");
		return false;
		//
	}

	//Decompress
	FBufferArchive DecompressedBinaryArray;
	Decompressor << DecompressedBinaryArray;

	//~
	//		  Read the Data Retrieved
	//~

	FMemoryReader FromBinary = FMemoryReader(DecompressedBinaryArray, true); //true, free data after done
	FromBinary.Seek(0);
	for (int32 Counter = 0; Counter < Count; Counter++) {
		FSaveDataStruct NewData;
		TArray<FTransformStruct> NewDataArray;
		for (int32 CounterElements = 0; CounterElements < NumberOFElements; CounterElements++) {
			NewDataArray.Add(FTransformStruct());
		}
		NewData.Data = NewDataArray;
		Data.Add(NewData);
	}
	SaveLoadData(FromBinary, Data, Count, NumberOFElements);

	//~
	//								Clean up 
	//~
	CompressedData.Empty();
	Decompressor.FlushCache();
	FromBinary.FlushCache();

	// Empty & Close Buffer 
	DecompressedBinaryArray.Empty();
	DecompressedBinaryArray.Close();

	return true;

}

bool UCReadWriteComponent::SaveGameDataToFileCompressed(const FString& FullFilePath, TArray<FSaveDataStruct>& Data) {
	FBufferArchive ToBinary;
	SaveLoadData(ToBinary, MotivData, MaxDataCount, NumberOfElements);

	// Compress File 
	//tmp compressed data array
	TArray<uint8> CompressedData;
	FArchiveSaveCompressedProxy Compressor =
		FArchiveSaveCompressedProxy(CompressedData, ECompressionFlags::COMPRESS_ZLIB);

	//Send entire binary array/archive to compressor
	Compressor << ToBinary;

	//send archive serialized data to binary array
	Compressor.Flush();

	//vibes to file, return successful or not
	if (FFileHelper::SaveArrayToFile(CompressedData, *FullFilePath))
	{
		// Free Binary Arrays 
		Compressor.FlushCache();
		CompressedData.Empty();

		ToBinary.FlushCache();
		ToBinary.Empty();

		// Close Buffer 
		ToBinary.Close();

		//ClientMessage("File Save Success!");

		return true;
	}
	else
	{
		// Free Binary Arrays 
		Compressor.FlushCache();
		CompressedData.Empty();

		ToBinary.FlushCache();
		ToBinary.Empty();

		// Close Buffer 
		ToBinary.Close();

		//ClientMessage("File Could Not Be Saved!");

		return false;
		//
	}

}

void UCReadWriteComponent::ReadDataFromFile(FString FileName)
{
	SaveDataArray = TArray<FSaveDataStruct>();
	SaveDataArray = ReadFromFile(FPaths::ConvertRelativePathToFull(FPaths::GameDir()) + "Data/" + FileName);
	SizeData = SaveDataArray.Num();
	DataIndex = 0;
}

void UCReadWriteComponent::CountDataIndexUP() {
	if (DataIndex + 1 >= SizeData) {
		DataIndex = 0;
	}
	else {
		DataIndex++;
	}
}




