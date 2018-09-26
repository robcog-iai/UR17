// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaterTap.generated.h"

class UParticleSystemComponent;
class AStaticMeshActor;
class AEmitter;

UCLASS()
class UWATERTAPLOGIC_API AWaterTap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaterTap();

	// This function checks if an Emitter and StaticMeshActor are set and activates them. And set the settings of the components
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
	// The function can create a text output. Can be used for debugging
	void PrintDebugLog(FString string) const;
	
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// The water tap handle. Is set by the user 
	UPROPERTY(EditAnywhere, Category = "TurnOnWater")
		AStaticMeshActor* Handle;

	// The water. Is set by the user 
	UPROPERTY(EditAnywhere, Category = "TurnOnWater")
		AEmitter*  Water;

private:

	// Is true, if a water-emitter was set
	bool bWaterSet;
	
	// Is true, if a water tap handle was set
	bool bHandleSet;
};
