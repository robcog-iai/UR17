// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

/**
* Provides an implementation of the game’s HUD, which will embed and respond to events triggered
* within SMainMenuUI.
*/
UCLASS()
class UGAME_API AGameHUD : public AHUD
{
	GENERATED_BODY()

	// Initializes the Slate UI and adds it as widget content to the game viewport.
	virtual void PostInitializeComponents() override;

	// Reference to the Main Menu Slate UI.
	TSharedPtr<class SRotationUI> RotationUI;

	// Reference to the Main Menu Slate UI.
	TSharedPtr<class SPickupUI> PickupUI;
public:
	AGameHUD();

	// Called by SMainMenu whenever the Play Game! button has been clicked.
	UFUNCTION(BlueprintImplementableEvent, Category = "Menus|Game Menu")
		void PlayGameClicked();

	// Called by SMainMenu whenever the Quit Game button has been clicked.
	UFUNCTION(BlueprintImplementableEvent, Category = "Menus|Game Menu")
		void QuitGameClicked();

	/** Set the selected Acotr for whom the menu should be drawn.
	*   @param Selected actor
	*/
	void SetActionGridActor(AActor* InSelectedActor);

	/** gets player controller */
	class AGameController* GetPlayerController() const;

protected:
	// This will be drawn at the center of the screen.
	UPROPERTY(EditDefaultsOnly)
		UTexture2D* CrosshairTexture;

	/** Actor for whom the menu should be displayed*/
	TWeakObjectPtr<AActor> SelectedActor;

public:
	// Primary draw call for the HUD.
	virtual void DrawHUD() override;
	
	/** Draw menu to ask for rotation*/
	void DrawMenu();

	/** Remove menus on screen*/
	void RemoveMenu();

	/** Draw PickUp menu to ask for rotation*/
	void DrawPickUpMenu();

	/** Position on screen */
	FVector2D MenuPosition;
};