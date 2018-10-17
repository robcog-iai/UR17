// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Runtime/Engine/Classes/Animation/SkeletalMeshActor.h"
#include "UAnimationDataStructure.h"
#include "AGraspingStyleManager.generated.h"

/**
 * Deprecated class for grasping style creation out of an animation played in the game
 */
UCLASS()
class AAGraspingStyleManager : public AInfo
{
	GENERATED_BODY()
	
public:
	//The animation from which a grasping style should get created
		UPROPERTY(EditAnywhere)
			ASkeletalMeshActor* SelectedMesh;

		//The name of the newly created grasping style
		UPROPERTY(EditAnywhere)
			FString GraspingStyleName;

		//Start time of the animation
		UPROPERTY(EditAnywhere)
			float StartTime = 0;

		//End time of the animation
		UPROPERTY(EditAnywhere)
			float EndTime;

		//After this time a step will get recorded
		UPROPERTY(EditAnywhere)
			float StepSize = 0.5;

		//If the animation belongs to a right hand
		UPROPERTY(EditAnywhere)
			bool bIsRightHand;

		//Name of a file where our bones a mapped to fingers
		UPROPERTY(EditAnywhere)
			FString MapFileName;


		AAGraspingStyleManager();

		virtual void Tick(float DeltaTime) override;

		virtual void BeginPlay() override;

private:
	// Current time that passed between a step.
	float CurrentTime;

	//The total time that passed after begin play.
	float TotalTime;

	bool bReachedEndTime = false;
	bool bHasSendFirstData = false;
	FHandAnimationData AnimationData = FHandAnimationData();

	TMap<FString, FRotator> BoneSpaceRotations;
	TMap<FString, FRotator> CalculatedBoneRotations;
	TMap<FString, FRotator> ComponentSpaceRotations;
	TMap<FString, FBoneData> NewEpisodeData;

	//A Map which contains the ETypeOfFinger for every bone of the passed mesh.
	TMap<FString, ETypeOfFinger> TypeMap;

	//if start rotation of bones have been saved
	bool StartRotatorsSet = false;

	//start rotation of bones
	TMap<FString, FQuat> StartRotators;

	/*
	*Creates a map with the name of the bone as FString and the rotation of the bone at
	*the current step as FRotator.
	*
	*@param SkeletalComponent The USkeletalMeshComponent for which the rotations of the bones
	*get saved.
	*/
	void GetBoneDataForStep(USkeletalMeshComponent * SkeletalComponent);


	/*Creates a TArray of FBoneFingerTypeNames out of the passed map. Each FBoneFingerTypeName
	* contains the bone name, the FingerType (e.g thumb) and a BoneType.
	*@param StepData A Map which contains the bone name as key and its rotation as value for a step.
	*@return An Array which contains FBoneFingerTypeName for all bones of the mesh.
	*/
	TArray<FBoneFingerTypeNames> CreateBoneFingerTypeNames(TMap<FString, FRotator> StepData);



	/*
	*Creates a FHandAnimationData struct out of the passed Map. This struct contains
	*the name of the mesh, which hand got recorded, the name of the animation and the
	*data for the fingers and their position at a step.
	*
	@param StepData A map which contains entries for the bone name as FString and their rotation as FRotator.
	*/
	void CreateAnimationData(TMap<FString, FRotator> StepData);

	/*
	*Return the ETypeOfFinger for a given string if the string matches a pattern.
	* @param Type The possible string representation of a ETypeOfFinger
	* @return A fitting ETypeOfFinger or ETypeOfFinger.None if it did not match the pattern.
	*/
	ETypeOfFinger GetTypeOfFinger(FString Type);
};
