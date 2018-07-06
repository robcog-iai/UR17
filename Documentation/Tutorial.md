# Tutorial P9

## Setup

1. Clone the repository containing the LiDAR Plugin into your plugin repository
2. Inside the repository, initialize the submodules to get UROSBridge
   - If you are working on the console, issue:
   `git submodule update --init --recursive`
3. You can now start your Unreal Engine project.
    I. navigate to the LiDAR C++ Clases and drag a LidarActor to your scene
    II. You will find a number of settings regarding the ROS server or the laser
    scanner in the Details pane of the LidarActor. Set them accoringly.
4. Start the game to create a connection and have the actor send messages to
   your ROS instance
5. In your ROS console you can subscribe to the topic you set up in the
   LidarActor to listen to the packages.

A video demonstration can be found [here](https://youtu.be/gxkiNfIG7_Q)
