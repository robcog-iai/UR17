// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Waldemar Zeitler

#pragma once

#include "Components/ActorComponent.h"
#include "Engine/StaticMeshActor.h"
#include "CoreMinimal.h"
#include "GOpenClose.generated.h"

class AGameController; // Use Forward Declaration. Including the header in COpenClose.cpp

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UGAME_API UGOpenClose : public UActorComponent
{
				GENERATED_BODY()

public:
				// Sets default values for this component's properties
				UGOpenClose();

				UPROPERTY(EditAnyWhere, Category = "CI - General")
								float ForceToApply;

				AGameController * PlayerCharacter; // The player character instance

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
