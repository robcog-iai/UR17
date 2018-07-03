# Milestone 3 - deadline 30.06.2018

* get Marcels thesis project to work
* save the position of a skeletal-mesh (from editor)
* read and write bone position to .ini
* fine-tuned force is applied while grasping
* test with various kitchen objects 
* switch grasp on button press (optional)
* inbetween steps for grasps possible (optional)
* physics material for hands (optional)


# Documentation

* New Plugin: URealisticGrasping
* New modules: URealisticGrasping, URealisticGraspingEditor


* URealisticGrasping module classes:

* URealisticGrasping:
The module itself

* AGraspingCharacter:
VR character, that the player posseses

* AGraspingHand:
Hands that get used by the AGraspingCharacter

* UFinger:
A finger

* UGraspingAnimationController:
Responsible for loading and managing all the different grasp for the hands

* UGraspingController:
A controller responsible for moving the bones of the hands

* UReadWrite:
Reads and writes files with grasping information


* URealisticGraspingEditor module classes:

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


# Tutorial:

In this project we create our grasping types as animations and then record how the bones move during the animations.
Afterwards we use that recorded data to apply force to our hands, so that they try to move to the same positions, that the animation was in. 
The following three tutorials explain each of these procedures. Step 1 and 2 are only needed if you want to record new grasping types. 
If you just want to use existing grasping types only step 3 is needed.

* [creating new animations](./Documentation/CreateNewAnimations.md)
* [recording new grasping types from aniamtions](./Documentation/ReadingHandleTypes.md)
* [using our character](./Documentation/tutorialChar_Bind.md)


# Issues

* Why we did not use the editor to get the rotations of the bones?
To get the bone rotations of a given mesh out of the editor we would need to access the Persona-Editor. This editor is the one that opens when you click on a skeleton and where you can manipulate the rotations of the bones (without beeing able to save your 
actions). From the Persona-Editor we would need to get access to the PersonaModule. This has the ability to create a PersonaToolkit, which has access to the PreviewMeshComponent or the EditableSkeleton (one of them could potentially be the mesh we want
to access). Our problem was that we were not able to load the module. A way to load a module would be like this:

FPersonaModule& PersonaModule =
		FModuleManager::Get().LoadModuleChecked<FPersonaModule>("PersonaModule"); 
		
However this always threw an exception when trying to do something on the loaded module, although LoadModuleChecked() already checks if there exists a module with the given name. So we were basically stuck at this point.
However we needed a way to get the rotations of the bones so we could try out the other parts of our milestone like the actual grasping. 
We will try to get the rotations of the bones out of the editor during the next milestone but for this milestone it was just important to have the rotations.
We also put up some questions in the unreal forum but we did not get any answers.
https://answers.unrealengine.com/questions/785734/access-persona-skeleton-editor-in-c.html
https://answers.unrealengine.com/questions/779903/how-to-access-the-skeletoneditor.html