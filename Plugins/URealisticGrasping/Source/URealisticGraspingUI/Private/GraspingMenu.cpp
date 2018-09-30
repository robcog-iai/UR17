// Fill out your copyright notice in the Description page of Project Settings.


#include "GraspingMenu.h"


void UGraspingMenu::NativeConstruct()
{
	//Call the base class method within this class to override it
	Super::NativeConstruct();
}

void UGraspingMenu::CreateMenu()
{
	bIsCalled = true;
	RebuildWidget();
}

TSharedRef<SWidget>UGraspingMenu::RebuildWidget()
{
	//Check if CreateMenu was called
	//If false, no new widget will be created, the default widget will not be changed
	if (!bIsCalled)
	{
		TSharedRef<SWidget>Widget = Super::RebuildWidget();
		return Widget;
	}

	//Call the root UObject widget wrapper and casts it to a UPanelWidget
	//It has to be done to override the root widget
	//It will later serve  as new root widget
	UPanelWidget* RootWidget = Cast<UPanelWidget>(GetRootWidget());

	//Check if Rootwidget is created
	//Construction of the root widget. This root widget is a UCanvasPanels (root widgets are UCanvasPanels by default)
	//Widgets need slots to add more items. In addition, all widget layout properties are handled in the widgets slot
	//Set RootWidget as root widget. The root widget serves as the basic element into which other widget elements can be inserted 
	if (!RootWidget)
	{
		RootWidget = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootWidget"));
		UCanvasPanelSlot* RootWidgetSlot = Cast<UCanvasPanelSlot>(RootWidget->Slot);
		
		//Check if RootWidgetSlot is created and set
		//Scale this widget and scale fullscreen on resize
		if (RootWidgetSlot)
		{
			RootWidgetSlot->SetAnchors(FAnchors(0.f, 0.f, 100.f, 100.f));
			RootWidgetSlot->SetOffsets(FMargin(0.f, 0.f));
		}

		WidgetTree->RootWidget = RootWidget;
	}

	//The underlying SWidget needs to be created because it allows all subclasses of UWidget to call functions
	TSharedRef<SWidget>Widget = Super::RebuildWidget();

	//Check if Rootwidget is created and the WidgetTree is set
	//Creates a scrollbox and makes the scrollbar invisible
	//Adds the scrollbox to the root widget
	//Creates slots for the scrollbox and scales them
	//Has to be executed in this order!!
	if (RootWidget && WidgetTree)
	{
		ScrollBoxMenu = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("ScrollBox"));
		ScrollBoxMenu->SetScrollBarVisibility(ESlateVisibility::Hidden);
		
		//Creates a button for each element in the array GraspNameArray and adds it to the scrollbox
		//The default color for texts and buttons is set (you can also determine the color for texts and buttons)
		//Created button are stored in an array
		for(int i=0; i < GraspNameArray.Num();i++){
			
			UGraspingButton* NewButton = CreateWidget<UGraspingButton>(this, UGraspingButton::StaticClass(), GraspNameArray[i]);
			NewButton->SetColorAndOpacity(FLinearColor(1, 1, 1, 0.2));
			ScrollBoxMenu->AddChild(NewButton);
			ButtonArray.Add(NewButton);	
		}
		RootWidget->AddChild(ScrollBoxMenu);
		UCanvasPanelSlot* ScrollBoxSlot = Cast<UCanvasPanelSlot>(ScrollBoxMenu->Slot);
		ScrollBoxSlot->SetSize(FVector2D(500, 200));
	}
	
	//Return the created widget and overwrite the default widget
	return Widget;
}

void UGraspingMenu::SwitchGraspType(int* GraspIndex)
{
	//It is checked if the menu is visible
	//The current grasp gets a new color
	//The selected button is placed in the middle of the scrollbox
	if (this->IsVisible())
	{
		ButtonArray[*GraspIndex]->SetColorAndOpacity(FLinearColor(1, 0, 0, 0.2));
		ScrollBoxMenu->ScrollWidgetIntoView(ButtonArray[*GraspIndex],false,EDescendantScrollDestination::Center);
	}
}

void UGraspingMenu::SetArray(TArray<FString> AnimationArray)
{
	TArray<FName> Names;

	//The individual string elements are converted into FName elements
	for (FString Elem : AnimationArray)
	{
		Names.Add(FName(*Elem));
	}
	GraspNameArray = Names;
}

