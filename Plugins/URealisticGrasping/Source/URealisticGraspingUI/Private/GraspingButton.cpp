// Fill out your copyright notice in the Description page of Project Settings.

#include "GraspingButton.h"



void UGraspingButton::NativeConstruct()
{
	//Call the base class method within this class to override it
	Super::NativeConstruct();
}

	TSharedRef<SWidget>UGraspingButton::RebuildWidget()
	{
		
		//Calls the root UObject widget wrapper and casts it to a UPanelWidget
		//It has to be done to override the root widget
		//It will later serve as new root widget
		UPanelWidget* RootWidget = Cast<UPanelWidget>(GetRootWidget());

		//Check if Rootwidget is created
		//Construction of the root widget. This root widget is a UButton (root widgets are UCanvasPanels by default)
		//Widgets need slots to add more items. In addition, all widget layout properties are handled in the widgets slot
		//Set RootWidget as root widget. The root widget serves as the basic element into which other widget elements can be inserted 
		if (!RootWidget)
		{
			RootWidget = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("RootWidget"));
			UButtonSlot* RootWidgetSlot = Cast<UButtonSlot>(RootWidget->Slot);
			WidgetTree->RootWidget = RootWidget;
		}

		//The underlying SWidget needs to be created because it allows all subclasses of UWidget to call functions
		TSharedRef<SWidget>Widget = Super::RebuildWidget();

		//Check if Rootwidget is created and the WidgetTree is set
		//Testbox is created and added to the button
		//The name of the button is set as text in the textbox. The button gets its name from its creation in class GraspingMenu
		//Here you can also determine the color and font of the text
		if (RootWidget && WidgetTree)
		{
			//Has to be executed in this order
		    Textbox = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), this->GetFName()); 
			RootWidget->AddChild(Textbox);
			Textbox->SetText(FText::FromString(this->GetFName().ToString()));
		}
		
		//Return the created widget
		return Widget;
	}

	