#include <hyperpose/hyperpose.hpp>
#include <opencv2/opencv.hpp>

#include <atomic>
#include <thread>

#include <pose/Pose3D.hpp>

#include "CameraServer.hpp"

/**
 * Allows the stop function to tell the camera server to stop
 */
static std::atomic<bool> mRunning = false;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Setup the model that we are going to use
 */
static auto mHyperPoseEngine = hyperpose::dnn::tensorrt(
            hyperpose::dnn::onnx{ "ppn-resnet50-V2-HW=384x384.onnx" },
            { 384, 384 },
            1
        );

/**
 * Setup the parser we are going to use
 */
static auto mHyperPoseParser = hyperpose::parser::pose_proposal(mHyperPoseEngine.input_size());

static void LoadModel() {

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * The thread that handles capturing
 */
static std::thread mCaptureThread;

/**
 * Handle capture
 */
static void CaptureThread() {
    cv::VideoCapture vidCapture1 {0, cv::CAP_V4L2};

    mRunning = true;
    while (mRunning) {
        // capture the two images
        cv::Mat capture1;
        vidCapture1 >> capture1;

        // Do the HyperPose pose estimation
        auto featureMaps = mHyperPoseEngine.inference({ capture1 });
        auto poses = mHyperPoseParser.process(featureMaps.front());

        // find the pose with the best score and use it
        hyperpose::human_t* bestPose = nullptr;
        for (auto& pose : poses) {
            if (bestPose != nullptr && bestPose->score < pose.score) {
                bestPose = &pose;
            } else if (bestPose == nullptr) {
                bestPose = &pose;
            }
        }

        // check if we even found a good pose
        if (bestPose != nullptr) {
            // we got a pose, reconstruct setup the points for 3d reconstruction
            std::array<vector2, hyperpose::COCO_N_PARTS> positions {
                vector2(bestPose->parts[0].x, bestPose->parts[0].y),
                vector2(bestPose->parts[1].x, bestPose->parts[1].y),
                vector2(bestPose->parts[2].x, bestPose->parts[2].y),
                vector2(bestPose->parts[3].x, bestPose->parts[3].y),
                vector2(bestPose->parts[4].x, bestPose->parts[4].y),
                vector2(bestPose->parts[5].x, bestPose->parts[5].y),
                vector2(bestPose->parts[6].x, bestPose->parts[6].y),
                vector2(bestPose->parts[7].x, bestPose->parts[7].y),
                vector2(bestPose->parts[8].x, bestPose->parts[8].y),
                vector2(bestPose->parts[9].x, bestPose->parts[9].y),
                vector2(bestPose->parts[10].x, bestPose->parts[10].y),
                vector2(bestPose->parts[11].x, bestPose->parts[11].y),
                vector2(bestPose->parts[12].x, bestPose->parts[12].y),
                vector2(bestPose->parts[13].x, bestPose->parts[13].y),
                vector2(bestPose->parts[14].x, bestPose->parts[14].y),
                vector2(bestPose->parts[15].x, bestPose->parts[15].y),
                vector2(bestPose->parts[16].x, bestPose->parts[16].y),
                vector2(bestPose->parts[17].x, bestPose->parts[17].y),
            };

            // do the 3d reconstruction
            // TODO: have the rel order so we would have better things
            Pose3D pose3d = Pose3D(positions, std::array<int, 11>());

            // update all the positions of the virtual trackers now that we have a new position
            GetDriverInstance().LeftLegTracker.UpdatePoint(pose3d.joints[JT_LEFT_ANKLE]);
            GetDriverInstance().RightLegTracker.UpdatePoint(pose3d.joints[JT_RIGHT_ANKLE]);
            GetDriverInstance().HipTracker.UpdatePoint(middle(pose3d.joints[JT_LEFT_HIP], pose3d.joints[JT_RIGHT_HIP]));
        } else {
            GetDriverInstance().LeftLegTracker.UpdateOutOfRange();
            GetDriverInstance().RightLegTracker.UpdateOutOfRange();
            GetDriverInstance().HipTracker.UpdateOutOfRange();
        }
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
