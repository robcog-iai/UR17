// Fill out your copyright notice in the Description page of Project Settings.

#include "WaterTap.h"
#include "Engine/StaticMeshActor.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "UObject/UObjectGlobals.h"
#include "Runtime/Core/Public/Math/Rotator.h"
#include "ParticleDefinitions.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "Runtime/Engine/Classes/Particles/Emitter.h"


// Sets default values
AWaterTap::AWaterTap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//initializes the booleans with false
	bHandleSet = false;
	bWaterSet = false;

}

// Called when the game starts or when spawned
void AWaterTap::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWaterTap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check if both Boolean are true. This will ensure that an Emitter and a StaticMeshActor are set
	// With a positive check, a new rotator object is created and set equal to the rotation location of the StaticMeshActor
	if (bHandleSet && bWaterSet)
	{
		FRotator Rotation;
		Rotation = Handle->GetActorRotation();
	
		// It checks if the yaw value of the rotation object is less than, or equal to one
		// With a positive check, it is checked if the Emitter is activated
		// Otherwise the Emitter will be deactivated
		if (Rotation.Yaw <= 1) {

			// If the Emitter is active, nothing happens
			// Otherwise it will be activated
			if (Water->IsActive()) {
				PrintDebugLog(FString::Printf(TEXT("NoOfTurns changed, Angle calculated: %f"), Rotation.Yaw));
			}
			else {
				
				Water->Activate();
				
			}
		}
		else if (Water->IsActive()) {
			
				Water->Deactivate();
			
		}
	}
}

//This function checks if an Emitter and StaticMeshActor are set and activates them. And set settings of the components
#if WITH_EDITOR
void AWaterTap::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	// Get the name of the property that was changed
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Checks if the StaticMeshActor is set and activates the physics and mobility of the StaticMeshActor. In addition, the boolean bHandleSet is set to true
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AWaterTap, Handle))
	{
		Handle->SetMobility(EComponentMobility::Movable);
		Handle->GetStaticMeshComponent()->SetSimulatePhysics(true);
		bHandleSet = true;

		//PrintDebugLog(TEXT("Handle object set"));
	}
	// Checks if the Emitter is set and activates the Emitter. In addition, the boolean bWaterSet is set to true
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(AWaterTap, Water))
	{
		bWaterSet = true;
		Water->Activate();

		//PrintDebugLog(TEXT("Water object set"));
	}
}
#endif


// Function for text output
// Can be used for debugging
void AWaterTap::PrintDebugLog(FString string) const
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, string);
	}
}

