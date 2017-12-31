// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: David Brinkmann

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Misc/FileHelper.h"
#include "SLOwl.h"
#include "SLRuntimeManager.h"
#include "CLogger.generated.h"

class ACharacterController; // Use Forward Declaration. Including the header in CLogger.cpp

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLICKINTERACTION_API UCLogger : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCLogger();

	// Whether or not the logging is enabled
	UPROPERTY(EditAnywhere, Category = "CI - Log")
		bool bEnableLogging;

	// The absolut path where the log files get stored
	UPROPERTY(EditAnywhere, Category = "CI - Log")
		FString SaveDirectory;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called when actor removed from game or game ended
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	ACharacterController * PlayerCharacter;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Starts logging
	void StartLogger(FString Scenario, FString Mode);

	// Stops logging
	void EndLogger();

	// Writes the final summary of the events
	void WriteSummary();

	// Starts logging an event
	void StartEvent(TSharedPtr<FOwlNode> Event);

	// Finishes the event
	void FinishEvent(TSharedPtr<FOwlNode> Event);

	// Cancels an event
	void CancelEvent(TSharedPtr<FOwlNode> Event);

private:
	FString CurrentLog; // The current log file
	FString FileName; // The filename of the current logfile
	int ActionCounter; // The amount of actions for this episode
	float SumOfActionTime; // The sum of seconds for all the actions in this episode
	float SumOfStackingTime; // The sum in seconds taken by the time of the stack check
	int CountStackEvents; // The amount of stack events in this episode
	float EndTimeOfLastEvent; // The timestamp of the last event

	TMap<TSharedPtr<FOwlNode>, float> StartTimeOfEvents; // Maps the event to the respective timestamp when the event was started
	ASLRuntimeManager* SemLogRuntimeManager; // SemLogger instance

		
	
};
