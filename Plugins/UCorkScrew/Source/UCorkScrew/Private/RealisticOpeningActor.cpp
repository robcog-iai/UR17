// Fill out your copyright notice in the Description page of Project Settings.

#include "RealisticOpeningActor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/SphereComponent.h"

#include "EngineGlobals.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "DrawDebugHelpers.h"

// Sets default values.
ARealisticOpeningActor::ARealisticOpeningActor() 
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (bIsActiveOpeningConstraint) 
		{
			BottleOpening();
			// Creating the overlapping sphere.
			LidCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Lid Collision Sphere Component"));
			LidCollisionSphere->InitSphereRadius(SphereRadius);
			LidCollisionSphere->SetCollisionProfileName("Trigger");
			// Adding the overlapping method to the overlapping sphere.
			LidCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ARealisticOpeningActor::OnOverlap);
		}
	
}

// Sets default values.
ARealisticOpeningActor::ARealisticOpeningActor(const FObjectInitializer & ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (bIsActiveOpeningConstraint) 
		{
			BottleOpening();
			// Creating the overlapping sphere.
			LidCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Lid Collision Sphere Component"));
			LidCollisionSphere->InitSphereRadius(SphereRadius);
			LidCollisionSphere->SetCollisionProfileName("Trigger");
			// Adding the overlapping method to the overlapping sphere.
			LidCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ARealisticOpeningActor::OnOverlap);
	}
}

// Called when the game starts or when spawned. Also sets the default values.
void ARealisticOpeningActor::BeginPlay()
{
	Super::BeginPlay();
	if (bIsActiveOpeningConstraint) 
		{
			BottleOpening();
		}
}

// Called every frame.
void ARealisticOpeningActor::Tick(float DeltaTime)
{
	// Local variable to save real current degree value.
	float currentDegree;
	// If the rotate direction is to the right.
	if (bIsRotateDirectionRight) 
		{
			// Calculate the current Degree for the direction to the right.
			currentDegree = GetDegreeCalcRight();
		}
		// Otherwise:
		else 
			{
				// Calculate the current Degree for the direction to the left.
				currentDegree = GetDegreeCalcLeft();
			}
	// Move and if necessary break the constraint given by the current degree.
	MovingAndBreaking(currentDegree);
	// If the debug mode is on:
	if (bIsDebuggingModeOn) {
		if (bIsBroken && TicksCounter == 100 && bIsTurnLidOnAgain)
			{
				DrawDebugSphere(GetWorld(), GetActorLocation(), SphereRadius, 20, FColor::Red, false, -1, 0, 1);
			}
			else if (bIsTurnLidOnAgain && bIsDebuggingModeOn && TicksCounter < 100)
			{
				TicksCounter++;
			}
		// Count every tick.
		TickCounter++;
		// Do every second tick:
		if (TickCounter % 2 == 0) {
			// If the MoveToDegree value is bigger or equals the currentDegree + 1 value while the opening direction is right
			// or if the MoveToDegree value is smaller or equals the currentDegree - 1 value while the opening direction is left.
			// And to both ors: The constraint shouldn't be broken.
			if (((MoveToDegree >= (currentDegree + 1) && bIsRotateDirectionRight) || (MoveToDegree <= (currentDegree - 1) && !bIsRotateDirectionRight)) && !bIsBroken)
				{
					// Rotate to the right.
					this->GetConstraintComp()->ConstraintActor2->AddActorLocalRotation(FRotator::FRotator(0, -TurningSpeed, 0));
				}
				// If the MoveToDegree value is smaller or equals the currentDegree - 1 value while the opening direction is right
				// or if the MoveToDegree value is bigger or equals the currentDegree + 1 value while the opening direction is left.
				// And to both ors: The constraint shouldn't be broken.
				else if (((MoveToDegree <= (currentDegree - 1) && bIsRotateDirectionRight) || (MoveToDegree >= (currentDegree + 1) && !bIsRotateDirectionRight)) && !bIsBroken)
					{
						// Rotate to the left.
						this->GetConstraintComp()->ConstraintActor2->AddActorLocalRotation(FRotator::FRotator(0, TurningSpeed, 0));
					}
			// If still a Degree is missing, do same as above again.
			if ((MoveToDegree - currentDegree) <= 1 && (MoveToDegree - currentDegree) > 0) {
				if (((MoveToDegree >= currentDegree && bIsRotateDirectionRight) || (MoveToDegree <= currentDegree && !bIsRotateDirectionRight)) && !bIsBroken)
				{
					this->GetConstraintComp()->ConstraintActor2->AddActorLocalRotation(FRotator::FRotator(0, -TurningSpeed, 0));
				}
				else if (((MoveToDegree <= currentDegree && bIsRotateDirectionRight) || (MoveToDegree >= currentDegree && !bIsRotateDirectionRight)) && !bIsBroken)
				{
					this->GetConstraintComp()->ConstraintActor2->AddActorLocalRotation(FRotator::FRotator(0, TurningSpeed, 0));
				}
			}
		}
		// If the log should be saved and the object has been moved more than 1 degree:
		if (bIsSaveLogsOn && OldCurrentDegreeForDebug+1 < currentDegree || OldCurrentDegreeForDebug-1 > currentDegree)
			{
				// Write stuff to the log.
				UE_LOG(LogTemp, Display, TEXT("Current Degree: %f"), currentDegree);
				UE_LOG(LogTemp, Display, TEXT("Move To Angle: %f"), MoveToDegree);
				UE_LOG(LogTemp, Display, TEXT("Current Degree: %f"), currentDegree);
				UE_LOG(LogTemp, Display, TEXT("Old Current Degree: %f"), OldCurrentDegreeForDebug);
				UE_LOG(LogTemp, Display, TEXT("Current Swing1 Degree: %f"), this->GetConstraintComp()->GetCurrentSwing1());
				UE_LOG(LogTemp, Display, TEXT("Turn Counter: %d"), TurnCounter);
				UE_LOG(LogTemp, Display, TEXT("Turn flag is: %d"), bIsTurnFlagOn);
				
				// The currentDegree will be the OldCurrentDegreeForDebug in the next run.
				OldCurrentDegreeForDebug = currentDegree;
			}
	}
}

