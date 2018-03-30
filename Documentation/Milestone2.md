# Milestone 2 - deadline 30.3.18

* hands following actors
* implement solution for moving and turning
* tutorial how to rig your own hand (from start to end) (cutting it in blender, rigging it in unreal)
* shoulders (arms) attached to camera
* finish VR arms project, so we can start with OptiTrack finger tracking in our 
* (optional) nicely rigged humanlike arm model 

# Using this version of our project:

The current version of our project has one characters in it's world, that the player can posses. 
The default one is the one we created using the generic male body and c++ code. It can collide with other objects.
The movement of the elbow is very limited.
The character arms can be controlled using the HTC Vive and Oculus Rift. 

# Documentation
* New Plugin: VRArmsTracking
* New classes: ArmAnimInstance, ArmAnimPawn


* ArmAnimInstance
Translates the coordinates from the motion controllers to fitting coordinates for unreal.
Moves the attached hands according to the coordinates of the motion controllers.
Has the ray trace collision algorithm in it so the character can not collide with other objects and will move on its surface.

* ArmAnimPawn
Pawn which will be controlled with the motion controllers and the headset. With the location and rotation coming from the controllers this
pawn will support realistic arm movement. The position of the pawn is also set in here and it will follow the headset location. This way the arms are attached to the camera.
Here is also our solution for turning and moving. 

* Main functionalities:
Full arm tracking using only VR devices (Oculus or HTC Vive). 

# How to start

- start the game

- get into position

- (!) press the thumpsticks or tracking pad on your MotionControllers to start arm movement and rotation (!)

# Tutorial:

- Create an animated blueprint using SKEL_Skelet as skeleton and the ArmAnimInstance as animation instance.
- Edit the anim graph of the new created blueprint like this:
![](./Img/AnimGraph.png)

- Create a new blueprint from the ArmAnimPawn classes.
- Set the anim class of the blueprint to the animated blueprint you just created.
![](./Img/AnimClass.png)

- Place the blueprint in the world and hit play.

- If you have hands you want to attach to the mesh go to the details pane of the of the blueprint you placed in the world and add them as left/ right hand. These hands
have to be actors in order to add them to the mesh. The actor needs to have a skeletal mesh component. Have a look at the tutorial.md on how to create fitting hands for the mesh since there are some things that has to be added.

- If you have any other mesh than our mesh then you have to change the uproperty SkeletalPath to your mesh.

- You can adjust multiple upropertys, most importantly "Mesh Rotation", which is used to align the forward axis of mesh and camera  

- make sure to go to the action mappings of your project and bind "resetPosition" to a button. In our project we already bound it to the MotionController thumpsticks and spacebar. 

- Pressing the assigned button will reset your current position and will start the movement and turning of the arms. 