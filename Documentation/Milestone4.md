# Author 
André Osse

# Documentation Milestone 4 - P3 - Opening objects of daily use

# Milestone 4 - 30.09
- Getting a visual feedback in VR for the debug mode.
	- Show if the Constraint is destroyed.
	- (Optional) Show the number of degrees it is turned in the debug mode with an ID or Names of the constraint.
	- Improve the log by adding IDs or Names to the different constraints.
- Making it possible to close the object again, after its completly open and the constraint is destroyed.
- Getting a better version of the friction damping.
	- (Optional) Should also be parameterized

# Getting a visual feedback in VR for the debug mode.

The debugging is simply a DebugSphere so its visible in debugmode, where the sphere begins, that needs to be overlapped by the lid. It only works with the lid that is actually used for this bottle.
![Overlapping Sphere](/Documentation/Img/Milestone3/overlappingsphere.JPG)
This sphere just appears when the Constraint is destroyed.
There has no log IDs been added.

# Making it possible to close the object again, after its completly open and the constraint is destroyed.

It has been an Overlapping Sphere added. It's a SphereComponent that just reacts when something is overlapping it. The overlapping method looks if the object is actually the lid, that was used and restores the physical constraint component. Because there is no way to edit the Swing1 degrees by yourself, it simply uses 180 degree counter to get back on. Also so turn Flag needs to be set. The new position of the lid is the point it should be with this degrees. Calculated by a unit vector in the direction where the lid should be that got to be on the length which is as far as it should be away. After that the new vector just gets moved with a simple addition over the point the constraint is actually. After setting it up again, its like the begining. Because of that the TurningFlag and TurnCounter is set manually. 

# Getting a better version of the friction damping.

There is no working part of this yet. Problem was mainly because lid keeps moving if the SetAngularDriveParams are different. The position gets it back on the position it was in begining and the velocity gets it turned of without moving it if it is bigger than the position strength.
# Fixed bugs

There has been no bugs fixed.

# Tutorial

To see the sphere, the debug mode and the "bIsTurnedLidOnAgain" flag must be set.

# Known Bugs

Sometimes the sphere sits a bit under the Bottle.