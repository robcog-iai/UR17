# UR17 - P2

## Creating a new Knife

### Step 1: Creating Sockets in the StaticMeshEditor

For this first choose a StaticMesh on which you want the SlicingLogic to be.
In this example we use the **SM_Knife_Table**. Double click on it in the
**Content Browser** to open the StaticMeshEditor for it.

In the top bottom should be a tab called **Socket Manager**. Press the button
and add three sockets named **BladeBox**, **Handle** and **CuttingExitpointBox**.
It should look like this: 

![TutorialPic1](TutorialPictures/1.PNG)

From there on you can begin to place the tokens as needed in the StaticMeshEditor, e.g.

![TutorialPic2](TutorialPictures/2.PNG)

***Note:*** **Currently you can only give the sockets relative scales value from 0 to 1.**

With that done you can save the StaticMesh in the StaticMeshEditor and leave it.
All the StaticMeshComponents with that StaticMesh now all have these Sockets.

### Step 2: Tag a StaticMeshComponent to a Knife

Since we don't want all **SM_Knife_Table** to be Cuttable we give the the Components, who
are supposed to be, a Tag names ***Knife***.

For this click on your desired Knife in the Viewport and click on the StaticMeshComponent
in the Details Tab on the right.

![TutorialPic3](TutorialPictures/3.PNG)

Then scroll down towards the section called Tags. Click on ![TutorialPic4](TutorialPictures/4.PNG)
and name the new Tag ***Knife***.

![TutorialPic5](TutorialPictures/5.PNG)

### Step 3: Spawn the boxes in the Editor

Now that the knife has it's Tag, we can begin to spawn the LogicBoxes.
First we select the StaticMeshComponent in the Editor followed by the
StaticMeshComponent in the Detail View.

![TutorialPic3](TutorialPictures/3.PNG)

Then on the top, in the same row as **Play** and **Launch**, there's the button
called "**Slicing Options**", clicking on it reveals more options such as Debug
Checkboxes. The important Button is "**Replace Sockets on Selected Object**".

![TutorialPic6](TutorialPictures/6.PNG)

After clicking it, it spawns the BoxComponents. If they don't appear at first
simply click on something else and click on the knife again. It just needs
to refresh.

***The Knife is done. Result:***

![TutorialPictures8](TutorialPictures/8.PNG)

## Make Object Cuttable

### Step 1: Select StaticMeshComponent

To make an object cuttable select one in the Viewport.

![TutorialPic7](TutorialPictures/7.PNG)

### Step 2: Enable Overlap Boolean & Tag

Just as in ***Step 2: Tag a StaticMeshComponent to a Knife*** we now add a
tag called "**Cuttable**" into the Tag section. Refer to the mentioned Step
above on how to do that.

The other thing to do is activating that our Selected Mesh is able to generate
Ovelaps on it's own. For that simply go to the Collision section of your Selected
StaticMeshComponent click on ***Generate Overlap Event***.

![TutorialPic9](TutorialPictures/9.PNG)

***Now that object is Cuttable.***