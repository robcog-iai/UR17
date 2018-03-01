// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "Components/BoxComponent.h"
#include "SlicingComponent.generated.h"

UCLASS()
class SLICINGLOGIC_API USlicingComponent: public UBoxComponent
{
	GENERATED_BODY()

public:
	// Sets default values. Called when generated, even in the editor.
	USlicingComponent();

public:
	// The names of the sockets that are also needed by the editor
	static const FName SocketHandleName;
	static const FName SocketBladeName;
	static const FName SocketCuttingExitpointName;
	// The tag names needed for recognition of objects related to the slicing plugin
	static const FName TagCuttable;

	// Called before BeginPlay()
	virtual void InitializeComponent() override;
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool bIsCutting = false;
	
	UStaticMeshComponent* Parent;

	FVector relLocation;
	FQuat relRotation;

	UProceduralMeshComponent* OComponent;
	// The rest of the functions
	UFUNCTION()
	void OnBladeBeginOverlap(
		UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBladeEndOverlap(
		UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp,	int32 OtherBodyIndex);

	bool bWrongCutting = false;
};