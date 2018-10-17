# UR17 - P9

### Members

* Kannan Thambiah

### Project description

Purpose of this project is to add an Actor to the VR in Unreal
that simulates a laser based LiDAR. The depth data is taken from the
[rendering process](http://api.unrealengine.com/INT/API/Runtime/Engine/Components/USceneCaptureComponent2D/index.html),
of Unreal and then send over [URosBridge](https://github.com/robcog-iai/UROSBridge)
and [rosbridge_suite](https://wiki.ros.org/rosbridge_suite) to a running
[ROS](http://www.ros.org/) instance.

This way, simulated data can be sent to real robotic programs running on ROS,
and reaction to the simulated data can then be analized or feed back to the
simulation.

### Milestones

* [Milestone3](Documentation/Milestone3.md)

### Documentation

* Documentation of the project of when it is **finalized**, written in the `Documentation/Documentation.md` file
* This should basically be a combination of all the milestone descriptions
* The above bulletpoints should obviously removed in the pull request, and replaced with something like: The documentation of the project can be found [here](Documentation/Documentation.md).

### Tutorial

* Documentation of the project of when it is **finalized**, written in the `Documentation/Tutorial.md` file
* Again combination of all the milestone tutorials
* The above bulletpoints should obviously removed in the pull request, and replaced with something like: Tutorials for testing the project can be found [here](Documentation/Tutorial.md).
