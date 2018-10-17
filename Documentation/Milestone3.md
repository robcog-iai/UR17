# Milestone 3 - deadline 31.06.18

* Create connection between UE4 and ROS by sending a LaserScan.msg
  * Dummy Data for start
* Create laser scanner actor in UE that will simulate a laser scanner
  * Using the depth information directly from the UE rendering process
* Realistically simulate a laser scan
  * Have the rendering data be send with a delay (simulating rotation of the
    laser scanner)
  * parameterize laser scan configuration
  * have lower more realistic resolution, e.g. only scan every point that is 4°
    appart from the last one
  * Have variable overall angle that is covered by the laser

# Documentation

## Implementation

### LaserScan.h

In order to send messages using the
[sensor_msgs/LaserScan](https://docs.ros.org/api/sensor_msgs/html/msg/LaserScan.html)
data format from ROS,
this format had first to be introduced into the existing URosBridge plugin,
as it did not yet exist.

A class was created for that purpose in a forked repo,
that can be found
[here](https://github.com/pygospa/UROSBridge/blob/master/Source/UROSBridge/Public/sensor_msgs/LaserScan.h),
following the exact example of the other messages already implemented, i.e.
* entire class definition is written into the header file
* getters and setters for all member variables are created
* there is a [FromJson](https://github.com/pygospa/UROSBridge/blob/master/Source/UROSBridge/Public/sensor_msgs/LaserScan.h#L150),
method that will take a pointer to a FJsonObject and read out all fields into
the instance of this class.
* there is a
  [GetFromJson](https://github.com/pygospa/UROSBridge/blob/master/Source/UROSBridge/Public/sensor_msgs/LaserScan.h#L181)
method that will take a pointer to a FJsonObject, create a new LaserScan
instance and call it's FromJson-Method (see above) with the FJsonObject pointer,
then return the newly created LaserScan class
* there is a
  [ToString](https://github.com/pygospa/UROSBridge/blob/master/Source/UROSBridge/Public/sensor_msgs/LaserScan.h#L188)
method that will print out the instance in JSON format
* there is a
  [ToJsonObject](https://github.com/pygospa/UROSBridge/blob/master/Source/UROSBridge/Public/sensor_msgs/LaserScan.h#L217)
method that will create a new JSON object that can be used, e.g. to send it to
ROS.
* there is a
  [ToYamlString](https://github.com/pygospa/UROSBridge/blob/master/Source/UROSBridge/Public/sensor_msgs/LaserScan.h#L247)
method that creates a YAML data object instead of a JSON one.

Of course, for my purposes only the Constructor and ToJsonObject would have been
needed, still I wanted to create the entire class probably so it might be used
in other usecase scenarios as well and behaves just like any other class in that
repository would. For the same reason of consistency I did not write any
comments, as UE4 according to the style guide would normally ask you to do over
each method and variable in JavaDoc format.

The class still resides in my repository, and my main project therefore links to
this repository, however a pull request will be sent out to the official
repository as well, and as soon as it is included in the official source I will
change the link of the dependency in the git submodule accordingly.

### LidarActor.cpp

The other class for this project can be found in the LidarActor.cpp, respectively the
LidarActor.h file.

The LidarActor implements a LiDAR (Light Detection And Ranging), which is the
general class in which also Laser Scanners (LADARs) fall.
I've decided to rename it from LaserScanner, as the concept implementet in the
class aply to any kind of LiDAR - that it's a laser scanner is just an
interpretation that we do in our robot scenario.

The Constructor sets up all the standard values, like the typical ROS port and
address. Special paramters that describe a laser scanner and that can vary in
specification from vendor to vendor are again handled in a
`PostEditChangeProperty`-function that I took from my last project and reused
it. I looked up different vendors such as the [Hokuyo
UTML-30LX](https://www.hokuyo-usa.com/products/scanning-laser-rangefinders/utm-30lx)
to get a feel about what specs are available in waht form.
This way the user can specify the parameters in different ways and the function
will calculate it into all other related values:

- ScanAngleDeg and ScanAngleRad will be updated by each other
- AngularResInDeg, AngularResInRad, AngularResStepsInside and
  AngularResStepsOf360 will update each other

Of course only those specs of interest for the LiDAR are considered. Besides the
above mentioned these are the minimum and maximum distance, and the time per
scan.

The Constructor also sets up the `USceneCaptureComponent2D` which I use to get
the rendering data. I use the `SCS_SceneDepth` Engine for capturing, as it
captures depth information in centimeters (according to e.g.
[this](https://answers.unrealengine.com/questions/708727/what-does-devicedepth-option-under-scenecapturecom.html)),
so no conversions needed.

`BeginPlay` and `EndPlay` methods are used for setting up the connection to ROS
instance, sending messages was successfully tested via String message as well as
dummy laser scan messages.

Inside the `Tick` method the following four parts take place:

1. As laser scanners have a delay, the tick first checks if that delay is
   reached already. If not, it returns
2. If it's time for another scan, the depth image is taken from the
   `USceneCaptureComponent2D`. It will be into a RenderTexture for
   preprocessing; applying additional materials (such as decribed
   [here](https://answers.unrealengine.com/questions/708727/what-does-devicedepth-option-under-scenecapturecom.html))
   can make the depth data visible. Testing this was successful, so the data
   seems to be reliable. For our processing however, we want the raw data, so
   these are saved into a buffer as FFloat16Pixels.
3. Our Postprocessing will do the following:
    - Read out every Pixel, and get the FFloat16Color value.
    - For each FFloat16Pixel get the value of the R Channel, as the depth
      information is saved into the R channel
    - If that information lies inside the bondaries of the laser scanner range,
      add them to an Array
4. Last step: Construct a LaserScan message with all the data gathered and send
   it to ROS

## Caveats and room for improvement

The program was tested with different settings and parameters and a few problems
and improvements occured:

- While with empty data array everything on the ROS side works fine, this stops
  as soon as you have data in your array. ROS will say that there is an error in
  the header, wich obviously there is not.
  The data get still send over to ROS, but the ROS server displays it as error
  and does not publish it to the topic.
  See example on [YouTube](https://youtu.be/XQvC49XmiFA).

- The resolution of the image can only be set up to 2048 pixel, without Unreal
  warning. Considering that typical laser scanners have a range of 200° and
  more, and that resolution is 0.25° you easily come towards the 2000 data
  points, making every pixel or every second pixel in the 2048 pixel range.


## Tutorial

A small text tutorial on the setup can be found [here](Tutorial.md).
A video showing how to set up the constraint and using it
can be found at [YouTube](https://youtu.be/gxkiNfIG7_Q).
