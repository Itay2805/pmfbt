#include <openvr_driver.h>

#include <string_view>
#include <array>

#include "PmfbtTracker.hpp"

static const char* trackerSerialNumbers[3] = {
    "PMFBT Left Leg",
    "PMFBT Right Leg",
    "PMFBT Hip"
};

class PmfbtDriver final : public vr::IServerTrackedDeviceProvider {
private:
    std::array<PmfbtTracker, 3> trackers;

public:

    vr::EVRInitError Init(vr::IVRDriverContext* driver_context) final {
        VR_INIT_SERVER_DRIVER_CONTEXT(driver_context);

        // TODO: initialize the tracking

        // add all devices
        for (int i = 0; i < 3; i++) {
            vr::VRServerDriverHost()->TrackedDeviceAdded(
                    trackerSerialNumbers[i],
                    vr::TrackedDeviceClass_GenericTracker,
                    &trackers[i]);
        }

        return vr::VRInitError_None;
    }

    void Cleanup() final {
        // TODO: shutdown the tracking

        VR_CLEANUP_SERVER_DRIVER_CONTEXT();
    }

    const char* const* GetInterfaceVersions() final {
        return vr::k_InterfaceVersions;
    }

    void RunFrame() final {
        vr::VREvent_t event{};
        while (vr::VRServerDriverHost()->PollNextEvent(&event, sizeof(event))) {
            // TODO: handle the event
        }
    }

    bool ShouldBlockStandbyMode() final {
        return false;
    }

    void EnterStandby() final {
        // TODO: pause the tracking
    }

    void LeaveStandby() final {
        // TODO: resume the tracking
    }
};

static PmfbtDriver& GetDriverInstance() {
    static PmfbtDriver instance;
    return instance;
}

extern "C" __declspec(dllexport) void* HmdDriverFactory(const char* interface_name, int* return_code) {
    if (std::string_view(interface_name) == vr::IServerTrackedDeviceProvider_Version) {
        return &GetDriverInstance();
    } else {
        return nullptr;
    }
}