// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetTree.h"
#include "CanvasPanel.h"
#include "CanvasPanelSlot.h"
#include "GraspingButton.h"
#include "ScrollBox.h"
#include "ScrollBoxSlot.h"
#include "UnitConversion.h"
#include "Engine.h"
#include "Blueprint/UserWidget.h"
#include "GraspingMenu.generated.h"

/**
 * 
 */
UCLASS()
class UREALISTICGRASPINGUI_API UGraspingMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//The NativeConstruct call must be used to create a widget because GraspButton is a subclass of UUserWidget
	virtual void NativeConstruct() override;

	//In this function, the widget is created and returned
	virtual TSharedRef<SWidget>RebuildWidget();

	//This function should delay the creation.Because the GraspNameArray has to be set first, so that the buttons have a caption
	void CreateMenu();

	//Dyes the button of the selected handle and sets the previously colored button back to the default color
	//@param GraspIndex, the index indicates which button is selected
	void SwitchGraspType(int* GraspIndex);

	//Sets the array GraspNameArray
	//@param AnimationArray, the array with the grasp names. The number of buttons is determined and the texts of the buttons are set
	void SetArray(TArray<FString> AnimationArray);

	//Is true when the "CreateMenu" function was called
	bool bIsCalled = false;
	
	//Box element in which the buttons are located
	UScrollBox* ScrollBoxMenu;
	
	//Array in which the created buttons are stored
	TArray<UGraspingButton*> ButtonArray;
	
	//Array in which the texts of the button are stored. Set in the "SetArray" function
	TArray<FName>GraspNameArray;
};
