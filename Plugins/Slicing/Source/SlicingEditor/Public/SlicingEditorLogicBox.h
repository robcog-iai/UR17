#pragma once

#include "Components/BoxComponent.h"
#include "Tickable.h"

class SlicingEditorLogicBox : public UBoxComponent, public FTickableGameObject
{
	public:
		SlicingEditorLogicBox();

		virtual void TickComponent(float DeltaTime,
			enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction) override;

		// Overlap Functions
		UFUNCTION()
		void OnBladeBeginOverlap(
			UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);

		UFUNCTION()
			void OnBladeEndOverlap(
				UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
				UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};