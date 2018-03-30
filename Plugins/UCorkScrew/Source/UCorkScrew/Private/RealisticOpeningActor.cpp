// Fill out your copyright notice in the Description page of Project Settings.

#include "RealisticOpeningActor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

// Sets default values
ARealisticOpeningActor::ARealisticOpeningActor() 
{
	PrimaryActorTick.bCanEverTick = true;
	if (ActiveOpeningConstraint) 
		{
			BottleOpening();
		}
	
}

ARealisticOpeningActor::ARealisticOpeningActor(const FObjectInitializer & ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (ActiveOpeningConstraint) 
		{
			BottleOpening();
		}
}

// Called when the game starts or when spawned
void ARealisticOpeningActor::BeginPlay()
{
	Super::BeginPlay();
	if (ActiveOpeningConstraint) 
		{
			BottleOpening();
		}
}

// Called every frame
void ARealisticOpeningActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RotateDirectionRight) 
		{
			float currentDegree = DegreeCalcRight();
			MovingAndBreaking(currentDegree);
		}
		else 
		{
			float currentDegree = DegreeCalcLeft();
			MovingAndBreaking(currentDegree);
		}
}

void ARealisticOpeningActor::LoadedFromAnotherClass(const FName & OldClassName) 
{
}

void ARealisticOpeningActor::PostLoad() 
{
	AActor::PostLoad();
}

void ARealisticOpeningActor::BottleOpening() 
{
	TurnCounter = 0;
	if (ActiveOpeningConstraint) 
		{
			// Sets the constraint limit, also checks in which direction it should be turnable.
			ActivateConstraintLimit();
			if (Bottle != nullptr) 
				{
					this->SetActorLocation(Bottle->GetActorLocation());
					this->GetConstraintComp()->ConstraintActor1 = Bottle;
					if (Lid != nullptr) 
						{
							this->GetConstraintComp()->ConstraintActor2 = Lid;
						}
				}
			OldSwing1Degree = this->GetConstraintComp()->GetCurrentSwing1();
			bIsTurnFlagOn = false;
	}
}

void ARealisticOpeningActor::ActivateConstraintLimit() 
{
	this->GetConstraintComp()->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0);
	this->GetConstraintComp()->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);
	// Locks movement at 30 degrees.
	this->GetConstraintComp()->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, 30.0f);
	if (RotateDirectionRight) 
		{
			// Gives a 30 degree offset to make it impossible turning it to the left, since the 30 degree limit.
			this->GetConstraintComp()->ConstraintInstance.AngularRotationOffset = FRotator::FRotator(0, 30, 0);
		}
		else 
		{
			// Gives a -30 degree offset to make it impossible turning it to the left, since the 30 degree limit.
			this->GetConstraintComp()->ConstraintInstance.AngularRotationOffset = FRotator::FRotator(0, -30, 0);
		}
}