// Was necessary for extending the APhysicsConstraintActor.
void ARealisticOpeningActor::LoadedFromAnotherClass(const FName & OldClassName) 
{
}

// Was necessary for extending the APhysicsConstraintActor.
void ARealisticOpeningActor::PostLoad() 
{
	AActor::PostLoad();
}

// Sets the default Values.
void ARealisticOpeningActor::BottleOpening() 
{	
	// If the Opening Constraint is Active.
	if (bIsActiveOpeningConstraint) 
		{
			// Sets the constraint limit, also checks in which direction it should be turnable.
			ActivateConstraintLimit();
			// If the upmoving distance is smaller than 0.25 like 0, we still need a Radius for the OverlapSphere.
			if (UpMovingDistance <= 10.0f) 
				{
					// Then set it on 0.25f.
					SphereRadius = 10.0f;
				} 
				else 
				{
					SphereRadius = UpMovingDistance;
				}
			// if the debug mode is on set the TicksCounter for overlapdebugging on 0 otherwise on 100.
			if (bIsDebuggingModeOn)
				{
					TicksCounter = 0;
				}
				else
				{
					TicksCounter = 100;
				}
				
			// If Bottle isn't a null pointer.
			if (Bottle != nullptr) 
				{
					// Set Actor Location on the Bottles location.
					this->SetActorLocation(Bottle->GetActorLocation());
					// Set the Actors Rotation on the Bottles rotation.
					this->SetActorRotation(Bottle->GetActorRotation());
					// Set the Constraint Actor 1 on the Bottle.
					this->GetConstraintComp()->ConstraintActor1 = Bottle;
					// If the Lid isn't a null pointer.
					if (Lid != nullptr) 
						{
							// Set the Constarint Actor 2 on the lid.
							this->GetConstraintComp()->ConstraintActor2 = Lid;
						}
				}
			// bIsTurnFlag is by default false.
			bIsTurnFlagOn = false;
			// Getting the distance to move up per degree.
			UpMovingDistancePerDegree = UpMovingDistance / DestroyAngle;
			// Setting the OldSwing1Degree by default on the actual one.
			OldSwing1Degree = this->GetConstraintComp()->GetCurrentSwing1();
			// Setting the OldSwingDegreeForDebug by default on the actual one.
			OldSwingDegreeDebug = this->GetConstraintComp()->GetCurrentSwing1();
			// Setting the OldCurrentDegreeForDebug on 0.
			OldCurrentDegreeForDebug = 0;
			// Setting the TickCounter by default on 0.
			TickCounter = 0;
			// Setting the OldCalculatedOver180Degrees by default on 0.
			OldCalculatedOver180Degrees = 0;
			// Set bIsBroken by default on false.
			bIsBroken = false;
			// TurnCounter on default on value 0.
			TurnCounter = 0;
	}
}

