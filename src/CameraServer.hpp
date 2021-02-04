#pragma once

#include "PmfbtDriver.hpp"

/**
 * Start the camera server thread, it will stream position
 * updates to the trackers
 */
[[noreturn]] void StartCameraServer();

/**
 * Stop the camera server
 */
void StopCameraServer();

