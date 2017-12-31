# Milestone 1 - deadline 31.12.17

* Cutting core code
  * Basic cutting (not all specifics are handled)
* GUI-Prototype
  * Being able to edit the knife
* Debug visuals
  * slicing plane can be visualized
  * cutting area can be visualized
  * cutting trajectory can be visualized

# Documentation

* The project is realized with a plugin. This plugin is then split up into two different modules: one for extending the Unreal Editor and one that deals with the actual logic of the slicing/cutting, called SlicingEditorModule and SlicingLogicModule respectively.

### SlicingEditorModule:

* In the Unreal Editor there are now two new toolbar buttons that appear if the Slicing plugin is loaded in the project.
  * One is seen in the Static Mesh Editor that gets opened if one selects any already existing static mesh and edits it. This button is only used as a tutorial for the user, so that he knows what he should be doing for the cutting to be working. The user would need to create new sockets in this editor, which would then be converted in the next toolbar button.
    * ![Static Mesh Editor](Documentation/StaticMeshEditorButton.png)
  * The other toolbar button is seen in the Level Editor that one sees when the Unreal Editor starts up. It has a drop-down menu in which it's possible to change the debug-options to be enabled/disabled. Another important button in the drop-down menu is the conversion of the sockets that were previously created. This is needed to actually change object in the world, compared to just the templates (aka the static meshes).
    * ![Level Editor](Documentation/LevelEditorButton.png)
  * A good start to find out how to extend the Unreal Editor is found here: https://www.youtube.com/watch?v=zg_VstBxDi8
* There are 4 classes that were created:
  * FSlicingEditorModule
    * This is the core class for the module. It not only represents the module itself but is the main point from where all the other classes are loaded/accessed from.
    * In here the UI elements get initialized by using the FSlicingEditorCommands class, which then get mapped to functions of the FSlicingEditorActionCallbacks class.
    * Those buttons then get added to the Unreal Editor by using the extension points that are exposed by the Unreal Editor, through the usage of FExtender and MenuBuilder.
  * FSlicingEditorStyle
    * This class is only needed to define the aesthetic of the UI elements. This is a standard class that is generated with every plugin by the Unreal Editor. In our case it's mainly used to display the icons of the two UI elements, but can be extended if one needs more customization on the look of the buttons (or other future elements).
  * FSlicingEditorCommands
    * This class is only used to register the commands of the UI elements that need to be added. This means the UI buttons that are used in the plugin are defined and initialized with the help of this class.
    * Here one defines what kind of UI element a given element should be (in our case either a togglebutton for the debug options, or a button for the rest of the elements), the texts that are  associated with the element (e.g. hover-over text) and an optional hotkey-association.
  * FSlicingEditorActionCallbacks
    * This is a class purely with static functions that are associated with the selection of a specific UI element. Therefore they are only called once the user presses the button associated with the function (the mapping of the function happens in the module-class).

### SlicingLogicModule:

* ...
  
# Tutorial

### Basic Cutting:

* ...

### UI elements:

* ...

### Editing of an existing static mesh:

* ...

### Testing the debug options:

* ...