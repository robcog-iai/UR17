# Milestone 4 - deadline 30.09.2018

* improve and test grasping
* menu for grasps
* implement steps
* create a walkthrough video for the whole process
* create grasps in editor (optional)
* force indicator (optional)


# Documentation

* Plugin: URealisticGrasping
* Modules: URealisticGrasping, URealisticGraspingEditor, URealisticGraspingForceIndicator, URealisticGraspingUI


### URealisticGrasping module classes:

* URealisticGrasping:
The module itself

* URealisticGraspingComponent:
An ActorComponent that can be added to any VR pawn. Replaces the AGraspingCharacter. 

* AGraspingHand:
Hands that get used by the AGraspingCharacter

* UAnimationDataStructure:
Data structure to save data about different hand bones.

* UGraspingAnimationController:
Responsible for loading and managing all the different grasp for the hands

* UGraspingController:
A controller responsible for moving the bones of a hand. Was used to control both hands in milestone 3. Now one seperate controller is created for each hand, to get rid of redundant code. 

* UReadWrite:
Reads and writes files with grasping information

* ForceBaseIndicatorInterface:
Interface for the URealisticGraspingForceIndicator module


### URealisticGraspingEditor module classes:

* URealisticGraspingEditor:
This class appends a new button to the skeleton editor module and initializes its drop down. It also registers the actions for the drop down menu entries.

* UGraspingEditorStyle:
Creates the style for the new button.

* UGraspingEditorCallback:
This class contains all the actions the menu entries of the drop down can have. 

* UGraspingEditorCommands:
Sets the label of the drop down menu entries and their type. In this case they are toogle buttons.

* AGraspingStyleManager:
This class is able to read all the bone rotations of a passed mesh during the game for a set time in set timesteps. This rotations and some additional informations with be written in a .ini file when the game is ended.


### URealisticGraspingForceIndicator module classes:

* UForceBaseIndicatorArrow:
Example class to visualize the force as an arrow.

* UForceBaseIndicatorBox:
Example class to visualize the force as a box.


### URealisticGraspingUI module classes:

* URealisticGraspingUI:
The module itself

* GraspingButton:
Class to create the button for the menu.

* GraspingMenu:
Class to create the menu.

# Tutorial

* [how to use our plugin](./Documentation/tutorialChar_Bind.md)
* [creating new grasps with the editor](./Documentation/CreateNewAnimationsWithEditor.md)
* [edit grasps with the editor](./Documentation/EditAnimation.md)
* [customize the menu](./Documentation/RealisticGraspingUserInterface.md)
* [creating a physics material](./Documentation/Create_physics_material.md)
* [what is Tightening Grasp](./Documentation/TighteningGrasp.md)
* [creating new animations](./Documentation/CreateNewAnimations.md)
* [recording new grasping types from animations](./Documentation/ReadingHandleTypes.md)

# Issues

* Before the end of the milestone an error has occurred in the intensive test. The bug only appears in Visual Studio and does not affect the functionality and stability of the plugin.
The error appears when the game ends without calling up the widget. The following error message appears:
*“UE4Editor-UMG.pdb contains the debug information needed to find the source for UE4Editor-UMG.dll module”* (in vs were not jumped in any class for debuging).
The bug only appears during the first run.
When the game is restarted (without restarting the unreal version), the error will no longer occur. In addition, the error does not occur after the widget has been rendered during the first run.  
Unfortunately, there was not enough time to fix the error. A help request was posted in the unreal engine forum under this [link](https://answers.unrealengine.com/questions/832873/ue4editor-umgpdb-contains-the-debug-information-ne.html).