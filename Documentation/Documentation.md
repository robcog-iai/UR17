# UR17 - P2

## Features
These are all currently implemented Features you're able to use in projects.

- The ability to cut through chosen StaticMeshComponents
	- Chosing certain StaticMeshComponents in the Editor giving a tag
- Have chosen Knifes able to cut certain StaticMeshComponents and generate ProceduralMeshComponents
	- By using Tags to identify them
	- Placing Sockets as the Positions SlicingLogicBoxes
		- Which updates all already placed and coming StaticMeshComponents with the chosen StaticMesh with these Sockets.
- Toggleable Debug Functions able to:
	- Print out text as needed (Basic in it, but only throwing "Test" right now)
	- Draw a DebugPlane showing a Plane on which the Component will be cut.
	- Draw Debugboxes, showing the position of the LogicBoxes on the Knife.
	- Draw the Trajectory of the Knife, up to 1 second ago currently.
- Editor Buttons on which the Debug Functions are Toggleable and Sockets of StaticMeshComponent can be filled with the Boxes.
	- Toggleable Debug Checkboxes
	- Button to spawn LogicBoxes on chosen StaticMeshComponents.
	
## Todos

- Realistic Interactions and Physics of the Knife
- Button to fill all Sockets of StaticMeshComponents, who fall under the Knife Category, with LogicBoxes
- Split DebugPlan and DebugPlane Button to two.

## Issues

- The Boxes spawned are not saved on the .umap
- Sockets are not the same size as BoxComponents, causing some placement issues for now. (Limiting scale of the socket from 0 to 1)