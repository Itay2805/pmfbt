#include <opencv2/opencv.hpp>
#include <pose/Pose3D.hpp>

#include <atomic>
#include <thread>

#include "CameraServer.hpp"

/**
 * Allows the stop function to tell the camera server to stop
 */
static std::atomic<bool> mRunning = false;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * The thread that handles capturing
 */
static std::thread mCaptureThread;

/**
 * Handle capture
 */
static void CaptureThread() {
    cv::VideoCapture vidCapture1 {0};
    cv::VideoCapture vidCapture2 {1};

    mRunning = true;
    while (mRunning) {
        // capture the two images
        cv::Mat capture1, capture2;
        vidCapture1 >> capture1;
        vidCapture2 >> capture2;

        // TODO: pass these through the system

        // TODO: transform into the Pose3D setup
        Pose3D pose3d = Pose3D(...);

        // update all the positions of the virtual trackers now that we have a new position
        GetDriverInstance().LeftLegTracker.UpdatePoint(pose3d.joints[JT_LEFT_ANKLE]);
        GetDriverInstance().RightLegTracker.UpdatePoint(pose3d.joints[JT_RIGHT_ANKLE]);
        GetDriverInstance().HipTracker.UpdatePoint(middle(pose3d.joints[JT_LEFT_HIP], pose3d.joints[JT_RIGHT_HIP]));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: the contorl server, allowing an external app to control which cameras and stuff we are going to use

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

[[noreturn]] void StartCameraServer() {
    // create the capture server thread
    mCaptureThread = std::thread(CaptureThread);
}

void StopCameraServer() {
    // tell everything to stop
    mRunning = true;

    // wait for all threads to stop
    mCaptureThread.join();
}
