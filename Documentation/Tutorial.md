# Tutorial

1. After starting any simple map that has at least two static meshes to be
   connected to each other, you can simply find the Static Srew Joint by
   searching for it (after having moved the directory into the plugin folder of
   your project).

* ![StartingScene](pics/tutorial/01.PNG)
* ![Searching for ASrewJoint](pics/tutorial/02.PNG)
* ![Dragging the ScrewJoint into the Scene](pics/tutorial/03.PNG)

2. In the begining it does not matter where you move your screw joint, it will
   position itself automatically. Simply drag it into the scene and then go to
   the "ScrewJoint->Components" setting, select the picking tool or type in your
   components name for lid and container.

* ![Initial Screw Joint](pics/tutorial/04.PNG)
* ![Pic a StaticMesh](pics/tutorial/05.PNG)

3. After having chosen both components, the constraint will position itself at
   the right position and you can change any other settings you want to have
   (e.g. debugging output, handedness or the angle)

* ![New Constraint Position](pics/tutorial/06.PNG)
* ![Use your VR Eqippment, or Simulate it](pics/tutorial/07.PNG)

4. Having the debug output activated, you'll get a clean small area in the left
   top corner. Most interesting: The "Open"-Flag. Once you turn your lid
   component in the right direction for the needed amount, this will change into
   "yes", and you can freely move the lid around.

* ![Debug message closed](pics/tutorial/08.PNG)
* ![Debug Message open](pics/tutorial/09.PNG)
* ![Lifting the Lid](pics/tutorial/10.PNG)

5. If you want to reattach the lid, just put it back at the correct positon and
   give it a gentle twist.

* ![Putting it back on](pics/tutorial/11.PNG)
* ![Reattaching it with a gentle twist](pics/tutorial/12.PNG)

