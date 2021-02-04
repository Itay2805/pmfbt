# Poor Man's Full Body Tracking

The aim of this project is to provide full-body tracking capability to vr games using only a phone/webcam.

## Modules
These are projects we are using directly from our driver.

### [HyperPose](https://github.com/tensorlayer/hyperpose)
A 2D pose estimation library based on machine learning, we use this to get the basic 2d positions which we are going to 
reconstruct into a 3d point afterwards.

### [OpenCV](https://github.com/opencv/opencv)
A framework for image processing, currently we use it for its video capture abilities.

### [OpenVR](https://github.com/ValveSoftware/openvr)
A framework for exposing VR related hardware and software to games, we use it for exposing the virtual trackers.

## Credits
These are projects we don't use directly but found useful while creating the driver.

### [3D Human Pose Reconstruction](https://github.com/cflamant/3d-pose-reconstruction)
This project is the main backbone for the algorithm that reconstructs the 3d position from the outputs 
of the network. 

### [KinectToVR](https://github.com/KinectToVR/KinectToVR)
This project was used to figure out how to properly create a driver that is exposed to the OpenVR server.
