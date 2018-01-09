// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: David Brinkmann

#pragma once

#include "CoreMinimal.h"
#include "HUD/CIHUD.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "Components/TimelineComponent.h"
#include "SLRuntimeManager.h"
#include "StackChecker.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStackCheckDone, bool, wasSuccessful);

class ACharacterController; // Use Forward Declaration. 
UCLASS()
class CLICKINTERACTION_API AStackChecker : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AStackChecker();

	// The animation curve for rotation
	UPROPERTY(EditAnywhere, Category = "CI - Animation Curves")
		UCurveVector* RotationAnimationCurveVector;

	// The animation curve for shaking
	UPROPERTY(EditAnywhere, Category = "CI - Animation Curves")
		UCurveVector* ShakingAnimationCurveVector;

	// The maximium deviation of the item's relative distance to base item
	UPROPERTY(EditAnywhere, Category = "CI - General")
		float MaxDeviation;

	// Whether or not to show the stability check window
	UPROPERTY(EditAnywhere, Category = "CI - General")
		bool bShowStabilityCheckWindow;

	// The time in seconds to wait between shaking/rotating and stability check
	UPROPERTY(EditAnywhere, Category = "CI - General")
		float DelayBeforeStabilityCheck;

	UPROPERTY()
		FStackCheckDone OnStackCheckDone;

	void StartCheck(AActor* BaseItemToCheck);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FTimeline TimelineRotation;
	FTimeline TimelineShaking;

	ACharacterController* PlayerCharacter;

	// Initial position and rotation
	FVector StartPosition;
	FRotator StartRotation;

	AStaticMeshActor* StackCopy;

	TMap<AStaticMeshActor*, FVector> PositionsOnStart;

	float SumOfTimelineSeconds;
	float SecondsLeft;

	TSharedPtr<FOwlNode> StackCheckLogEvent;

	ACIHUD* ClickInteractionHUD;

	UFUNCTION()
		void RotateVector(FVector Vector);

	UFUNCTION()
		void RotateDone();

	UFUNCTION()
		void Shake(FVector Vector);

	UFUNCTION()
		void ShakeDone();

	void CheckStackRelativePositions();

	bool bStackCheckIsRunning;

	AStaticMeshActor* CopyStack(AActor* ActorToCopy);
	AStaticMeshActor* CopyActor(AActor* ActorToCopy);
	void SetScreenCaptureEnabled(bool bEnabled);

	void GenerateLogEvent(AActor * BaseItemToCheck);
	void FinishLogEvent(bool bStackCheckSuccessful);
	void EvaluateStabilityCheck();

	FTimerHandle TimerHandlePauseBeforeCheck;
};
