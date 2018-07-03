// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Animation/SkeletalMeshActor.h"
#include "UGraspingAnimationController.h"
#include "AGraspingHand.generated.h"

UCLASS()
class UREALISTICGRASPING_API AGraspingHand : public ASkeletalMeshActor
{
	GENERATED_BODY()
	
public:	
	//// Sets default values for this actor's properties
	AGraspingHand();

//protected:
//	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
//
public:	
//	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
