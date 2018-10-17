## Create new animations 

This tutorial shows and explains how to edit an existing animation in the skeleton editor. 


First you need to open  a skeleton to open its preview scene. In the toolbar you have a button called Edit Grasp. Its a drop down button with multiple entries. For now press the button "Load grasping style". This will open a window where you can
type in the name of the animation and a specific episode you want to edit. When you did this and pressed the button at the buttom of the window the episode will be displayed in the preview scene (if there exists an animation with the given name and
the given episode). You can not edit the episode 0 since this should always contain the initial positions of the mesh. Now you can change the rotations of the bones and adjust the episode. When you are finished you can press the 
button "Overwrite loaded episode" to apply your changes. After you pressed the button at the buttom of the window, the currently loaded rotations of the episode will be replaced with the currently displayed rotations.
This video https://youtu.be/GybwNNdLkpk shows how to edit a grasp.

After you load in an episode of an animation you can also use the buttons "Show next episode" and "Show previous episode". These will either show the next or the previous episode depending on the episode currently displayed. When you press one of this
buttons they will also change the episode you are currently editing. For example if you load in episode 2 of an animation, press "Show next episode", change the bone rotations and press "Overwrite loaded episode" you will change the rotations of the
3. episode. You will always overwrite the last episode you loaded in the preview scene.



