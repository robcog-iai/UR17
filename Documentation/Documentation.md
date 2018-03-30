# UR17 - P2

## Features
These are all currently implemented Features you're able to use in projects.

- The ability to cut through chosen StaticMeshComponents
	- Chosing certain StaticMeshComponents in the Editor giving a tag
- Have chosen Knifes able to cut certain StaticMeshComponents and generate ProceduralMeshComponents
	- By using Tags to identify them
	- Placing Sockets as the Positions SlicingLogicBoxes
		- Which updates all already placed and coming StaticMeshComponents with the chosen StaticMesh with these Sockets.
- Make be restricted as to:
	- Not cutting when pulled out prematurely by..
		- .. exiting at the start point
		- .. exiting in the middle by pulling out.
		- .. Not cutting correctly deep enough.
- Toggleable Debug Functions able to:
	- Print out text as needed (Basic in it, but only throwing "Test" right now)
	- Draw a DebugPlane showing a Plane on which the Component will be cut.
	- Draw Debugboxes, showing the position of the LogicBoxes on the Knife.
	- Draw the Trajectory of the Knife, up to 1 second ago currently.
	- Draw Start-/Exit points when cutting
- Editor Buttons on which the Debug Functions are Toggleable and Sockets of StaticMeshComponent can be filled with the Boxes.
	- Toggleable Debug Checkboxes
	- Button to spawn LogicBoxes on chosen StaticMeshComponents.
	
## Todos

- Realistic Interactions and Physics of the Knife
- Button to fill all Sockets of StaticMeshComponents, who fall under the Knife Category, with LogicBoxes

## Issues

- The Debug Box showing the presumed exitpoint of the knife is only shown correctly at the first cut of the object.