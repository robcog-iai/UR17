# Milestone 1 - deadline 31.12.17

* Create an editor plugin for creating screw joints between two static meshes
* The screw joint constraints the movement of the lid to one direction (rotation
  along the z axis, i.e. yaw).
* Destroy the constraint of the screw joint after moving the lid mesh a certain
  distance

## Documentation

### Implementation

All agreed upon features where implemented. The implementation has a standard
opening angle of 45° after wich the constraint breaks. To get initialized the
`ScrewJointActor` first has to check if both components are available (i.e. no
null-pointers). In the first iteration the physics constraint was constructed,
i.e. a blueprint was used to construct the scenario and it was broken during
runtime. To allow for more flexibility the constraint was then changed so that
the two static meshes could be chosen during the design time in the editor.
This lead to some challanges, and to changing the initial datatype for the
components from `StaticMesh` to `Actor` and finally to `StaticMeshActor`.
The latter still behaves like an Actor while providing access to variables we
need from the `StaticMesh`, i.e. the possibility to enable `Physics` and
`movability` via code. Thus the user does not need to worry about anything
other than picking the Meshes he dragged into the scene.

The physics constraint uses a +/- angle freedom, when limiting the angular swing
along the x-y-plane (usually known as yaw, in Unreal Engine denoted as
`AngularSwing1`). This means that adding a value of 45° actually allows for 90°
movement (45° to the left, and to the right). This also means that initially
one could move the lid both clockwise and counterclockwise, which is not
desirable. To counter this effect we use a angular offset on the swing value
in height of the angle. For a scenario where an angle of 45° is preset, this
means that an Angle of -45° is moved 45° counterclockwise. This narrows the
angle down and gives us the advantage to start at one of the extreme values.
The handedness is preset to be righthanded, i.e. a counterclockwise turn will
open the lid. By monitoring the swing value one could then break the constraint,
once the other extreme is reached.

However, while testing I encounterd the rare situation, where the angular
swing does not start at 0°, but at a higher value, even though the rotation is
at the bottom extreme and cannot travel clockwise. This lead to the constraint
breaking earlier than expected, sometimes even right at the beginning. As this
effect only occured pretty seldom and not with any standard Unreal Materials but
only with the scanned Grocery items I was not able to pinpoint the problem in
time.

Since this was unsatisfying I refrained from using the angular swing and
instead now use the rotation vector of the StaticMeshActor. Saving the initial
state at the beginning one can take the absolute value of the difference from
intial value to current rotation.

### Testing

We tested the implementation using standard materials from Unreal as well as
scanned meshes from the RobCog kitchen, using the simulation capabilties of the
unreal editor. Our tests were overall satisfying. Optimizations we found are:
1. After an initial push (even by running against the lid in simulation mode)
   the lid would simply slide open.
2. When moving the lid fully to the closing side it would "collide" with the
   angular limit and tend to "bounce" back.

### Tutorial

See [Tutorial](Tutorial.md).