void ARealisticOpeningActor::OnOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// In case the debug mode is deactivated the standard value of TicksCounter is 100. So it has no effect.
	// Otherwise there will be a delay on the overlap.
	// Also checks if the other actor is the used Lid.
	if (this->bIsBroken && TicksCounter >= 100 && OtherActor != this && OtherActor != nullptr)
		{
			// Sets the Lids rotation on the actual constraints rotation.
			this->Lid->SetActorRotation(this->GetActorRotation(), ETeleportType::TeleportPhysics);
			// Getting the exact vector to the new position for the lid.
			// 180 is a integer to get a integer value.
			FVector rotation = this->GetActorRotation().Vector() * (UpMovingDistancePerDegree * (DestroyAngle/180));
			// Getting the actual location of the constraint and calculate the new position of the lid.
			FVector newLidLocation = this->GetActorLocation() + rotation;
			// Sets the Lids location.
			this->Lid->SetActorLocation(newLidLocation);
			// Gets the Bottle.
			UPrimitiveComponent* rootBottle = Cast<UPrimitiveComponent>(Bottle->GetRootComponent());
			// Gets the Lid
			UPrimitiveComponent* rootLid = Cast<UPrimitiveComponent>(Lid->GetRootComponent());
			// Sets the ConstraintComponent Active again. 
			this->GetConstraintComp()->SetConstrainedComponents(rootBottle, "", rootLid, "");
			// Since there is no way to set Swing1 manually, just the last 180 degree pointer gets used. It is at least 180 Degrees.
			this->GetConstraintComp()->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Free, 360.f);
			// Sets the TurnCounter on the max possible.
			TurnCounter = DestroyAngle / 180;
			this->bIsBroken = false;
			this->bIsTurnFlagOn = true;
		}
}

// Sets the Limits of the Constraint.
void ARealisticOpeningActor::ActivateConstraintLimit() 
{
	// Swing 2 and Twist is should be limited.
	this->GetConstraintComp()->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0);
	this->GetConstraintComp()->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);
	// Locks movement at 30 degrees.
	this->GetConstraintComp()->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, 30.0f);
	// Depending on the opening direction.
	if (bIsRotateDirectionRight)
		{
			// Opening direction to the right.
			// Gives a 30 degree offset to make it impossible turning it to the left, since the 30 degree limit.
			this->GetConstraintComp()->ConstraintInstance.AngularRotationOffset = FRotator::FRotator(0, 30, 0);
		}
		else 
			{
				// Opening direction to the left.
				// Gives a -30 degree offset to make it impossible turning it to the left, since the 30 degree limit.
				this->GetConstraintComp()->ConstraintInstance.AngularRotationOffset = FRotator::FRotator(0, -30, 0);
			}
	// Sets a linear position drive for the up down moving on the Z-Axis.
	this->GetConstraintComp()->SetLinearPositionDrive(false, false, true);
	this->GetConstraintComp()->SetLinearPositionTarget(FVector::FVector(0, 0, 0));
	// Sets a linear velocity drive for the up down moving on the Z-Axis.
	this->GetConstraintComp()->SetLinearVelocityDrive(false, false, true);
	// Sets the linear drive parameters on 50000 to make it almost like a limited rotation.
	// This is set to the position strength and the velocity strength.
	this->GetConstraintComp()->SetLinearDriveParams(50000, 50000, 0);
	// Sets the velocity target direction.
	this->GetConstraintComp()->SetLinearVelocityTarget(FVector::FVector(0, 0, 0));
	// Sets the linear limit for the Z-Movement. Since it's not turned at all the limit is 0.
	this->GetConstraintComp()->SetLinearZLimit(ELinearConstraintMotion::LCM_Limited, 0);
	
	// Setting angular mode to twist and swing.
	this->GetConstraintComp()->SetAngularDriveMode(EAngularDriveMode::TwistAndSwing);
	// Sets the Twist movement drive on false and the swing drive on true.
	this->GetConstraintComp()->SetAngularVelocityDriveTwistAndSwing(false, true);
	// Sets the velocity and the position strength on 50000 to make both feel like its limited.
	this->GetConstraintComp()->SetAngularDriveParams(50000, 50000, 0);
}

