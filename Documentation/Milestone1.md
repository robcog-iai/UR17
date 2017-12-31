# Milestone 1 - deadline 31.12.17

* Simulate a realistic walking simulation.

# Documentation

* The walking speed is handled in the CMovment.h and CMovement.cpp, in the Plugins/Source/Private/Character/Components folder. The class UCMovment, which contains the walking simulation, handles the overall movment of the character. 

* To calculate a realistic speed up the "Sprungantwort" function form PT1-Glied is used. It creates a rising curvature until it nrealy reaches the maximum value. The function curve looks like the following picture. ![](Img/Step_PT1.png "Sprungantwort")  
[Link to the picture and PT1-Glied documentation.](https://de.wikipedia.org/wiki/PT1-Glied)

* To realise the speed up the function VelocitySPeed is called, which takes a TimeStep parameter, which determins how fast the speed up incresses. The TimeStep variable is held in the SpeedUpTime varibale which rises by a SpeedUpvalue which can be incressed or decressed to speed up or speed down the speed increasse.  

* The increasse happens in the move functions, where the SpeedUpTime is checked and increassed until it reaches the max value. The new CurrentSpeed is calculated Through the VelocitySpeedUp function, the SpeedUpTime and MinMovmentSpeed. The MinMovmentSpeed is needed, because without the character speed gets buged and does rise in the wrong way or gets stuck.  

* In the TickComponent function the CurrentSpeed and the SpeedUpTime are reset, after the characters stops moving. Thic is checked with the GetVelocity function of the character.  

* The following code shows the "Sprungfunktion" in code version:

```
float UCMovement::VelocitySpeedUp(float TimeStep)
{
	// The variables are fixed and change the curve of the function, if changed
	int TVariable = 1;


	// Variables to hold the values from the function
	float Tmp = -(TimeStep / TVariable);
	float BracePart = (1 - FMath::Exp(Tmp));
	float NewSpeed = MaxMovementSpeed * BracePart;


	return NewSpeed;
}
```

# Tutorial
* The functionality of the walking can easily be testet by starting the RobCoG.uproject, starting the game and moving around with W, A, S and D. The character should start in a slow movment speed and getting faster, until the character reaches the maximum speed.  
![](Img/SpeedValuesDebug.PNG "Debug for speed values in the editor.")

* If the game is startet from the RobCoG.sln, in the debug mode, the movment speed will be printed in the output log of the Unreal Engine Editor. It will also mention at which point the speed is being reseted. The speed also depends on the direct which is pressed, so it will be negativ for walking backwards or to the left. The following picture shows the output of the speed, in this case it switches between 0.26 and -0.26, because the character walks forward and left and the speed reset, which is called when the character does not move.   
![](Img/DebugLog.PNG "Output message for the speed and speed reset.")

