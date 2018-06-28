# Milestone 3 - Clickable objects with menu and rotation

* The GameHUD class is the controller of the the three different menus that are currently available. To choose the right menu GameHUD has an instance UGPickup and for the correct position it receives the mouse position for drawing the menu. The menu is drawn by assigning the PickupUI and giving it the current mouse position. Over the GEngine PickupUI gets called and added to the viewport.  

```
void AGameHUD::DrawPickUpMenu(float MouseX, float MouseY)
{
	SAssignNew(PickupUI, SPickupUI).GameHUD(this);
	PickupUI->WidgetPosition = FVector2D(MouseX, MouseY);

	if (GEngine->IsValidLowLevel())
	{
		GEngine->GameViewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(PickupUI.ToSharedRef()));
	}
}
```  

The buttons are then removed with a call to the GEngine.

```
void AGameHUD::RemoveMenu()
{
	GEngine->GameViewport->RemoveAllViewportWidgets();
}
```