// Degree calculation for the right opening direction.
float ARealisticOpeningActor::GetDegreeCalcRight()
{
	// Saving a current Swing1Degree value.
	float CurrentSwing1Degree = this->GetConstraintComp()->GetCurrentSwing1();
	// Sees how far it moves and in which direction by subtracting the old value of the current value.
	float ActualTurnedDegrees = CurrentSwing1Degree - OldSwing1Degree;
	// Sets the CalculatedOver180Degrees value default by 0.
	float CalculatedOver180Degrees = 0;
	// Sets the flag if its moved legit to false, because this should be proofed first.
	bool bIsLegitMove = false;
	// Checks if the rotation was more than 20 degrees, if it was its probably not a legit turn, since a human cant move that quick.
	// It gets checked every tick, so more than 20 degrees should be impossible.
	if(ActualTurnedDegrees <= 20 && ActualTurnedDegrees >= -20)
		{
			// if the actual rotate direction was to the right.
			if (ActualTurnedDegrees > 0)
				{
					// Check if the turn flag is set to make sure its not just wobbling a small bit over lower than -30 and back again
					// and checks if the old value was below -30 degrees and also the new value is above -30 degrees,
					// because it must have passed the -30 degree point then.
					if (bIsTurnFlagOn && OldSwing1Degree < -30 && CurrentSwing1Degree >= -30)
						{
							// Increment the TurnCounter.
							++TurnCounter;
						}	
						// Checks if the TurnFlag is set and if it has passed 150 Degree point like above.
						else if (bIsTurnFlagOn && OldSwing1Degree < 150 && CurrentSwing1Degree >= 150)
							{
								// Increment the TurnCounter.
								++TurnCounter;
							}
						// If the IsTurnFlagOn isn't and the 0 degree point is passed.
						else if(!bIsTurnFlagOn && OldSwing1Degree < 0 && CurrentSwing1Degree >= 0)
							{
								// Set the IsTurnFlagOn true.
								bIsTurnFlagOn = true;
								// Free the Swing 1 rotation limit.
								this->GetConstraintComp()->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Free, 360.f);
							}
				}
				// If the direction is to the left.
				else if (ActualTurnedDegrees < 0)
					{
						// If the TurnFlag is on to be sure its not just wobbling
						// and the -30 degree point got passed.
						if(bIsTurnFlagOn && OldSwing1Degree >= -30 && CurrentSwing1Degree <-30)
							{
								// Decrement the TurnCounter.
								--TurnCounter;
							}
							// Also checking if it is wobbling and if the 150 degree point has been passed.
							else if (bIsTurnFlagOn && OldSwing1Degree >= 150 && CurrentSwing1Degree < 150)
								{
									// Decrement the TurnCounter.
									--TurnCounter;
								}
							// If the Turnflag is on and TurnCounter is 0 already, also it passes the 0 again to the left side.
							else if(bIsTurnFlagOn && TurnCounter == 0 && OldSwing1Degree >= 0 && CurrentSwing1Degree < 0) 
								{
									// Deactivate the TurnFlag
									bIsTurnFlagOn = false;
									// Set the limit back again.
									this->GetConstraintComp()->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, 30.0f);
								}
					}
			// This was indeed a legit move so set the flag true.
			bIsLegitMove = true;
		}

	// If it just passes the 180 degree spot, while moving just 20 degrees, it is a legit move.
	if((CurrentSwing1Degree >= 170 && OldSwing1Degree <= -170) || (CurrentSwing1Degree <= -170 && OldSwing1Degree >= 170))
		{
			// Set the legit move flag true.
			bIsLegitMove = true;
		}

	// If it was a legit move and it is between -30 degrees and 180 degrees.
	if (bIsLegitMove && CurrentSwing1Degree >= -30 && CurrentSwing1Degree <= 180)
		{
			// calculate the real actual degree value.
			CalculatedOver180Degrees = 30 + CurrentSwing1Degree;
			// If its bigger than 180, subtract 180 degrees to get the degrees to get the degree value that is over 180.
			if (CalculatedOver180Degrees >= 180)
			{
				CalculatedOver180Degrees = CalculatedOver180Degrees - 180;
			}
		}
		// If it was a legit move and the turn flag is on, but it wasn't the move above.
		else if (bIsTurnFlagOn && bIsLegitMove)
			{
				// Since the actual CurrentSwing1Degree value must be below -30 its a negative value.
				// By adding 210 and the degree value the correct value, which should be negative, the correct value over 180 degrees gets calculated.
				CalculatedOver180Degrees = 210 + CurrentSwing1Degree;
			}
		// If it wasnt a legit move at all.
		else
			{
				// Set the OldCalculatedOver180Degrees value also as new value.
				CalculatedOver180Degrees = OldCalculatedOver180Degrees;
			}
	// Set the new CalculatedOver180Degrees value on the OldCalculatedOver180Degrees value for the next run.
	OldCalculatedOver180Degrees = CalculatedOver180Degrees;
	// Set the OldSwing1Degree on the CurrentSwing1Degree for the next run.
	OldSwing1Degree = CurrentSwing1Degree;

	// Calculate the actual real turned degrees and give it back.
	return CalculatedOver180Degrees + (TurnCounter * 180);
}

