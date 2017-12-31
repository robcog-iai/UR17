// Copyright 2017, Institute for Artificial Intelligence - University of Bremen
// Author: David Brinkmann

#define printTimelineInfo(text) if (GEngine) GEngine->AddOnScreenDebugMessage(1, 5.0f,FColor::Emerald,text, false);
#define SEMLOG_TAG "SemLog"

#include "StackChecker.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Engine.h" // Needed for GEngine
#include "TagStatics.h"
#include "SLUtils.h"
#include "../Private/Character/CharacterController.h"
#include "Components/StaticMeshComponent.h"



// Sets default values
AStackChecker::AStackChecker()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxDeviation = 20.0f;
	DelayBeforeStabilityCheck = 1.0f;
	bShowStabilityCheckWindow = true;
}

// Called when the game starts or when spawned
void AStackChecker::BeginPlay()
{
	Super::BeginPlay();

	// Get the player character controller from the world
	for (TActorIterator<ACharacterController>RMItr(GetWorld()); RMItr; ++RMItr)
	{
		PlayerCharacter = *RMItr;
		break;
	}


	// Find HUD
	ClickInteractionHUD = Cast<ACIHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	StartPosition = GetActorLocation();
	StartRotation = GetActorRotation();

	if (ShakingAnimationCurveVector != nullptr) {
		FOnTimelineVector ShakeFunction;
		FOnTimelineEvent OnDoneFunction;

		ShakeFunction.BindUFunction(this, FName("Shake"));
		TimelineShaking.AddInterpVector(ShakingAnimationCurveVector, ShakeFunction);
		TimelineShaking.SetLooping(false);

		OnDoneFunction.BindUFunction(this, FName("ShakeDone"));
		TimelineShaking.SetTimelineFinishedFunc(OnDoneFunction);
	}

	// ** Rotation via Vector
	if (RotationAnimationCurveVector != nullptr) {
		FOnTimelineVector RotateFunction;
		FOnTimelineEvent OnDoneFunction;

		RotateFunction.BindUFunction(this, FName("RotateVector"));
		TimelineRotation.AddInterpVector(RotationAnimationCurveVector, RotateFunction);
		TimelineRotation.SetLooping(false);

		OnDoneFunction.BindUFunction(this, FName("RotateDone"));
		TimelineRotation.SetTimelineFinishedFunc(OnDoneFunction);
	}

	// Set overall time of timelines
	if (TimelineShaking.GetTimelineLength() > TimelineRotation.GetTimelineLength()) {
		SumOfTimelineSeconds = TimelineShaking.GetTimelineLength();
	}
	else {
		SumOfTimelineSeconds = TimelineRotation.GetTimelineLength();
	}
}

// Called every frame
void AStackChecker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimelineShaking.TickTimeline(DeltaTime);
	TimelineRotation.TickTimeline(DeltaTime);

	if (bStackCheckIsRunning /*TimelineShaking.IsPlaying() || TimelineRotation.IsPlaying()*/) {
		SecondsLeft -= DeltaTime;
		printTimelineInfo(FString("Running stability test: ") + FString::SanitizeFloat(SecondsLeft));
	}
}

void AStackChecker::StartCheck(AActor * BaseItemToCheck)
{
	bStackCheckIsRunning = true;
	GenerateLogEvent(BaseItemToCheck);

	SetScreenCaptureEnabled(true);
	SecondsLeft = SumOfTimelineSeconds + DelayBeforeStabilityCheck;

	UE_LOG(LogTemp, Warning, TEXT("Starting stack check for %f seconds"), SecondsLeft);

	StackCopy = CopyStack(BaseItemToCheck);

	TimelineRotation.PlayFromStart();
	TimelineShaking.PlayFromStart();
}



void AStackChecker::RotateVector(FVector Vector)
{
	AddActorWorldRotation(FRotator(Vector.X, Vector.Y, Vector.Z));
}

