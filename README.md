* see [master](https://github.com/robcog-iai/UR17/tree/master) branch for filling out the README
# Members
- André Osse

# Project description
- A physics constraint that helps bottles to have realistic behavior at opening them.
- Easy to use

# Milestones
- Milestone 1:
	- Creating an editor plugin for a screw joint between 2 mesh actors.
	- Screw joint constraints the movement in one direction.
	- Destroy the constraint after moving the lid by a fixed degree.
	
- Milestone 2:
	- Parameterize
		- Angle (standard value = 60°)
		- Turning way (standard value = right)
		- Height of threaded area
		- Number of turns to open
		- Approximate lead as ratio height/no of turns
	- Make lid travel linearly up/down on the threaded axis while rotating it

- Milestone 3:
	- Getting lid up/down moving to work.
		- Should work in every rotation.
		- Calculated by the turned degree.
	- Getting a friction/dumping force.
	- Creating a debug mode.
	- Putting documentation in markdown on GitHub.


# Documentation
You can find the documentation for milestone 1 and 2 in the /Documentation/Documentation.pdf or in markdown [here](/Documentation/Milestone1and2.md).
You can find the documentation for milestone 3 [here](/Documentation/Milestone3.md).
# Tutorial
You can find a  written tutorial in the Milestone 1 and 2 documentation, but you can also find a video [here](https://youtu.be/Fwbgb_8h_Us) or in the /Documentation/TutorialVideo/ folder.
If you want to see this working in VR you can see this [here](https://youtu.be/KHbCYiSnoqw) or in the /Documentation/WorkInVRVideo/ folder.