// Calculates the real degree value of a left turn opening. Is familar to the calculation of the right turn.
float ARealisticOpeningActor::GetDegreeCalcLeft() 
{
	// Saving a current Swing1Degree value.
	float CurrentSwing1Degree = this->GetConstraintComp()->GetCurrentSwing1();
	// Sees how far it moves and in which direction by subtracting the old value of the current value.
	float ActualTurnedDegrees = CurrentSwing1Degree - OldSwing1Degree;
	// Sets the CalculatedOver180Degrees value default by 0.
	float CalculatedOver180Degrees = 0;
	// Sets the flag if its moved legit to false, because this should be proofed first.
	bool bIsLegitMove = false;
	// Checks if the rotation was more than 20 degrees, if it was its probably not a legit turn, since a human cant move that quick.
	// It gets checked every tick, so more than 20 degrees should be impossible.
	if (ActualTurnedDegrees <= 20 && ActualTurnedDegrees >= -20)
	{
		// If the turning direction is to the right.
		if (ActualTurnedDegrees > 0)
		{
			// If the 30 degrees point gets passed.
			if (bIsTurnFlagOn && OldSwing1Degree < 30 && CurrentSwing1Degree >= 30)
			{
				// Decrement the TurnCounter.
				--TurnCounter;
			}
				// If the -150 degree point gets passed.
				else if (bIsTurnFlagOn && OldSwing1Degree < -150 && CurrentSwing1Degree >= -150)
				{
					// Decrement the TurnCounter.
					--TurnCounter;
				}
				// if the 0 degree point gets passed and the TurnCounter is on 0 already.
				else if (bIsTurnFlagOn && TurnCounter == 0 && OldSwing1Degree < 0 && CurrentSwing1Degree >= 0)
				{
					// The TurnFlagOn is false.
					bIsTurnFlagOn = false;
					// Limit gets set.
					this->GetConstraintComp()->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, 30.0f);
				}
		}
		// If the rotate direction is to the right.
		else if (ActualTurnedDegrees < 0)
		{
			// If the 30 degree point gets passed.
			if (bIsTurnFlagOn && OldSwing1Degree >= 30 && CurrentSwing1Degree <30)
			{
				// Increment the TurnCounter.
				++TurnCounter;
			}
			// If the -150 degree point gets passed.
			else if (bIsTurnFlagOn && OldSwing1Degree >= -150 && CurrentSwing1Degree < -150)
			{
				// Increment the TurnCounter.
				++TurnCounter;
			}
			// If the 0 degree point gets passed.
			else if (!bIsTurnFlagOn && OldSwing1Degree >= 0 && CurrentSwing1Degree < 0)
			{
				// Set Turning Flag on true.
				bIsTurnFlagOn = true;
				// Free the limit.
				this->GetConstraintComp()->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Free, 360.f);
			}
		}
		// This was a legit move.
		// Set the legit move flag true.
		bIsLegitMove = true;
	}

	// Passing the 180 Degree just by passing with 20 degrees is always a legit move.
	if ((CurrentSwing1Degree >= 170 && OldSwing1Degree <= -170) || (CurrentSwing1Degree <= -170 && OldSwing1Degree >= 170))
	{
		// Set the legit move flag true.
		bIsLegitMove = true;
	}

	// If the CurrentSwing1Degree is between -180 and 30 degrees.
	if (bIsLegitMove && CurrentSwing1Degree <= 30 && CurrentSwing1Degree >= -180)
	{
		// Real degree calculation.
		CalculatedOver180Degrees = 30 - CurrentSwing1Degree;
		// If its bigger than 180, subtract 180 degrees to get the degrees to get the degree value that is over 180.
		if (CalculatedOver180Degrees >= 180)
		{
			CalculatedOver180Degrees = CalculatedOver180Degrees -180;
		}
	}
	else if (bIsTurnFlagOn && bIsLegitMove)
	{
		// Since the actual CurrentSwing1Degree value must be above 30 its a positive value.
		// By subtracting 180 and the degree value the correct value above the 180 degree point gets calculated.
		CalculatedOver180Degrees = 180 - CurrentSwing1Degree;
	}
	else
	{
		// It wasn't a legit move, take the value from before.
		CalculatedOver180Degrees = OldCalculatedOver180Degrees;
	}
	// The new values becomes the old values on the next run.
	OldCalculatedOver180Degrees = CalculatedOver180Degrees;
	OldSwing1Degree = CurrentSwing1Degree;

	// Calculate the real turned degrees and return this value.
	return CalculatedOver180Degrees + (TurnCounter * 180);
}

