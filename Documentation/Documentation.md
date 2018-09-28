# UR17 - Cutting & Slicing - P2

## Features
These are all implemented Features that are available in this plugin.

- **Simple way of choosing 'Cuttable' StaticMeshComponents.** In the editor by simply selecting an object and pushing a integrated button in the editor.
- **User-friendly solution to create a 'Knife'.** From a existing StaticMesh with the usage of Unreal Engine's own StaticMesh Editor and it's ability to place Sockets.
- **A Realisitc Cutting Experience.** With the usage of Physical Constraints, in total enabling the realistic simulation of...
	- ... **Resistance.** Causing the Knife's movement to stagger and be stopped.
	- ... **Connection.** For between the Knife and Object, adding another layer of realism.
	- ... **Cutting Conditions.** As not allowing to cut under certain situations and circumstances. Which include...
	 - ... exiting the object at the very start.
	 - ... exit the object while cutting in the middle.
	 - ... having not cut the object correctly, as not deep on one point of it's path.
- **Toggle-able Visual Debugging Options.** Giving you the ability to see visual cues and debugging help for following things...
	- **Predicted Cut Plane.** In the form of a drawn DebugPlane.
	- **Representation of Logic Boxes.** In the form of DebugBoxes in the places where they are.
	- **Trajectory of gone Path.** In the form of little points showing you it's gone path.
	- **Start- and Endposition.** In the form of two DebugBoxes based on the entrance and the predicted exit.
- **Integrated Editor Menu.** Which gives a user-friendly option to activate with the Visual Debugs and Create a Cuttable or Knife object.
	- Converting can also be done by selecting ***multiple*** objects at once.
- **VR-Compatibility.** This plugin, while also usable in a 3D Game, was build with the intention for VR-Games.
- **Compatibility with existing Projects.** The converting to ProceduralMeshComponents is not permanent and will promptly be converted to a StaticMeshComponent after the cut.

## Documentation

The goal of this sub-project was a user-friendly plugin for a realistic cutting and slicing experience. To do so we've split our 

## Todos

- Done

## Issues
- TODO
