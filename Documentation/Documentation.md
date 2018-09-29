# UR17 - Cutting & Slicing - P2

## Table of Contents

| Nr | Tutorials |
|:----:|--------|
|1| [Features](#features) |
|2| [Documentation](#documentation) |
|3| [Todos](#todos)|
|4| [Issues](#issues)|

## 1. Features
These are all implemented Features that are available in this plugin.

* **Simple way of choosing 'Cuttable' StaticMeshComponents.** In the editor by simply selecting an object and pushing a integrated button in the editor.
* **User-friendly solution to create a 'Knife'.** From a existing StaticMesh with the usage of Unreal Engine's own StaticMesh Editor and it's ability to place Sockets.
* **A Realisitc Cutting Experience.** With the usage of Physical Constraints, in total enabling the realistic simulation of...
	* ... **Resistance.** Causing the Knife's movement to stagger and be stopped.
	* ... **Connection.** For between the Knife and Object, adding another layer of realism.
	* ... **Cutting Conditions.** As not allowing to cut under certain situations and circumstances. Those roughly being exiting prematurely and cutting incorrectly.
* **Toggle-able Visual Debugging Options.** Giving you the ability to see visual cues and debugging help for following things...
	* **Predicted Cut Plane.** In the form of a drawn DebugPlane.
	* **Representation of Logic Boxes.** In the form of DebugBoxes in the places where they are.
	* **Trajectory of gone Path.** In the form of little points showing you it's gone path.
	* **Start- and Endposition.** In the form of two DebugBoxes based on the entrance and the predicted exit.
* **Integrated Editor Menu.** Which gives a user-friendly option to activate with the Visual Debugs and Create a Cuttable or Knife object.
	* Converting can also be done by selecting ***multiple*** objects at once.
* **VR-Compatibility.** This plugin, while also usable in a 3D Game, was build with the intention for VR-Games.
* **Compatibility with existing Projects.** The converting to ProceduralMeshComponents is not permanent and will promptly be converted to a StaticMeshComponent after the cut.

## 2. Documentation

**__This is a summary of Previous Milestones ([1](Milestone1), [2](Milestone2), [3](Milestone3))__**

The goal of this sub-project was a user-friendly plugin for a realistic cutting and slicing experience. To do so we've split it into two parts, namely **SlicingEditor** and **SlicingLogic**, splitting things needed in the Editor and Logic.

### SlicingEditor

This holds all code which works closely to the Editor itself and is responsible for things like, adding our menu into the Editor and it working as a bridge for our logic.

#### SlicingEditorCommands
Text

#### SlicingEditorActionCallbacks
Text

#### SlicingEditorModule
Text

#### SlicingEditorStyle
Text

### SlicingLogic

This module holds everything which our plugin needs to perform as intended, including things like the classes for our Logic Boxes and so on. For the purpose of this Documentation we've put all Component parts into a one section, since they all inherit from the same Class.

#### SlicingComponents & Children
Text

* **SlicingBladeComponent.**
	* Details about this and it's Methods.
* **SlicingTipComponent.**
	* Details about this and it's Methods.
* **SlicingDebugVisualComponent.**
	* Details about this and it's Methods.

#### SlicingHelper
Text

#### SlicingLogicModule
Text

## 3. Todos

- Done

## 4. Issues
* **IMPORTANT:** Cutting only works on static meshes that only have "Convex Elements" as collision primitives. "Spheres", "Boxes" or "Capsules" do NOT work and result in the sliced object to just fall through the world instead of being cut. This is a limitiation of the ```UKismetProceduralMeshLibrary::SliceProceduralMesh(...)``` function and cannot be (currently) changed.
* Sometimes if it lags (the hand starts to vibrate too much):
	* The object is cut more than once.
	* The game might crash if too many objects were cut in rapid succession.
* The cutting-exit-point will only show on static meshes that were in the stage before the game started. Dynamically generated static meshes from our cutting process will not show that exit-point properly (instead showing it at the 0,0,0 point, as the hit-result for the line-trace does not yield a proper result anymore)
* Exiting the object without cutting it only works from the same point as the entrance-point. Any other point (even if only a small distance next to it) will result in the cutting process finishing with the current slicing-plane. Without free-form slicing, this issue cannot be resolved.
