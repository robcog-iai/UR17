# Milestone 2 - deadline 31.03.17

* Parameterization
  * ~~Angle of thread~~ (standardized to 60° for all screws)
  * Handedness (standard value = righthanded)
    * Righthanded = when turning lid counterclockwise, the lid moves up along
      the z-axis
    * Lefthanded = when turning lid clockwise, the lid moves up along the z-axis
  * Height of threaded area
  * Number of turns to open
  * Turning angle to open
  * **Lead** = Distance a screw cap travels, after turning it 360°
    * **Lead** is standard key characteristics in engineering
    * Unknown for any household container
    * Therefore: approximate **lead** as ratio of height/NoOfTurns
* ~~Make lid travel linearly up/down on threaded axis while rotating it~~
* (*optional*) Make it force based
  * Lid is loose when not fully closed
  * Apply (higher) friction/dampening, when lid is fully screwed onto container,
    so that opening it requires an initial force

## Documentation

### Stipulated Milestone points

#### Parameterization

Implementing the parameters worked without any flaw, and using the built-in
functions of the Unreal Engine we ensured that parameters can only be inserted
in the desired form without need for extra validity checks. As discussed in the
plenum, we did not implement the angle of the thread, as this would not bring any
added value to the simulation and overcomplicate things.

To make sure that the physics constraint makes use of the value changes via the
editor, the initialization process had to be changed. To make sure that any
changes done in the editor take immediate effect and to enable the user to test
and debug parts of the functionality even in editor mode, the physical
constraint does not get initialized in the `AScrewJointActor::BeginPlay()`
method, but instead in the `AScrewJointActor::PostEditChangeProperty()` method,
that monitors value changes and acts on them.

Whenever a value gets changed, the method checks if all values needed for a
physical constraint (both StaticMesh components as well as the the `bIsOpen`
flag) are set, the constraint gets initialized.

During the initialization process the following values are set for the physical
constraint:

- AngularSwing1 is limited to the user defined angle
- AngularSwing2 and Twist are locked
- All Linear movements are locked (per default)
- The `bSoftConstraint` is set to false, making the limit a "hard" limit. The
  default "soft" limit makes the constraint "bouncy".

The sliding could not be disabled via constraint. This is rather done by setting
the damping of the lid mesh. After some testing with different values, "100"
turned out to be pretty decent (at a damping value of 100 the gravity is
countered, i.e. no undesired movement takes place). However, the value is also
parameterized so the user may change it to his needs via the Editor interface.

#### Problem: Traveling on the rotational axis

Traveling along the rotational axis was not possible to be solved that easy:
As the objects are constrained along all axes it is not possible to change the
position of the lid mesh, even in code. Disabling the limitation of the z-axis
would allow to simply move the lid up to detach it, which is not desirable.

A workaround would be to break the constraint, and reinitialize it in every
step, making it an expensive operation to be performed every tick. Alternatively
the limit on the z-axis could also be set to a limit, being 0 at the beginning
and gradually rise, while at the same time the linear offset would need to rise
as well (analogue to the angular offset this would set the bottom border);
also the direction of the turning movement would have to be captured and if
turning downwards again, the whole process needs to take place in reverse.

While this could actually work, one can easily see how this would overcomplicate
the code, making it more error-prone and harder to maintain. The gain on the
other hand would be minimal. In the kitchen scenario, most threaded areas are at
most 2~3cm high (e.g. PET soda bottles), and usually much smaller (0.5cm for a
glass of jam). So the effect would not even be that evident most of the time.

In favour for easier and more maintainable code, this point was therefore not
implemented, i.e. striken from the original milestone list.

The parameters are still available though; making it a possible extension for
future versions of the code.

#### Optional: Force-Based approach

I liked the idea of having an initial force when opening objects, so two new
parameters where introduced:

1. `InitialForce`
2. `ForceThreshold`

The first value affects the dampening. If the lid is tightly screwed onto the
container, the dampening of the lid is increased to this value. If this is not
desired, it is safe to set it to the same value as the `StandardDamping` that is
used for the overall damping of the lid (see above).

The second value determines for how long this higher damping will have an
effect. It is set as a value form 0-1 and can be interpreted as initial fraction
of the overall angular movement. This should probably be really small, so a
value of `0.1` is preset, while the initial force is preset at 10 times more
than the standard dampening. As we were not able to test it in a force-based
environment, the preset is just an educated guess. However, this will probably
vary from scenario to scenario, so having it user-definable is of more value
than determining a personal favorite initial dampening.

#### Bonus: Debugging

Besides the agreed upon milestone points, a debugging output interface was
created, that can be user activated and that will print out important values
every tick, as well as changes made during setup time.

#### Bonus: Closing the lid

Another "bonus" feature is the ability to close the lid. As this was not part of
the assignment, this is kept pretty rudimentary, but it allows for scenarios
where a lid has to be opened and closed multiple times.

For closing the lid after it has been opened, one has to place the lid back onto
the container and then turn it for **TODO** ° in either direction. If desired
for future releases, this can be extended to a more realistic closing by
initializing a reverse constraint, that would break both, at starting and at
ending point, but depending on wich side it would either open up again (starting
point) or reinitialize the opening constraint (at ending point).

For the realistic opening task, being able to quickly close an object might
however be more feasable than having a very realistic closing of objects as
well.

To determine wether lid and container are at the desired position, both, lid and
container have a small sphere that would overlap when both are at the right
position. Only then will a slight turn of the lid trigger a new constraint.


## Tutorial

A small text tutorial with pictures can be found [here](Tutorial.md).
A video showing how to set up the constraint and using it with a realistic mesh
from the kitchen repertoire can be found at [YouTube](https://youtu.be/cMsCL2Jlw5k).

However, as realistic objects are not as demonstrating when rotating (due to
their round shape), there is also a second video using simple squares that can
be found [here](https://youtu.be/K5l_XbJYrbQ).
