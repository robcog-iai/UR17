# Milestone 2 - deadline 31.12.17

* Realize a roation of the objects, while they are hold.

# Documentation
* The functionality of the rotation is done in the CharacterController.h/cpp and the CPickup.h/cpp, in the Plugins/Source/Private/Character/Components folder. The class UCPickup handles the picking up and holding of objects and the ACharacterController class handles the whole functionality of the character.

* For the rotation the "Tab"-button was assigned. After pushing the button the function RotationMode is called which assagins a value to the RotationValue variable. This varibale can either be 0, 1 or 2. If the value is 0 no rotations happen, after one push of the "Tab"-button the value is set to 1 and now the object, if one is hold, can be rotated, a second push of the button sets the value to 2 and the object in the right hand can be rotated and after the third push the value is again 0. This could also be solved with an enum but this method works fine too. The setting of the RotationValue is handled in the UCPickup class.

* In the ACharacterController the main functionality of the rotation is handled. For this AplayerController is necessary, with which it is possible to get the mousposition and the character position. These both are needed to calculate the rotation which is done by the mouse. The rotation is handled in the Tick function of the class and it it always checked if the rotaton mode was activated and if so the current hand is checked.
  
* The following code shows how the rotaion is handled:
```
	// Rotate the object depending of the rotation mode
	if (PickupComponent->RotationValue != 0) {
		// Get mouse position on screen
		float XMouse;
		float YMouse;
		PlayerController->GetMousePosition(XMouse, YMouse);
		
		// Get Character position on screen
		FVector CharLoc = Character->GetActorLocation();
		FVector2D CharInScreen;
		PlayerController->ProjectWorldLocationToScreen(CharLoc, CharInScreen);


		// Get mouse position relative to the Character.
		FVector2D Result;
		Result.X = -(YMouse - CharInScreen.Y);
		Result.Y = XMouse - CharInScreen.X;


		// Get angle rotation and rotation Character
		float angleX = FMath::RadiansToDegrees(FMath::Acos(Result.X / Result.Size()));
		float angleY = FMath::RadiansToDegrees(FMath::Acos(Result.Y / Result.Size()));


		if (Result.Y < 0) {
			angleX = 360 - angleX;
		}


		if (Result.X < 0) {
			angleY = 360 - angleX;
		}


		FRotator rot(-angleY, -angleX, 0);
		if (PickupComponent->RotationValue == 1) {
			PickupComponent->ItemInLeftHand->SetActorRotation(rot);
		}
		else {
			PickupComponent->ItemInRightHand->SetActorRotation(rot);
		}


	}
  ```
# Tutorial

* The functionality can easily be tested by picking up an object in the game and pressing the tab button. Afterwards it is possible to rotated the object with horizontal and vertical movments of the mouse. At the moment the left or right mouse button have to be pressed before the rotation stiwches to the hold object. So if the left hand holds an object it is needed to press te left mouse button and afterwards the tab button to rotate the object and before rotating the right hand object the object has also to be activated with the right mouse button before entering the rotation mode with tab. This will be fixed in the next iteration.
![](Img/RotatedObjects.PNG "Objects rotated randomly.")
