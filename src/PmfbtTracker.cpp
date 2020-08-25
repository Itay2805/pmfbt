#include <openvr_capi.h>
#include "PmfbtTracker.hpp"

PmfbtTracker::PmfbtTracker()
    : objectId(vr::k_unTrackedDeviceIndexInvalid)
    , lastPose()
    , mutex()
{
    // setup an invalid pose
    this->lastPose.poseIsValid = false;
    this->lastPose.result = vr::TrackingResult_Uninitialized;
}

void PmfbtTracker::UpdatePoint(const vector3& new_point) {
    vr::DriverPose_t newPose{};

    // setup generic information
    newPose.deviceIsConnected = true;
    newPose.poseIsValid = true;
    newPose.result = vr::TrackingResult_Running_OK;

    // TODO: everything to setup the new position

    // actually set the pose
    {
        std::lock_guard<std::mutex> guard{this->mutex};
        this->lastPose = newPose;
    }

    // notify the server we got a new pose
    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(this->objectId, this->lastPose, sizeof(this->lastPose));
}

void PmfbtTracker::UpdateOutOfRange() {
    vr::DriverPose_t newPose{};

    // setup generic information
    newPose.deviceIsConnected = true;
    newPose.poseIsValid = true;
    newPose.result = vr::TrackingResult_Running_OutOfRange;

    // actually set the pose
    {
        std::lock_guard<std::mutex> guard{this->mutex};
        this->lastPose = newPose;
    }

    // notify the server we got out-of-range
    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(this->objectId, this->lastPose, sizeof(this->lastPose));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The OpenVR interface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Will setup the device's properties
 */
vr::EVRInitError PmfbtTracker::Activate(uint32_t unObjectId) {
    this->objectId = unObjectId;
    auto props = vr::VRProperties()->TrackedDeviceToPropertyContainer(this->objectId);

    // opt-out of hand selection
    vr::VRProperties()->SetInt32Property(props, vr::Prop_ControllerRoleHint_Int32, vr::TrackedControllerRole_OptOut);

    return vr::VRInitError_None;
}

void PmfbtTracker::Deactivate() {
    this->objectId = vr::k_unTrackedDeviceIndexInvalid;
}

void* PmfbtTracker::GetComponent(const char *pchComponentNameAndVersion) {
    return nullptr;
}

void PmfbtTracker::DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) {

}

/**
 * Just return the last valid pose, makes sure to
 * take the mutex so we will not race with the tracking
 * thread.
 */
vr::DriverPose_t PmfbtTracker::GetPose() {
    std::lock_guard<std::mutex> guard{this->mutex};
    return this->lastPose;
}

/**
 * We don't ave standby mode on a per-device
 * level
 */
void PmfbtTracker::EnterStandby() {

}
