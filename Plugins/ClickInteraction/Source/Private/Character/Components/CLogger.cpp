// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: David Brinkmann

#include "CLogger.h"

#include "../Private/Character/CharacterController.h"
#include "PlatformFilemanager.h"
#include "Paths.h"
#include "DateTime.h"
#include "SecureHash.h"
#include "EngineUtils.h"

// Sets default values for this component's properties
UCLogger::UCLogger()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	bEnableLogging = true;
	SaveDirectory = FPaths::GameDir().Append("CILog/");
}


// Called when the game starts
void UCLogger::BeginPlay()
{
	Super::BeginPlay();

	// Get the semantic log runtime manager from the world
	for (TActorIterator<ASLRuntimeManager>RMItr(GetWorld()); RMItr; ++RMItr)
	{
		SemLogRuntimeManager = *RMItr;
		break;
	}

	// Choose the folder depending on the scenario and mode
	if (PlayerCharacter == nullptr) {
		UE_LOG(LogTemp, Fatal, TEXT("UCLogger::BeginPlay: The PlayerCharacter was not assigned. Restarting the editor might fix this."));
	}
	else {
		switch (PlayerCharacter->ScenarioType) {
		case EScenarioType::OnePersonBreakfast:
			SaveDirectory.Append("OnePersonBreakfast/");
			break;
		case EScenarioType::TwoPersonBreakfast:
			SaveDirectory.Append("TwoPersonBreakfast/");
			break;
		case EScenarioType::FourPersonBreakfast:
			SaveDirectory.Append("FourPersonBreakfast/");
			break;
		}

		switch (PlayerCharacter->InteractionMode) {
		case EInteractionMode::OneHandMode:
			SaveDirectory.Append("1Hand/");
			break;
		case EInteractionMode::TwoHandMode:
			SaveDirectory.Append("2Hand/");
			break;
		case EInteractionMode::TwoHandStackingMode:
			SaveDirectory.Append("Stack/");
			break;
		}
	}
	// **************************************
}

void UCLogger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndLogger();
}


// Called every frame
void UCLogger::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCLogger::StartLogger(FString Scenario, FString Mode)
{
	if (bEnableLogging == false) return;

	FString EpisodeID;

	if (SemLogRuntimeManager != nullptr) {
		EpisodeID = FString(SemLogRuntimeManager->GetEpisodeId());
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("UCLogger::StartLogger:Could not find USemLogger. Using Timestamp"));
		EpisodeID = FMD5::HashAnsiString(*FString(FDateTime::Now().ToString())); // Use hash of timestamp
	}

	// Scenario description
	CurrentLog.Append(";ID:").Append(EpisodeID).Append(";Szenario:").Append(Scenario).Append(";Modus:").Append(Mode).Append(LINE_TERMINATOR);

	// Header
	CurrentLog.Append("Nr.;")
		.Append("Aktion;")
		.Append("Objekt;")
		.Append("Start;")
		.Append("Ende;")
		.Append("Dauer")
		.Append(LINE_TERMINATOR);

	FileName.Append(EpisodeID).Append(".csv");
	UE_LOG(LogTemp, Warning, TEXT("UCLogger::StartLogger: Started log %s"), *FileName);
}

void UCLogger::EndLogger()
{
	if (bEnableLogging == false) return;
	if (ActionCounter <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("UCLogger::EndLogger: No actions done. Nothing to log. Skip creating log file."));
		return;
	}

	bEnableLogging = false; // Deactivate logging to avoid creating of the file twice

	WriteSummary();

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// CreateDirectoryTree returns true if the destination
	// directory existed prior to call or has been created
	// during the call.
	if (PlatformFile.CreateDirectoryTree(*SaveDirectory))
	{
		// Get absolute file path
		FString AbsoluteFilePath = SaveDirectory + "/" + FileName;

		if (FFileHelper::SaveStringToFile(CurrentLog, *AbsoluteFilePath) == false) {
			UE_LOG(LogTemp, Warning, TEXT("UCLogger::EndLogger:Could not create log file %s. Restart the Engine. That mostly solves that problem."), *AbsoluteFilePath);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("UCLogger::EndLogger:Could not create folder %s. Restart the Engine. That mostly solves that problem."), *SaveDirectory);
	}
}

