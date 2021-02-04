#pragma once

#include <array>

#include "math/vector3.hpp"

/**
 * The indexes of the joints
 */
enum JointType {
    JT_HEAD             = 0,
    JT_COLLARBONE       = 1,
    JT_TAILBONE         = 2,
    JT_RIGHT_SHOULDER   = 3,
    JT_LEFT_SHOULDER    = 4,
    JT_RIGHT_HIP        = 5,
    JT_LEFT_HIP         = 6,
    JT_RIGHT_ELBOW      = 7,
    JT_LEFT_ELBOW       = 8,
    JT_RIGHT_KNEE       = 9,
    JT_LEFT_KNEE        = 10,
    JT_RIGHT_WRIST      = 11,
    JT_LEFT_WRIST       = 12,
    JT_RIGHT_ANKLE      = 13,
    JT_LEFT_ANKLE       = 14,
};

/**
 * Represents a pair of two joints
 */
using JointPair = std::pair<JointType, JointType>;

/**
 * This is basically list of all the bones as a pair
 * of joints
 */
extern std::array<JointPair, 14> JOINT_PAIRS;

/**
 * Represents a single full pose of the person
 */
struct Pose3D {

    /**
     * Our joints
     */
    std::array<vector3, 15> joints;

    /**
     * Takes in the raw pose, which is 2d points + reldepth and turns
     * it into a 3d reconstruction of the pose
     */
    Pose3D(const std::array<vector2, hyperpose::COCO_N_PARTS>& coco_pose, const std::array<int, 11>& relorder);

    /**
     * Transform the pose given the hmd position
     *
     * @param head_pos  [IN] The position of the head
     */
    void SetHeadPosition(const vector3& head_pos);

    /**
     * Save as object
     *
     * @param name      [IN] The filename
     */
    void SaveAsObj(const char* name);
};
