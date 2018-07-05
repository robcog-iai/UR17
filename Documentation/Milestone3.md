# Milestone 3 - deadline 30.06.18

* Properly separate and convert cut procedural meshes to whole (and separate) actors [static meshes]
* Add more usability options
  * Let user choose the material that is shown in the inner-parts of a sliced object (with a specific material-slot name)
* Finishing touches on realistic cutting
  * Limit the up- and down-movement once cutting has started (via use of realistic constraints conditions)
* Test-scenarios
  * Create a test-scenario for showcase purposes
  * Find better values for the constraints conditions
  * Help P4 with setting up the plugin for the 2D game

## Features
These are all currently NEW implemented features that are useable in other projects (ontop of the ones mentioned in [Milestone1](Documentation/Milestone1.md) and [Milestone2](Documentation/Milestone2.md)):

* Static meshes are now only converted to procedural meshes at the immediate step before the cutting-function is called
  * This results in no unnecessary conversions, as those only need to happen, because procedural meshes are the only ones that can be used for the cutting-function that we use
* Converted static meshes (as procedural meshes) are now converted back to static meshes after the cutting-process has finished
  * This allows the game-environment to be unchanged as much as possible, as procedural meshes only have limited functionality (e.g. the tested grasping in VR only worked with static meshes, so without the conversion back, the user would not have been able to pick up cut objects anymore). [Reference](#standard-cutting-procedure)
  * The process for that is rather complicated (and processor-intensive), so code was copied from a function in the unreal engine (as it was unaccessable otherwise), with slight modifications where applicable. The copied code is clearly marked.
* The user can now supply a new material to be used as the inner cut material for the newly cut out area of objects. This means that once an object is cut, the two new exposed areas will have the material the user sets in the material slot with the slot name "InsideCutMaterial". [Reference](#standard-cutting-procedure)
  * If the user does not create a new slot, the default WorldGridMaterial will be used.
* Cutting is now using constraints to simulate a knife being "stuck" in an object, meaning it will move the object with the knife, instead of just being able to freely move around. [Reference](#constraints-in-action)
* Cuttable objects can now have a resistance value attached to them. This value determines how much force the user has to apply to be able to cut the objects (aka move the knife through the object)
  * This results in limiting the movement of a cutting object to only one plane while cutting - as that's how a knife would cut objects in reality


### Constraints In-Action:
![Constraints In-Action](DocumentationPictures/Constraints.gif)  
Here you can see the constraints (moving the cut object with the blade) and tip-functionality (pushing the object with the tip).

### Standard Cutting Procedure:
![Standard Cutting Procedure](DocumentationPictures/CuttingProcedure.gif)  
Here you can see the standard cutting procedure as would normally be done.

# Documentation

* This time a lot of work has been done to refactor the code, but the general idea of the existing classes stayed the same. For more detailed explanation refer to the explanation seen in [Milestone2](Documentation/Milestone2.md).

### SlicingLogicModule:

* The visual debugging got refactored into the class "USlicingDebugVisualComponent".
  * This component has to be added to a cuttable object to be able to display any visual debugging.
    * It will search for other slicing components that might be attached to the same actor. If it doesn't find that specific component, it will not display the visuals relating to that component.
    * Everything else was mostly untouched and just copied over
  * No other component will be able to display any visual debugging information.
* The class "FSlicingHelper" was created to house all the static functions used in the slicing logic.
  * This is the place where the conversions from static mesh to procedural mesh and back happen.
    * Now the static materials are saved throughout the conversion process, as to correctly recreate the static mesh at the end.
  * There are also some auxilliary function to aid other classes, like with the function "GetSlicingComponent(UStaticMeshComponent* SlicingObject)".

### SlicingBladeComponent
  * Uses **Constraints** now to simulate the realistic behaviors of cutting.
    * Added the variable `resistancePercentage` inside the `OnBeginOverlap` function which is used to simulate a resistance of an object with the help of **Linear Damping**.
      * `resistancePercentage` takes it's value through the reading of the `CutComponent` **Tags**. More specifically a *percentage* integer tag below one called **Resistance** (See *Picture 1*).
    * It's usage simulates pushing of the object with the knife, if you shove it with the flat side while cutting.
      * Statically puts **Linear Damping** for the `CutComponent` to 100 to "pseudo" disable physics of it.
        * Needs to be done with physics, since the Constraints need it.
  * Takes and sets `InsideCutMaterial` by identifying the SlotName by the name (See *Picture 2*) inside the SlicingLogicModule, which then sets it in here.


# Tutorial

* The Tutorial for using the plugin hasn't changed from the previous tutorial as supplied in [Milestone2](Documentation/Milestone2.md).
* The only exception being the new inner cut material that can be displayed when cutting an object.
  * For that the user can now create a new material-slot in a specific static mesh and give it the material slot name "InsideCutMaterial" (See *Picture 2*.)
    * To create another slot, press the `+` Key, choose the desired Material in the Dropdown menu of the spawned new slot and name the slot `InsideCutMaterial`

**Picture 2:**  
![Inner Cut Material Setup](TutorialPictures/InnerMaterial.png)


# Todos

* Refactoring the code copied from UE for the conversion of proc meshes to static meshes
* Fine-tune values for the cutting-resistance (dampening)
  * Have a more user-friendly way of editing the resistance values and/or allow the user more options
* Fix a lot of small bugs

## Issues

* Constraints cause a handful of oddities, when used
  * Debug visuals have problems showing up correctly in multiple ways
  * Cutting can sometimes not abort correctly and the game will think we are still in the cutting process (reseted, once another object is cut again)
  * Objects might lose physics and only gain them once the objects collides with any other actor in the level (even if it's an object unrelated to cutting)
* The inner cut material is sometimes not used correctly and will instead display the standard WorldGridMaterial
