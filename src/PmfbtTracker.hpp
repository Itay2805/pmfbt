#pragma once

#include <openvr_driver.h>

#include <cstdint>
#include <mutex>

#include <math/vector3.hpp>

class PmfbtTracker : public vr::ITrackedDeviceServerDriver {
private:
    uint32_t objectId;
    vr::DriverPose_t lastPose;
    std::mutex mutex;

public:

    PmfbtTracker();

    /**
     * This function will cause the tracker to update it's
     * internal pose to point to the given point.
     */
    void UpdatePoint(const vector3& new_point);

    /**
     * Update the device that the user is out-of-range (aka, we
     * can not find it)
     */
    void UpdateOutOfRange();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The OpenVR interface
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    vr::EVRInitError Activate(uint32_t unObjectId) final;
    void Deactivate() final;
    void *GetComponent( const char *pchComponentNameAndVersion ) final;
    void DebugRequest( const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize ) final;
    vr::DriverPose_t GetPose() final;
    void EnterStandby() final;
};
