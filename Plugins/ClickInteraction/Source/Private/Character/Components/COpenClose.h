// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: David Brinkmann

#pragma once

#include "Components/ActorComponent.h"
#include "Engine/StaticMeshActor.h"
#include "CoreMinimal.h"
#include "SLRuntimeManager.h"
#include "COpenClose.generated.h"

class ACharacterController; // Use Forward Declaration. Including the header in COpenClose.cpp

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CLICKINTERACTION_API UCOpenClose : public UActorComponent
{
	GENERATED_BODY()


public:
	// Sets default values for this component's properties
	UCOpenClose();

	UPROPERTY(EditAnyWhere, Category = "CI - General")
		float ForceToApply;

	ACharacterController * PlayerCharacter; // The player character instance

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetupKeyBindings(UInputComponent* PlayerInputComponent);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	TSet<AActor*> SetOfOpenCloasableItems; // All actors in the world that can be opened

	void SetLockedByComponent(bool bIsLocked); 

	AStaticMeshActor* ClickedActor; // The last clicked actor
	void AddForceToObject(float DeltaTime);

	ASLRuntimeManager* SemLogRuntimeManager; // SemLogger instance
	TSharedPtr<FOwlNode> CurrentLogEvent; // The current log event
	TSharedPtr<FOwlNode> CurrentSemLogEvent;

	void StartLogEvent(AActor* ActorToHandle);
	void FinishLogEvent(AActor* ActorToHandle);

	// *** Input ***
	bool bLeftMouseHold;
	bool bRightMouseHold;

	void InputLeftHandPressed();
	void InputLeftHandReleased();
	void InputRightHandPressed();
	void InputRightHandReleased();

	void OnInteractionKeyPressed(bool bIsRightKey);
	void OnInteractionKeyHold(bool bIsRightKey, float DeltaTime);
	void OnInteractionKeyReleased(bool bIsRightKey);
	// ********************
};
