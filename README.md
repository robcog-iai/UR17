# UR17
Unreal Robots (2017-18) master project repository

# Info

* each branch belongs to a given subproject, e.g. `ur17-p1` belongs to subproject P1
* every branch has the up to date `dev-env` branch from [RobCoG](https://github.com/robcog-iai/RobCoG/tree/dev-env) as default
* every subproject will create a pull request for each milestone on its corresponding branch
* every pull request should have a `MilestoneX.md` file, in `/Documentation` for every milestone, describing the milestone, what has been done(documentation), how to test the code(working/tested tutorials).
* the pull request (if containing source code) should be contained in a plugin, e.g. `\Plugins\MyPlugin`
* the pull request (if containing new uassets, e.g. meshes, etc.) can be done on the project directly, it should however include the 3D mesh files as well in `.fbx` and `.dae` format in the folder `\Documentation\Meshes`

----

# Every project will maintain its `README.md` as follows:

----

# UR17 - PX

### Members

* Bender Rodriguez
* Turanga Leela

### Project description

* 2-3 lines describing the project

### Milestones

* [Milestone1](Documentation/Milestone1.md)
* [Milestone2](Documentation/Milestone1.md)

### Documentation

* Documentation of the project of when it is **finalized**, written in the `Documentation/Documentation.md` file
* This should basically be a combination of all the milestone descriptions
* The above bulletpoints should obviously removed in the pull request, and replaced with something like: The documentation of the project can be found [here](Documentation/Documentation.md).

### Tutorial

* Documentation of the project of when it is **finalized**, written in the `Documentation/Tutorial.md` file
* Again combination of all the milestone tutorials
* The above bulletpoints should obviously removed in the pull request, and replaced with something like: Tutorials for testing the project can be found [here](Documentation/Tutorial.md).
