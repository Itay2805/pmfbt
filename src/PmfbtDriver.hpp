#pragma once

#include <string_view>
#include <array>

#include <openvr_driver.h>

#include "PmfbtTracker.hpp"

class PmfbtDriver final : public vr::IServerTrackedDeviceProvider {
public:
    PmfbtTracker RightLegTracker;
    PmfbtTracker LeftLegTracker;
    PmfbtTracker HipTracker;

public:
    vr::EVRInitError Init(vr::IVRDriverContext* driver_context);
    void Cleanup();
    const char* const* GetInterfaceVersions();
    void RunFrame();
    bool ShouldBlockStandbyMode();
    void EnterStandby();
    void LeaveStandby();
};

/**
 * Get the global instance of the driver so we can
 * access it from anywhere
 */
PmfbtDriver& GetDriverInstance();