// Function to move the lid and break the constraint when the screw angle is passed.
void ARealisticOpeningActor::MovingAndBreaking(float CurrentDegree) 
{
	// To move upwards.
	// Valvulate how far it should move up. Since the lid is on Constraint position 2 it should be a negative value to move upwards.
	float moveUp = -(CurrentDegree * UpMovingDistancePerDegree);
	// Set the new limit on the value how far it should be actually.
	this->GetConstraintComp()->SetLinearZLimit(ELinearConstraintMotion::LCM_Limited, -moveUp);
	// Move up.
	this->GetConstraintComp()->SetLinearPositionTarget(FVector::FVector(0, 0, moveUp));
	// Set the Drive.
	this->GetConstraintComp()->SetLinearVelocityDrive(false, false, true);

	// If it has reached or passed the destroy angle with the degrees, break it.
	if (CurrentDegree >= DestroyAngle) 
		{
			bIsBroken = true;
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
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ARealisticOpeningActor, Bottle) && bIsActiveOpeningConstraint) 
		{
			// Sets the location of the constraint on the position of the bottle automaticly.
			// It sets on the top of the bottle so it should stand.
			FVector origin, boxExtent;
			FVector newVector = Bottle->GetActorLocation();
			origin = Bottle->GetActorScale3D();
			Bottle->GetActorBounds(false, origin, boxExtent);
			newVector.Z = newVector.Z + boxExtent.Z;
			this->SetActorLocation(newVector);
			// Sets the rotation of the constraint on the same rotation the bottle has automaticly.
			this->SetActorRotation(Bottle->GetActorRotation());
			// Set this actor already on the constraint, so it also gets the usal red color for the first actor.
			this->GetConstraintComp()->ConstraintActor1 = Bottle;
			LidCollisionSphere->SetWorldLocation(newVector);
		}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ARealisticOpeningActor, Lid) && bIsActiveOpeningConstraint) 
		{
			// Set this actor already on the constraint, so it also gets the usal blue color for the second actor.
			this->GetConstraintComp()->ConstraintActor2 = Lid;
		}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ARealisticOpeningActor, bIsActiveOpeningConstraint)) 
		{
			// If the Opening constraint should be active set everything for the bottle opening.
			if (bIsActiveOpeningConstraint) 
				{
					BottleOpening();
				}
		}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ARealisticOpeningActor, bIsRotateDirectionRight)) 
		{
			// If the Opening constraint should be active set everything for the bottle opening.
			// Will set it the way it's needed for the rotation way.
			if (bIsActiveOpeningConstraint) 
			{
				BottleOpening();
			}
		}
}