float ARealisticOpeningActor::DegreeCalcRight()
{
	float CurrentSwingDegree = this->GetConstraintComp()->GetCurrentSwing1();

	// Checks the direction of the movement. If the direction is to the right its true.
	// Current value will be bigger than the old one if it turns to the right.
	if (CurrentSwingDegree > OldSwing1Degree) 
		{
			if (OldSwing1Degree < -170 && CurrentSwingDegree >= 170) 
				{
				}
				else 
				{
					// If the turning flag isnt set and it is passing the 0 point, it needs to be set
					// and the limit needs to be turned of, so the lid can rotate free.
					// Not more checks necessary, because the right turn can only increase the turning values
					// on right turning opening.
					if (!bIsTurnFlagOn && CurrentSwingDegree > 0 && OldSwing1Degree < 0) 
						{
							this->GetConstraintComp()->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Free, 360.f);
							bIsTurnFlagOn = true;
						}
						// If it passes the 150 degree to the right, it has 180 degrees completed. Increase counter.
						else if (bIsTurnFlagOn && CurrentSwingDegree >= 150 && OldSwing1Degree < 150) 
						{
							++TurnCounter;
						}
						// If it passes the -30 degree to the right, it has 180 degrees completed. Increase counter.
						else if (bIsTurnFlagOn && CurrentSwingDegree > -30 && OldSwing1Degree < -30) 
						{
							++TurnCounter;
						}
				}
		}
		// If the the current value is smaller than the old one, direction must be on the left.
		// No else necessary, since there is nothing to do if the value is equal.
		else if (CurrentSwingDegree < OldSwing1Degree) 
		{
			// To make sure it does nothing on the switch to negative values.
			if (OldSwing1Degree > 170 && CurrentSwingDegree >= -180 && CurrentSwingDegree < 0) 
				{
				}
				// When it doesnt switch just to negative values
				else 
				{
					// Check if it passes the 0 and the 180 degree turn Counter is on 0, to turn the limits on again.
					if (bIsTurnFlagOn && CurrentSwingDegree < 0 && OldSwing1Degree > 0 && TurnCounter == 0) 
						{
							ActivateConstraintLimit();
							bIsTurnFlagOn = false;
						}
					// If it passes the 150 degree to the left, it has 180 degrees completed. Decrease counter.
					if (bIsTurnFlagOn && CurrentSwingDegree < 150 && OldSwing1Degree >= 150) 
						{
							--TurnCounter;
						}
						// If it passes the -30 degree to the left, it has 180 degrees completed. Decrease counter.
						else if (bIsTurnFlagOn && CurrentSwingDegree < -30 && OldSwing1Degree > -30) 
						{
							--TurnCounter;
						}
				}
		}

	// Set the current degrees to the old once, to use it at the next call.
	OldSwing1Degree = CurrentSwingDegree;

	// Calculating the degree rest over the 180 degree counter.
	// Also takes care of the 30 degrees offset.

	// If the current swing is between 150 and 180.
	if (CurrentSwingDegree >= 150 && CurrentSwingDegree <= 180) 
		{
			CurrentSwingDegree = CurrentSwingDegree - 150;
		}
		// If the current swing is between -180 and -30.
		else if (CurrentSwingDegree >= -180 && CurrentSwingDegree < -30) 
		{
			CurrentSwingDegree = 210 + CurrentSwingDegree;
		}
		// If the current swing is between -30 and 150
		else 
		{
			CurrentSwingDegree = CurrentSwingDegree + 30;
		}

	// The degree over the 180 degree counter + 180 degree counter * 180 degrees
	// is the real turned value, return this value.
	return CurrentSwingDegree + (TurnCounter * 180);

}

