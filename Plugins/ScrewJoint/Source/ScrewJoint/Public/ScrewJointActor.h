// Copyright 2017-2018 Kannan Thambiah. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ScrewJointActor.generated.h"

class UPhysicsConstraintComponent;
class AStaticMeshActor;
class USphereComponent;
class AActor;

// TODO: Rename to ScrewJointActor in accordance with UE4 Engine

/**
* A ScrewJointActor is an object that connects two MeshActors representing container and lid like a screw thread,
* by initializing a PhysicsConstraintComponent in a certain way. Lid and screw have to be attached in Editor-Time
* and can afterwards be detached and reattached durcing runtime.
*
* @see https://docs.unrealengine.com/en-us/Engine/Physics/Constraints/ConstraintsReference
* @see https://docs.unrealengine.com/en-us/Programming/UnrealArchitecture/Actors
*/
UCLASS(meta = (ShortTooltip = "A ScrewJointActor is an object that connects two MeshActors representing container and lid like a screw thread"))
class SCREWJOINT_API AScrewJointActor : public AActor
{
	GENERATED_BODY()

		// --- Methods --- //
public:
	/**
	* Default constructor for AScrewJointActor
	*/
	AScrewJointActor();

	/**
	* Called when a property is changed from the Editor to recalculate dependencies and produce debugging output
	*/
	void PostEditChangeProperty(FPropertyChangedEvent & PropretyChangedEvent) override;

	/**
	* Called every frame to register rotation changes in StaticMeshActor of lid to determine if container is opened/closed
	*/
	virtual void Tick(float DeltaTime) override;

protected:
	/**
	* Called when the game starts, or ScrewJointActor is spawned to safe initial state of variables need for calculations
	*/
	virtual void BeginPlay() override;

private:
	/**
	* Gets called when all needed objects are initialized so that a constraint and helping objects can be created.
	*/
	void InitializeConstraint();

	/**
	* Position helping collision sphere to the right positions.
	*/
	void PositionSphere(USphereComponent* Sphere, const AStaticMeshActor* StaticMeshActor, const float ZAxisFactor);

	/**
	* Calculate radius of the spheres in proportion to the lid by approximating lid diameter.
	*/
	void SetSphereRadius();;

	/**
	* Method helping pritning out debug messages
	*/
	void PrintDebugLog(FString string) const;

	/**
	* Called when an object enteres the collision sphere of the lid;
	* if the collision is caused by the collision sphere of the container, flags are set to allow closing the object again
	*/
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	* Called when an object leafs the collision sphere of the lid;
	* if this is the collision sphere of the container, flags are set to make sure, they cannot be connected again
	*/
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	// --- Variables --- //
public:
	/**
	* Flag to controll if debugging output is printed on screen
	*/
	UPROPERTY(EditAnywhere, Category = "ScrewJoint|Debugging")
		bool bShowDebugLog;

	/**
	* Flag to indicate if lid and container are connected with each other
	*/
	UPROPERTY(VisibleAnywhere, Category = "ScrewJoint|Debugging")
		bool bIsOpen;

	/**
	* StaticMeshActor representind the lid part of the object
	*/
	UPROPERTY(EditAnyWhere, Category = "ScrewJoint|Components")
		AStaticMeshActor* Lid;

	/**
	* StaticMeshActor representind the container part of the object
	*/
	UPROPERTY(EditAnyWhere, Category = "ScrewJoint|Components")
		AStaticMeshActor* Container;