void UCLogger::WriteSummary()
{
	if (ActionCounter <= 0) return;

	FString Summary = LINE_TERMINATOR;

	if (PlayerCharacter->InteractionMode == EInteractionMode::TwoHandStackingMode) {
		Summary.Append(";;Mit Stack-Zeit;Ohne Stack-Zeit").Append(LINE_TERMINATOR);
		Summary.Append(";Spielzeit;").Append(FString::SanitizeFloat(EndTimeOfLastEvent)).Append(";").Append(FString::SanitizeFloat(EndTimeOfLastEvent - 2 * SumOfStackingTime)).Append(LINE_TERMINATOR);
		Summary.Append(";Aktionszeit;").Append(FString::SanitizeFloat(SumOfActionTime - SumOfStackingTime)).Append(";").Append(FString::SanitizeFloat(SumOfActionTime - 2 * SumOfStackingTime)).Append(LINE_TERMINATOR);
		Summary.Append(";Anzahl Aktionen;").Append(FString::FromInt(ActionCounter)).Append(";").Append(FString::FromInt(ActionCounter - CountStackEvents));
	}
	else {
		Summary.Append(";Spielzeit;").Append(FString::SanitizeFloat(EndTimeOfLastEvent)).Append(LINE_TERMINATOR);
		Summary.Append(";Aktionszeit;").Append(FString::SanitizeFloat(SumOfActionTime)).Append(LINE_TERMINATOR);
		Summary.Append(";Anzahl Aktionen;").Append(FString::FromInt(ActionCounter)).Append(LINE_TERMINATOR);
	}

	CurrentLog.Append(Summary);
}

void UCLogger::StartEvent(TSharedPtr<FOwlNode> Event)
{
	if (bEnableLogging == false) return;

	StartTimeOfEvents.Add(Event, GetWorld()->GetTimeSeconds());
}

void UCLogger::FinishEvent(TSharedPtr<FOwlNode> Event)
{
	if (bEnableLogging == false) return;

	ActionCounter++;

	FString NewLogLine;

	FString ActionDone;
	FString OtherObject;

	bool bIsStackingEvent = false;

	TArray <FOwlTriple> Properties = Event->Properties;
	for (auto& Property : Properties) {
		FOwlPrefixName Subject = Property.Subject;

		if (Subject.Name.Equals("taskContext")) { // Read task context
			ActionDone = Property.Value;

			if (ActionDone.Contains("Stack Checking")) bIsStackingEvent = true;
		}
		else if (Subject.Name.Equals("objectActedOn")) { // or read the actor
			OtherObject = Property.Object;
			OtherObject.RemoveFromStart("&log;");
		}
	}

	FString StartTimeText;
	FString EndTimeAndDuration;

	if (StartTimeOfEvents.Contains(Event)) {
		float StartTime = StartTimeOfEvents[Event];
		float EndTime = GetWorld()->GetTimeSeconds();
		float Duration = EndTime - StartTime;
		StartTimeText = FString::SanitizeFloat(StartTime);

		EndTimeAndDuration = FString::SanitizeFloat(EndTime).Append(";").Append(FString::SanitizeFloat(Duration));

		// *** For summary ***
		EndTimeOfLastEvent = EndTime;
		SumOfActionTime += Duration;

		if (bIsStackingEvent) {
			SumOfStackingTime += Duration;
			CountStackEvents++;
		}
		// *** *** *** *** ***
	}

	NewLogLine.Append(FString::FromInt(ActionCounter)).Append(";")
		.Append(ActionDone).Append(";")
		.Append(OtherObject).Append(";")
		.Append(StartTimeText).Append(";")
		.Append(EndTimeAndDuration)
		.Append(LINE_TERMINATOR);

	CurrentLog.Append(NewLogLine);

	StartTimeOfEvents.Remove(Event);
}

void UCLogger::CancelEvent(TSharedPtr<FOwlNode> Event)
{
	if (bEnableLogging == false) return;

	if (StartTimeOfEvents.Contains(Event)) {
		StartTimeOfEvents.Remove(Event);
	}
}

