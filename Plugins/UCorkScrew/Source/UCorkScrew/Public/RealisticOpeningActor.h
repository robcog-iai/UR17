// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PhysicsEngine/PhysicsConstraintActor.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RealisticOpeningActor.generated.h"

UCLASS()
class UCORKSCREW_API ARealisticOpeningActor : public APhysicsConstraintActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARealisticOpeningActor();
	ARealisticOpeningActor(const FObjectInitializer & ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = "ScrewPropertys")
		bool ActiveOpeningConstraint = true;
	UPROPERTY(EditAnywhere, Category = "ScrewPropertys")
		AActor* Bottle;
	UPROPERTY(EditAnywhere, Category = "ScrewPropertys")
		AActor* Lid;
	// Sets the angle. At this angle the constraint will break if the function is activated.
	UPROPERTY(EditAnywhere, Category = "ScrewPropertys", meta = (UIMin = "1.0"))
		float ScrewAngle = 60;
	// Distance it should move up. Is just prepared yet.
	UPROPERTY(EditAnywhere, Category = "ScrewPropertys", meta = (UIMin = "0.0"))
		float UpMovingDistance = 0;
	// If its true, the rotation to open it is to the right. Else it is going to be left.
	UPROPERTY(EditAnywhere, Category = "ScrewPropertys")
		bool RotateDirectionRight = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Some needed variables, in different functions.

	// Old degree value of swing 1.
	float OldSwing1Degree; 
	// The turn flag says, if it is turned more than 30 degrees or less.
	// Important to know if the limit should be locked or not.
	bool bIsTurnFlagOn;
	// Counts every half turn.
	int TurnCounter;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Needed to extend APhysicsConstraintActor
	virtual void LoadedFromAnotherClass(const FName & OldClassName);
	void PostLoad();


private:
	// Sets values for the bottle opening constraint.
	void BottleOpening();
	// Activates the Limits for the constraint.
	void ActivateConstraintLimit();
	// Checks if a value is changed in the editor.
	void PostEditChangeProperty(struct FPropertyChangedEvent& e);
	// Calculates the actual real degrees the lid is turned on the right site.
	float DegreeCalcRight();
	// Calculates the actual real degrees the lid is turned on the left site.
	float DegreeCalcLeft();
	// Moves and breaks the constraint, depeends on the current degrees its turned.
	void MovingAndBreaking(float CurrentDegree);
};