	/**
	* The (fraction of a) turn you need to open an object, i.e. 0.5 = 180°, 2 = 720°, etc.
	* This value can be provided directly or it will be calculated if Turn is set.
	* As for this version, only values from 0-360° are supported.
	*/
	UPROPERTY(EditAnywhere, Category = "ScrewJoint|JointParameters|Rotation", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float Turn;

	/**
	* The angle that has to be reached in order to have opened the lid.
	* This value can be provided directly or it will be calculated if Turn is set.
	* As for this version, only values from 0-360° are supported.
	*/
	UPROPERTY(EditAnywhere, Category = "ScrewJoint|JointParameters|Rotation", meta = (ClampMin = "0.0", ClampMax = "360.0"))
		float Angle;

	/**
	* Defines the handedness of the screw joint. Standard lid screws are usually righthanded,
	* i.e. when turning them counterclockwise they move upwards.
	* However, if needed a screw joint can be created that allows lefthanded joints
	* @see https://en.wikipedia.org/wiki/Screw_thread#Handedness
	*/
	UPROPERTY(EditAnywhere, Category = "ScrewJoint|JointParameters|Rotation")
		bool bRightHanded;

	/**
	* This value represents the height of the threaded area of the joint.
	* It is used to approximate the Lead for the axial advancement.
	* With the lead being an engineering dimension it is probably not known for objects of every day use
	* and it's determination would be unfeasable.
	* This value can be set directly, or it will be calculated, when lead is set.
	* @see https://en.wikipedia.org/wiki/Lead_(engineering)
	*/
	UPROPERTY(EditAnywhere, Category = "ScrewJoint|JointParameters|Travel", meta = (ClampMin = "0.0"))
		float ThreadHeight;

	/**
	* This value represents the axial advance of a screw during one complete turn.
	* It can be set directly, if that value is know, or it will be approximated by the ThreadHeight and the Turn values
	* @see https://en.wikipedia.org/wiki/Lead_(engineering)
	*/
	UPROPERTY(EditAnywhere, Category = "ScrewJoint|JointParameters|Travel", meta = (ClampMin = "0.0"))
		float Lead;

	/**
	* Higher damping that simulates the initial force needed to loosen a tightly screwed-on lid
	*/
	UPROPERTY(EditAnywhere, Category = "ScrewJoint|JointParameters|Force", meta = (ClampMin = "0.0"))
		float InitialForce;

	/**
	* Percentage of the angle needed to screw open, that the initial force has to be applied to
	*/
	UPROPERTY(EditAnywhere, Category = "ScrewJoint|JointParameters|Force", meta = (ClapMin = "0.0"))
		float ForceThreshold;

	/**
	* Standard damping when the lid is attached but sitting loose on the container
	*/
	UPROPERTY(EditAnywhere, Category = "ScrewJoint|JointParameters|Force", meta = (ClapMin = "0.0"))
		float StandardDamping;

private:
	/**
	* Constant to calculate from angles in degrees to fraction of a full turn describing the same angle
	* Eg. 90° <=> 0.25
	*/
	static const float Degree2Turns;

	/**
	* Constant to calculate from fraction of turn to angles in degrees describing the same partial turn
	* Eg. 0.5 <=> 180°
	*/
	static const float Turns2Degree;

	/**
	* PhysicsConstraint object that realizes the constrained movement of the lid when attached
	*/
	UPhysicsConstraintComponent* PhysicsConstraint;

	/**
	* Collision sphere for lid
	*/
	USphereComponent* LidSphere;

	/**
	* Collision sphere for container
	*/
	USphereComponent* ContainerSphere;

	/**
	* Saving the lid rotation the second the lid reenteres the detaching position
	*/
	FRotator InitialLidRotation;

	/**
	* Saving the initial swing of the lid for calculating the angle traveled by user rotation
	*/
	float InitialSwing;

	/**
	* Flag indicating that a lid is set so that physical constraint can be initialized
	*/
	bool bLidSet;

	/**
	* Flag indicating that a container is set so that physical constraint can be initialized
	*/
	bool bContainerSet;

	/**
	* Variable needed to optimize debugging output
	*/
	float PrevSwing;

	/**
	* Flag needed to optimize debugging output
	*/
	bool bPrevIsOpen;

	/**
	* Flag to indicate if lid is sitting at the correct position on the container
	*/
	bool bLidOnContainer;
};