// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
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
	// Called before BeginPlay()
	virtual void InitializeComponent() override;
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool bIsCutting = false;

	// The rest of the functions
	UFUNCTION()
	void OnBladeBeginOverlap(
		UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBladeEndOverlap(
		UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp,	int32 OtherBodyIndex);
};