float ARealisticOpeningActor::DegreeCalcLeft() 
{
	float CurrentSwingDegree = this->GetConstraintComp()->GetCurrentSwing1();

	// Checks the direction of the movement. If the direction is to the right its true.
	// Current value will be bigger than the old one if it turns to the right.
	if (CurrentSwingDegree > OldSwing1Degree) 
		{
			// To make sure it does nothing on the switch to negative values.
			if (OldSwing1Degree < -170 && CurrentSwingDegree >= 170) 
				{
				}
				else 
				{
					// Check if it passes the 0 and the 180 degree turn Counter is on 0, to turn the limits on again.
					if (bIsTurnFlagOn && CurrentSwingDegree > 0 && OldSwing1Degree < 0 && TurnCounter == 0)
						{
							ActivateConstraintLimit();
							bIsTurnFlagOn = false;
						}
						// If it passes the -150 degree to the right, it has 180 degrees completed. Decrease counter.
						else if (bIsTurnFlagOn && CurrentSwingDegree >= -150 && OldSwing1Degree < -150) 
						{
							--TurnCounter;
						}
						// If it passes the 30 degree to the right, it has 180 degrees completed. Decrease counter.
						else if (bIsTurnFlagOn && CurrentSwingDegree > 30 && OldSwing1Degree < 30) 
						{
							--TurnCounter;
						}
				}
		}
		// If the the current value is smaller than the old one, direction must be on the left.
		// No else necessary, since there is nothing to do if the value is equal.
		else if (CurrentSwingDegree < OldSwing1Degree) 
		{
			// To make sure it does nothing on the switch to negative values.
			if (OldSwing1Degree > 170 && CurrentSwingDegree >= -180 && CurrentSwingDegree < 0) 
				{
				}
				else {
					// If the turning flag isnt set and it is passing the 0 point, it needs to be set
					// and the limit needs to be turned of, so the lid can rotate free.
					// Not more checks necessary, because the right turn can only increase the turning values
					// on right turning opening.
					if (!bIsTurnFlagOn && CurrentSwingDegree < 0 && OldSwing1Degree > 0) 
						{
							this->GetConstraintComp()->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Free, 360.f);
							bIsTurnFlagOn = true;
						}
						// If it passes the -150 degree to the left, it has 180 degrees completed. Increase counter.
						else if (bIsTurnFlagOn && CurrentSwingDegree < -150 && OldSwing1Degree >= -150) 
						{
							++TurnCounter;
						}
						// If it passes the 30 degree to the left, it has 180 degrees completed. Increase counter.
						else if (bIsTurnFlagOn && CurrentSwingDegree < 30 && OldSwing1Degree > 30) 
						{
							++TurnCounter;
						}
				}
		}

	// Set the current degrees to the old once, to use it at the next call.
	OldSwing1Degree = CurrentSwingDegree;
	
	// Calculating the degree rest over the 180 degree counter.
	// Also takes care of the -30 degrees offset.

	// If the current swing is between -150 and -180.
	if (CurrentSwingDegree <= -150 && CurrentSwingDegree >= -180) 
		{
			CurrentSwingDegree = -150 + CurrentSwingDegree;
		} 
		// If the current swing is between -150 and 30.
		else if (CurrentSwingDegree > -150 && CurrentSwingDegree < 30) 
		{
			CurrentSwingDegree = 30 - CurrentSwingDegree;
		}
		// If the current swing is between 30 and 180
		else 
		{
			CurrentSwingDegree = 210 - CurrentSwingDegree;
		}

	// The degree over the 180 degree counter + 180 degree counter * 180 degrees
	// is the real turned value, return this value.
	return CurrentSwingDegree + (TurnCounter * 180);;
}

// Function to move the lid and break the constraint when the screw angle is passed.
void ARealisticOpeningActor::MovingAndBreaking(float CurrentDegree) 
{
	//Moving function is just prepared since it doesn't work yet.

	// If it has reached or passed the screw angle with the degrees, break it.
	if (CurrentDegree >= ScrewAngle) 
		{
			this->GetConstraintComp()->BreakConstraint();
		}
}

// Helpfull URL for this function: https://answers.unrealengine.com/questions/252811/how-do-i-use-posteditchangeproperty.html
// Gets triggerd each time a property is changed in the editor.
void ARealisticOpeningActor::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	Super::PostEditChangeProperty(e);

	// Name of the property that is changed. If the proberty isn't named at all, set it on NAME_None.
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
	// If the bottle is set.
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ARealisticOpeningActor, Bottle) && ActiveOpeningConstraint) 
		{
			// Sets the location of the constraint on the position of the bottle automaticly.
			this->SetActorLocation(Bottle->GetActorLocation());
			// Set this actor already on the constraint, so it also gets the usal red color for the first actor.
			this->GetConstraintComp()->ConstraintActor1 = Bottle;
		}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ARealisticOpeningActor, Lid) && ActiveOpeningConstraint) 
		{
			// Set this actor already on the constraint, so it also gets the usal blue color for the second actor.
			this->GetConstraintComp()->ConstraintActor2 = Lid;
		}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ARealisticOpeningActor, ActiveOpeningConstraint)) 
		{
			// If the Opening constraint should be active set everything for the bottle opening.
			if (ActiveOpeningConstraint) 
				{
					BottleOpening();
				}
		}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ARealisticOpeningActor, RotateDirectionRight)) 
		{
			// If the Opening constraint should be active set everything for the bottle opening.
			// Will set it the way it's needed for the rotation way.
			if (ActiveOpeningConstraint) 
			{
				BottleOpening();
			}
		}
}