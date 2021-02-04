#include "PmfbtDriver.hpp"

#include "PmfbtTracker.hpp"
#include "CameraServer.hpp"

vr::EVRInitError PmfbtDriver::Init(vr::IVRDriverContext* driver_context) {
    VR_INIT_SERVER_DRIVER_CONTEXT(driver_context);

    // add all devices
    vr::VRServerDriverHost()->TrackedDeviceAdded(
            "PMFBT Left Leg",
            vr::TrackedDeviceClass_GenericTracker,
            &LeftLegTracker);

    vr::VRServerDriverHost()->TrackedDeviceAdded(
            "PMFBT Right Leg",
            vr::TrackedDeviceClass_GenericTracker,
            &RightLegTracker);

    vr::VRServerDriverHost()->TrackedDeviceAdded(
            "PMFBT Hip",
            vr::TrackedDeviceClass_GenericTracker,
            &HipTracker);

    // start the camera server (handles all the camera inputs)
    StartCameraServer();

    return vr::VRInitError_None;
}

void PmfbtDriver::Cleanup() {
    StopCameraServer();

    VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

const char* const* PmfbtDriver::GetInterfaceVersions() {
    return vr::k_InterfaceVersions;
}

void PmfbtDriver::RunFrame() {
    vr::VREvent_t event{};
    while (vr::VRServerDriverHost()->PollNextEvent(&event, sizeof(event))) {
        // TODO: handle the event
    }
}

bool PmfbtDriver::ShouldBlockStandbyMode() {
    return false;
}

void PmfbtDriver::EnterStandby() {
    // TODO: pause the tracking
}

void PmfbtDriver::LeaveStandby() {
    // TODO: resume the tracking
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PmfbtDriver& GetDriverInstance() {
    static PmfbtDriver instance;
    return instance;
}

extern "C" void* HmdDriverFactory(const char* interface_name, int* return_code) {
    if (std::string_view(interface_name) == vr::IServerTrackedDeviceProvider_Version) {
        return &GetDriverInstance();
    } else {
        return nullptr;
    }
}