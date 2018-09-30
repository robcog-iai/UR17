# UR17 - Cutting & Slicing - P2

## Table of Contents

| Nr | Tutorials |
|:----:|--------|
|1| [Features](#features) |
|2| [Documentation](#documentation) |
|3| [Todos](#todos)|
|4| [Issues](#issues)|

## Features
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

![Standard Cutting Procedure](DocumentationPictures/CuttingProcedure.gif)

## Documentation

**__This is a summary of Previous Milestones ([1](Milestone1.md), [2](Milestone2.md), [3](Milestone3.md))__**

The goal of this sub-project was a user-friendly plugin for a realistic cutting and slicing experience. To do so we've split it into two parts, namely **SlicingEditor** and **SlicingLogic**, splitting things needed in the Editor and Logic.

### SlicingEditor
This holds all code which works closely to the Editor itself and is responsible for things like, adding our menu into the Editor and it working as a bridge for our logic.
#### SlicingEditorCommands
The class which simply holds our commands, which are later used in the menus of the Editor.

#### SlicingEditorActionCallbacks
This class holds all the actions or more specifically methods corresponding to our commands. Giving our buttons the interaction they're supposed to do. The important methods include...
- **MakeCuttableObjects()**. Which is responsible for turning a selected object in the editor, and after the button press, into an object which the 'Knife' us able to cut.
- **AddBoxComponent()**. Which is responsible for adding the appropriate BoxComponents and is used by other function within the class.

#### SlicingEditorModule
It's there to handle the module, starting and shutting it down. It's also where we create and add the buttons to the menu of the Editor and give them their respective Actions.

#### SlicingEditorStyle
This class simply hold stylistic bits needed for our menu, giving an icon for example.

### SlicingLogic

This module holds everything which our plugin needs to perform as intended, including things like the classes for our Logic Boxes and so on. For the purpose of this Documentation we've put all Component parts into a one section, since they all inherit from the same Class.

#### SlicingComponents

* **SlicingComponent.** This class holds the necessary basic methods, like attaching to a given object, called upon by all the Components it's inherited by.
* **SlicingBladeComponent.** One of the more logic heavy classes, holding the code controlling our entire process of cutting, variables important for all the methods and the set up and handling of thins like the Physical Constraints.
	* **SliceComponent().** Is hereby the center point of this class and holds the code working with the ProceduralMeshComponents and our cutting in general.
* **SlicingTipComponent.** Is a class and component with only one goal, checking if what's overlapping it's boundaries is the object being cut by it's related SlicingBladeComponent. If not, it lets it's SlicingBladeComponent know.
* **SlicingDebugVisualComponent.** The class simply holding all our code to show our visual debug.

#### SlicingHelper

One of the more important classes, as this one holds the code and tools necessary to convert our ProceduralMeshComponent and StaticMeshComponent and helps by building a new StaticMeshs from scratch per code.

#### SlicingLogicModule
It's there to handle the module, starting and shutting it down.

## Todos

- Done

## Issues
* **IMPORTANT.** Cutting only works on static meshes that only have "Convex Elements" as collision primitives. "Spheres", "Boxes" or "Capsules" do NOT work and result in the sliced object to just fall through the world instead of being cut. This is a limitation of the ```UKismetProceduralMeshLibrary::SliceProceduralMesh(...)``` function and cannot be (currently) changed.
* Sometimes if it lags, seen when the hand starts to vibrate unnaturally...
	* ... the object is cut more than once.
	* ... the game might crash if too many objects were cut in rapid succession.
* The cutting-exit-point will only show on static meshes that were in the stage before the game started. Dynamically generated static meshes from our cutting process will not show that exit-point properly (instead showing it at the 0,0,0 point, as the hit-result for the line-trace does not yield a proper result anymore)
* Exiting the object without cutting it only works from the same point as the entrance-point. Any other point (even if only a small distance next to it) will result in the cutting process finishing with the current slicing-plane. Without free-form slicing, this issue cannot be resolved.