void AStackChecker::RotateDone()
{
	if (TimelineShaking.IsPlaying() == false) {
		if (DelayBeforeStabilityCheck > 0) {
			GetWorld()->GetTimerManager().SetTimer(TimerHandlePauseBeforeCheck, this, &AStackChecker::EvaluateStabilityCheck, DelayBeforeStabilityCheck, false);
		}
		else {
			EvaluateStabilityCheck();
		}
	}

}

void AStackChecker::Shake(FVector Vector)
{
	AddActorWorldOffset(Vector);
}

void AStackChecker::ShakeDone()
{
	if (TimelineRotation.IsPlaying() == false) {
		if (DelayBeforeStabilityCheck > 0) {
			GetWorld()->GetTimerManager().SetTimer(TimerHandlePauseBeforeCheck, this, &AStackChecker::EvaluateStabilityCheck, DelayBeforeStabilityCheck, false);
		}
		else {
			EvaluateStabilityCheck();
		}
	}

}

void AStackChecker::EvaluateStabilityCheck()
{
	UE_LOG(LogTemp, Warning, TEXT("Delayed strated"));
	CheckStackRelativePositions();
	SetActorLocationAndRotation(StartPosition, StartRotation);
}


void AStackChecker::CheckStackRelativePositions()
{
	bool bIsSuccess = true;

	for (auto& StackedItem : PositionsOnStart) {

		FVector RelativePosition = StackedItem.Key->GetActorLocation() - StackCopy->GetActorLocation();

		float OldDistance = StackedItem.Value.Size();
		float NewDistance = RelativePosition.Size();

		float DeltaDistance = NewDistance - OldDistance;
		if (DeltaDistance > MaxDeviation) {
			UE_LOG(LogTemp, Warning, TEXT("AStackChecker::CheckStackRelativePositions: Stack test failed. Stack unstable. Item: %s Distance: %s"), *StackedItem.Key->GetName(), *FString::SanitizeFloat(DeltaDistance));
			bIsSuccess = false;
			break;
		}
	}

	OnStackCheckDone.Broadcast(bIsSuccess);

	StackCopy->Destroy();
	for (auto& StackedItem : PositionsOnStart) {
		StackedItem.Key->Destroy();
	}
	PositionsOnStart.Empty();

	UE_LOG(LogTemp, Warning, TEXT("AStackChecker::CheckStackRelativePositions: Test done"));

	if (bIsSuccess) {
		printTimelineInfo("Stability test done... success");
	}
	else {
		printTimelineInfo("Stability test done... failed");
	}

	SetScreenCaptureEnabled(false);

	bStackCheckIsRunning = false;
	FinishLogEvent(bIsSuccess);

}

AStaticMeshActor* AStackChecker::CopyStack(AActor * ActorToCopy)
{
	AStaticMeshActor* BaseCopy = CopyActor(ActorToCopy);

	FAttachmentTransformRules TransformRules = FAttachmentTransformRules::KeepRelativeTransform;
	TransformRules.bWeldSimulatedBodies = true;

	BaseCopy->GetStaticMeshComponent()->SetSimulatePhysics(true);

	TArray<AActor*> ChildActors;
	ActorToCopy->GetAttachedActors(ChildActors);

	for (auto& Child : ChildActors) {
		if (BaseCopy == nullptr) break;

		AStaticMeshActor* ChildCopy = CopyActor(Child);

		ChildCopy->GetStaticMeshComponent()->SetSimulatePhysics(true);
		ChildCopy->AttachToActor(BaseCopy, TransformRules);

		FVector RelativeLocation = Child->GetActorLocation() - ActorToCopy->GetActorLocation();
		FVector NewPosition = BaseCopy->GetActorLocation() + RelativeLocation;

		ChildCopy->SetActorLocation(NewPosition);
		ChildCopy->SetActorRotation(Child->GetActorRotation());

		PositionsOnStart.Add(ChildCopy, RelativeLocation);
	}

	BaseCopy->AttachToActor(this, TransformRules);
	BaseCopy->SetActorRelativeLocation(FVector::ZeroVector);
	BaseCopy->SetActorRelativeRotation(FRotator::ZeroRotator);

	BaseCopy->GetStaticMeshComponent()->UnWeldChildren();

	return BaseCopy;
}

