# Author 
André Osse

# Documentation Milestone 3 - P3 - Opening objects of daily use

# Milestone 3 Description
- Getting lid up/down moving to work.
	- Should work in every rotation.
	- Calculated by the turned degree.
- Getting a friction/dumping force.
- Creating a debug mode.
- Putting documentation in markdown on GitHub.

# Getting lid/up down moving to work

By the idea that usually a screw lid is controlled by a degree to turn around and the way it moves upwards is a simple function of the turned degrees and the way it should move up, the calculation of the moving way upwards is simply the distance it should move upwards divided by the degrees it needs to be destroyed. This means the calculation is like this:
DistanceToMoveUpPerDegree = DistanceToMoveUp / RotationUntilDestroyingTheJoint
In other words, first of all before the start the distance it should move upwards per degree gets calculated. In every tick it gets checked how far it really moved, by a familiar calculation like in milestone 1 and 2, and this real turned degrees get multiplied by DistanceToMoveUpPerDegree. So the exact value it should be moved upwards gets calculated.
In order to get the lid to move upwards, it was necessary using some kind of drive in the Unreal Engine. It had to be set with a position strength, which means the initial position, and a velocity strength, which is the actual position it should move to. Both of these strengths had to be set on 50 000 to make sure it feels like it is limited and can't be moved by your own. In the VR with unlimited strength it still can be taken off but it snaps back right in the second the player releases it. So it is like its limited on this spot. To make this more sure, the linear limit of the Z-Axis gets limited on the spot it should be moved up. By the velocity and strength used, 50 000 like a limited motion, it won't move to the initial point and it won't move more far away.
[Source that was used to figure this out.](https://docs.unrealengine.com/en-us/Engine/Physics/FrictionRestitutionAndDamping)
By setting the position target on the value it should be moved on and the setting of the limit, it is impossible to move it for the player, it is just possible to be moved by the code of the RealisticOpeningConstraintActor, so this plugin.

# Getting a friction/dumping force

The friction/damping force was the idea to help the lid not moving endless when it gets an initial turning force from a human. This means unlike in the first milestone it shouldn't move endless when it gets hit. In order to do this it has been used also a drive, since the friction/damping functions the PhysicsConstraint of the Unreal Engine had been removed and replaced by a drive. The use of the drive is similar as the one used in the section before. It has got a drive just working on the Swing1 Axis, which is the Axis it can be turned. The biggest change is, that this drive isn't a linear once it is an angular once that only works on the Swing axis. Since the Swing2- and Twist-Axis aren't used at all, they got limited by 0. To have almost the same effect on the Swing1-Axis like the linear Z-Axis Drive causes, it's also set to 50 000 on position and velocity strengths. This causes, since the limitation is free this time, that the player can rotate the lid himself and it gets stopped at the point he stopped rotating it.

# Creating a debug mode

The debug-mode is pretty simple. In the Tick function, that gets called on every tick, there is a function that is a part, which is only activated if the debug-mode is activated too. The debug-mode simply got a new category on the Unreal Editor. Shown in the figure below. If the save log flag is activated, it just prints the log on the output log. You can find a few values there, like the real current degree value and the actual swing 1 value. It just gets an output if the change is bigger than 1 degree from the last output, so it won't be flooding the output log. Though this should be just activated on one constraint at the time, to make sure the reader doesn't get confused by different values.

![Figure 1](/Documentation/Img/Milestone3/FIGURE1.PNG)

In the debug-mode there are 2 values, that also can be used as sliders. The first value called MoveToDegree means, that the lid gets rotated until this degree. The TurningSpeed simply sets a speed the rotation gets done. It can be a value between 0.1 and 1.0. 1.0 is the quickest rotation that can be set. The limit is to make it better visible how the rotation of the lid in a realistic way works. Of course, it also moves up if a value for moving upwards is set.
If the debug-mode isn't activated this doesn't work and effects nothing. By default, the debug-mode is deactivated.

# Putting documentation in markdown on GitHub

This and also the old documentation can now be find as markdown documentations on Github.

# Fixed bugs

On the first version it wasn't possible to set the bottle and lid on physics on, since it was wobbling a little and got swing 1 degree values, without moving it at all, like -30.0008. This caused that it thought it gets over the -30 degree point the second time and increased the 180 degree counter. This obviously caused that every destroy angle below 180 degrees got destroyed fast, without even moving it. This and some other effects that happened by moving it in VR are fixed by a new calculation of the 180 degree spots. In this section this new calculation will be explained by the opening direction to the right, because the other direction works similar, just with simple changes that can be seen in the code.
As first the complete calculation got to be a little simpler. Now it just calculates in one way if the swing 1 is currently between -30 degrees and 180 degrees. It just needs to add the offset of 30 degrees and if the value is above or equals 180 degrees, 180 degrees should be subtracted. Reason for this is that the 180 degree turns still get counted at the same places. You can find this as green line on Figure 2.

![Figure 2](/Documentation/Img/Milestone3/Figure2.png)

The other calculation is from -180 degrees to -30 degrees. This just needs to add the negative value on the 210 degrees. So the 210 degrees gets subtracted by the value of the swing 1 degree. The 30 degrees that are missing from the positive side, causes the value to be 210 degrees and not 180 degrees.
You can find this sector as red line on Figure 2.

This makes the calculation a lot less complex. Though it needs to be checked if a human really could have turned the lid also by filtering out artifacts that sometimes happens with VR. For this there is a flag that says if a value is legit or not. A turning of 20 degrees is a legit value. It also had to check if the value only passed the 180 degree point and switched the sign by passing this spot. Of course this is over 20 degree, if it goes from -175 degrees to 175 degrees, but because its just the sign that changed by passing the 180 degree spot, it is not really turned more than 20 degrees, it just has turned 10 degrees, and still is a legit value. This also gets checked.
You can see this section red marked on Figure 3.

![Figure 3](/Documentation/Img/Milestone3/Figure3.png)

All in all this fixed all known bugs from milestone 1 and 2. Also this fixed the bugs that has been found in VR.

# Tutorial

A video tutorial can be found in /Documentation/TutorialVideo. Or just on [Youtube](https://youtu.be/Fwbgb_8h_Us). You can also find a video showing the constraint working in VR example project of the Unreal Engine [here on Youtube](https://youtu.be/KHbCYiSnoqw). You also can find this video in the /Documentation/WorkInVR/ Folder on GitHub.

# Known Bugs

If you use more than one bottle and let them write in the output log for debugging, it might get confusing output, since there is not shown yet which constraint caused which output.
