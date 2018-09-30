// Copyright 2018, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UAnimationDataStructure.h"
#include "UReadWrite.h"
#include "UGraspingAnimationController.generated.h"

//Event when the current activ animation changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNextAnimationDelegateR, FHandAnimationData, Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNextAnimationDelegateL, FHandAnimationData, Data);

/*
This class loads and manages all possible animation for both hands.
Only animations that are needed are loaded, but they remain in memory.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UREALISTICGRASPING_API UGraspingAnimationController : public UActorComponent
{
	GENERATED_BODY()

private:	
	// Sets default values for this component's properties
	UGraspingAnimationController();

	//Holds every animation that are available for the left and right hand
	TArray<FString> RightHandAnimationNames;
	TArray<FString> LeftHandAnimationNames;

	//Every already loaded animation for the left and right hand
	TMap<FString, FHandAnimationData> RightHandAnimations;
	TMap<FString, FHandAnimationData> LeftHandAnimations;

	//Current selected animation for the left and right hand
	FHandAnimationData* CurrentRightAnimation;
	FHandAnimationData* CurrentLeftAnimation;

	//The skeletal mesh name for the left and the right hand
	FString LeftHandMeshName;
	FString RightHandMeshName;

	//Read and write into files
	UReadWrite ReadWrite;

	//Current Animation index (needed for the next animation function)
	int CurrentRightGraspIndex;
	int CurrentLeftGraspIndex;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	This function sets the mesh name for the right or left hand
	@param Name, the skeletal mesh name
	@param bIsRightHand, true if this name stands for the right hand, flas for the left one
	*/
	void SetMeshName(FString Name, bool bIsRightHand);

	/*
	This function will return all possible animations for on hand
	@param bIsRightHand, true stands for the right hand, false for the left one
	@return all possible animations for this hand
	*/
	TArray<FString> GetAnimationNames(bool bIsRightHand);

	/*
	This function loads and sets a new animation
	@param AnimationName, the aniamtion name that should be loaded 
	@param bIsRightHand, true stands for the right hand, false for the left one

	*/
	void SetNewAnimation(FString AnimationName, bool bIsRightHand);

	/*
	This function loads the next animation in the row
	@param bIsRightHand, true stands for the right hand, false for the left one	
	@return the next aniamtion
	*/
	FHandAnimationData GetNextAnimation(bool bIsRightHand);

	//This delegate sends an update every time when the animation for the right hand changes
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FNextAnimationDelegateR OnNextAnimationR;

	//This delegate sends an update every time when the animation for the left hand changes
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FNextAnimationDelegateL OnNextAnimationL;
	
};
