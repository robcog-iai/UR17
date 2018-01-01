#pragma once

#include "Components/BoxComponent.h"
#include "Tickable.h"

UCLASS()
class SlicingEditorLogicBox : public UBoxComponent, public FTickableGameObject
{
	public:
		bool* bShowDebugPlane = false;

		virtual void TickComponent(float DeltaTime,
			enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction) override;
		virtual bool IsTickable() const override;
		virtual TStatId GetStatId() const override;

		// Overlap Functions
		UFUNCTION()
		void OnBladeBeginOverlap(
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult & SweepResult);

		UFUNCTION()
			void OnBladeEndOverlap(
				UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
				UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};