AStaticMeshActor * AStackChecker::CopyActor(AActor * ActorToCopy)
{
	AStaticMeshActor* CastActor = Cast<AStaticMeshActor>(ActorToCopy);
	if (CastActor == nullptr) return nullptr;

	AStaticMeshActor* CopyActorBase = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
	CopyActorBase->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	CopyActorBase->GetStaticMeshComponent()->SetStaticMesh(CastActor->GetStaticMeshComponent()->GetStaticMesh());

	CopyActorBase->SetActorScale3D(ActorToCopy->GetActorScale3D());
	CopyActorBase->SetActorLocation(ActorToCopy->GetActorLocation());
	CopyActorBase->SetActorRotation(ActorToCopy->GetActorRotation());

	TArray<UMaterialInterface*> Materials = CastActor->GetStaticMeshComponent()->GetMaterials();
	for (size_t i = 0; i < Materials.Num(); i++)
	{
		CopyActorBase->GetStaticMeshComponent()->SetMaterial(i, Materials[i]);
	}

	return CopyActorBase;
}

void AStackChecker::SetScreenCaptureEnabled(bool bEnabled)
{
	if (ClickInteractionHUD != nullptr) {
		ClickInteractionHUD->bStabilityCheckScreenEnabled = bEnabled;
	}
}

void AStackChecker::GenerateLogEvent(AActor * BaseItemToCheck)
{
	// Log Pickup event
	const FString ItemClass = FTagStatics::GetKeyValue(BaseItemToCheck, SEMLOG_TAG, "Class");
	const FString ItemID = FTagStatics::GetKeyValue(BaseItemToCheck, SEMLOG_TAG, "Id");

	// Create contact event and other actor individual
	const FOwlIndividualName OtherIndividual("log", ItemClass, ItemID);
	const FOwlIndividualName ContactIndividual("log", "StackChecking", FSLUtils::GenerateRandomFString(4));

	// Owl prefixed names
	const FOwlPrefixName RdfType("rdf", "type");
	const FOwlPrefixName RdfAbout("rdf", "about");
	const FOwlPrefixName RdfResource("rdf", "resource");
	const FOwlPrefixName RdfDatatype("rdf", "datatype");
	const FOwlPrefixName TaskContext("knowrob", "taskContext");
	const FOwlPrefixName InContact("knowrob_u", "objectActedOn");
	const FOwlPrefixName OwlNamedIndividual("owl", "NamedIndividual");

	// Owl classes
	const FOwlClass XsdString("xsd", "string");
	const FOwlClass TouchingSituation("knowrob_u", "StackChecking");

	TArray <FOwlTriple> Properties;
	Properties.Add(FOwlTriple(RdfType, RdfResource, TouchingSituation));
	Properties.Add(FOwlTriple(TaskContext, RdfDatatype, XsdString, "-- Stack Checking -- "));
	Properties.Add(FOwlTriple(InContact, RdfResource, OtherIndividual));

	StackCheckLogEvent = MakeShareable(new FOwlNode(OwlNamedIndividual, RdfAbout, ContactIndividual, Properties));
	
	if (PlayerCharacter != nullptr && PlayerCharacter->LogComponent != nullptr) PlayerCharacter->LogComponent->StartEvent(StackCheckLogEvent);

}

void AStackChecker::FinishLogEvent(bool bStackCheckSuccessful)
{
	if (StackCheckLogEvent.IsValid()) {
		const FOwlPrefixName RdfDatatype("rdf", "datatype");
		const FOwlPrefixName TaskContext("knowrob", "taskContext");
		const FOwlClass XsdString("xsd", "string");

		FString SucessText = bStackCheckSuccessful ? "Success" : "Failed";
		StackCheckLogEvent.Get()->Properties.Add(FOwlTriple(TaskContext, RdfDatatype, XsdString, " -- Stack Checking -- " + SucessText));

		if (PlayerCharacter->LogComponent != nullptr) PlayerCharacter->LogComponent->FinishEvent(StackCheckLogEvent);
		StackCheckLogEvent = nullptr;
	}
}

