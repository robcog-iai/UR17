## Create new grasps with the editor

This tutorial shows and explains how to create new animations in the skeleton editor. 

What has to be done before:
Have a file called TypeMap.txt in the folder YourProjectFolder\Plugins\URealisticGrasping\Config. It should map all the used bones of the mesh to a finger (e.g index_01 is a finger of type Index). It should look like this:
![](Img/TypeMap.jpg "TypeMap")

First you need to open  a skeleton to open its preview scene. In the toolbar you have a button called Create Grasp. Its a drop down button with multiple entries. When you want to create a new grasping style you should 
press the button "Save current hand position as episode" to save the initial positions of the hand. A Grasp should always start with the initial position. Now you can more hand positions as episodes using the button again if you want to.
If you want to start again and discard all your created steps then press the button "Discard all saved episodes".
When you have recorded all the episodes you want you can create a new grasping style with the entry "Create new grasping style out of saved episodes"
from the Create grasp button. There you have to name your newly created grasping style. This creates a .ini file in the GraspingAnimations folder of your project. You are only able to create a grasping style out of your animations if you have at least
2 episodes recorded.
Click on this link to see a video about it https://youtu.be/aH353CnSfFA

If you want to edit an episode afterwards you can do like its explained in the following tutorial.

* [Edit an animation](./Documentation/EditAnimation.md)



