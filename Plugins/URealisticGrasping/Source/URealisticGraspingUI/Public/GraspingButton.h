// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetTree.h"
#include "TextWidgetTypes.h"
#include "TextBlock.h"
#include "Button.h"
#include "CanvasPanel.h"
#include "Widget.h"
#include "CanvasPanelSlot.h"
#include "ButtonSlot.h"
#include "UnitConversion.h"
#include "Blueprint/UserWidget.h"
#include "GraspingButton.generated.h"

/**
 * 
 */
UCLASS()
class UREALISTICGRASPINGUI_API UGraspingButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//The NativeConstruct call must be used to create a widget because GraspButton is a subclass of UUserWidget
	virtual void NativeConstruct() override;
	
	//In this function, the widget is created and returned
	virtual TSharedRef<SWidget>RebuildWidget();


	//The button text is displayed in this textbox
	UTextBlock* Textbox;	
};
