#include <algorithm>
#include <fstream>
#include <cmath>
#include <hyperpose/hyperpose.hpp>

#include "math/vector3.hpp"
#include "math/vector2.hpp"

#include "Pose3D.hpp"

/**
 * Pairs of our model
 */
std::array<JointPair, 14> JOINT_PAIRS = {
    // right hand
    JointPair{JT_RIGHT_SHOULDER, JT_RIGHT_ELBOW },
    JointPair{JT_RIGHT_ELBOW, JT_RIGHT_WRIST },

    // left hand
    JointPair{JT_LEFT_SHOULDER, JT_LEFT_ELBOW },
    JointPair{JT_LEFT_ELBOW, JT_LEFT_WRIST },

    // head
    JointPair{JT_HEAD, JT_COLLARBONE },

    // main body
    JointPair{JT_COLLARBONE, JT_RIGHT_SHOULDER },
    JointPair{JT_COLLARBONE, JT_LEFT_SHOULDER },
    JointPair{JT_COLLARBONE, JT_TAILBONE },
    JointPair{JT_TAILBONE, JT_RIGHT_HIP },
    JointPair{JT_TAILBONE, JT_LEFT_HIP },

    // right leg
    JointPair{JT_RIGHT_HIP, JT_RIGHT_KNEE },
    JointPair{JT_RIGHT_KNEE, JT_RIGHT_ANKLE },

    // left leg
    JointPair{JT_LEFT_HIP, JT_LEFT_KNEE },
    JointPair{JT_LEFT_KNEE, JT_LEFT_ANKLE },
};

/**
 * Relative body proportion value
 */
constexpr float FOREARM = 14.0;
constexpr float UPPER_ARM = 15.0;
constexpr float SHOULDER = 18.0;
constexpr float FORELEG = 20.0;
constexpr float THIGH = 19.0;
constexpr float PELVIC = 14.0;
constexpr float SPINE = 24.0;
constexpr float NECK = 7.0;
constexpr float HEIGHT = 70.0;

/**
 * Convertion table to convert to our model
 */
static int OUR_TO_COCO[15] = {
    2, 5, 8, 11, 3, 6, 9, 12, 4, 7, 10, 13
};

/**
 * the relorder joint pair
 * -right shoulder - left shoulder
 * -collarbone - tailbone
 * -right hip - left hip
 * -right elbow - right shoulder
 * -left elbow - left shoulder
 * -right knee - right hip
 * -left knee - left hip
 * -right wrist - right elbow
 * -left wrist - left elbow
 * -right ankle - right knee
 * -left ankle - left knee
 */
static std::pair<int, int> relorder_joints[11] = {
    { 2, 5 },
    { 1, 1 }, // we don't have tailbone so ignore
    { 8, 11 },
    { 3, 2 },
    { 6, 5 },
    { 9, 8 },
    { 12, 11 },
    { 4, 3 },
    { 7, 6 },
    { 10, 9 },
    { 13, 12 },
};

/**
 * Check the minimum value of a scale implied by the constraint
 * on segment length and image position delta
 */
static float s_constraint(const vector2& vec, float length) {
    return sqrt(vec.x * vec.x + vec.y * vec.y) / length + 1e-10;
}

/**
 * Compute the |delta Z| implied by the delta image point,
 * length of segment and scale.
 */
static float dz(const vector2& dpoint, float length, float scale) {
    return sqrt(length * length - (dpoint.x * dpoint.x + dpoint.y * dpoint.y) / (scale * scale));
}

/**
 * We are going to use the algorithm as made in:
 *  https://github.com/cflamant/3d-pose-reconstruction/blob/master/code/pointstopos.py
 *
 * this should give us all the 3d info we may need.
 */
Pose3D::Pose3D(const std::array<vector2, hyperpose::COCO_N_PARTS>& coco_pose, const std::array<int, 11>& relorder) {
    // convert the coco model into a model that the 3d reconstruction uses
    std::array<vector2, 15> points{};
    for (int i = 0; i < 15; i++) {
        const auto& part = coco_pose[OUR_TO_COCO[i]];
        points[i].x = part.x;
        points[i].y = part.y;
    }

    // we will use the collarpoint provided by hyperpose for better results
    vector2 collarpoint = vector2(coco_pose[1].x, coco_pose[1].y);
    vector2 tailpoint = (points[JT_LEFT_HIP] + points[JT_RIGHT_HIP]) / 2;

    /*
     * First compute the constraint on the minimum size of s.
     */
    std::array<float, 14> smins = {
        s_constraint(points[JT_HEAD] - collarpoint, SHOULDER / 2),
        s_constraint(points[JT_COLLARBONE] - collarpoint, SHOULDER / 2),
        s_constraint(collarpoint - tailpoint, SPINE),
        s_constraint(points[JT_TAILBONE] - tailpoint, PELVIC / 2.0),
        s_constraint(points[JT_RIGHT_SHOULDER] - tailpoint, PELVIC / 2.0),
        s_constraint(points[JT_LEFT_SHOULDER] - points[JT_HEAD], UPPER_ARM),
        s_constraint(points[JT_RIGHT_HIP] - points[JT_COLLARBONE], UPPER_ARM),
        s_constraint(points[JT_LEFT_HIP] - points[JT_TAILBONE], THIGH),
        s_constraint(points[JT_RIGHT_ELBOW] - points[JT_RIGHT_SHOULDER], THIGH),
        s_constraint(points[JT_LEFT_ELBOW] - points[JT_LEFT_SHOULDER], FOREARM),
        s_constraint(points[JT_RIGHT_KNEE] - points[JT_RIGHT_HIP], FOREARM),
        s_constraint(points[JT_LEFT_KNEE] - points[JT_LEFT_HIP], FORELEG),
        s_constraint(points[JT_RIGHT_WRIST] - points[JT_RIGHT_ELBOW], FORELEG),
        0
    };

    // Set to specified scale or minimum allowed scale
    float s = *std::max_element(smins.begin(), smins.end());

    /*
     * Now compute the 3D positions assuming the scale and using the
     * relative ordering.
     */

    // Set right shoulder at (0,0,0).
    joints[JT_RIGHT_SHOULDER] = vector3::zero();

    // left shoulder
    vector2 temp = joints[JT_RIGHT_SHOULDER].xy() + (points[JT_COLLARBONE] - points[JT_HEAD]) / s;
    joints[JT_COLLARBONE] = vector3(
            temp.x, temp.y,
            joints[JT_RIGHT_SHOULDER].z - relorder[0] * dz(points[JT_COLLARBONE] - points[JT_HEAD], SHOULDER, s)
    );

    // collarbone
    joints[JT_COLLARBONE] = (joints[JT_RIGHT_SHOULDER] + joints[JT_LEFT_SHOULDER]) / 2.0;

    // tailbone
    temp = joints[JT_COLLARBONE].xy() + (tailpoint - collarpoint) / s;
    joints[JT_TAILBONE] = vector3(
        temp.x, temp.y,
        joints[JT_COLLARBONE].z - relorder[1] * dz(tailpoint - collarpoint, SPINE, s)
    );

    // right hip
    temp = joints[JT_TAILBONE].xy() + (points[JT_TAILBONE] - tailpoint) / s;
    joints[JT_RIGHT_HIP] = vector3(
        temp.x, temp.y,
        joints[JT_TAILBONE].z - relorder[2] * dz(points[JT_TAILBONE] - tailpoint, PELVIC / 2.0, s)
    );

    // left hip
    temp = joints[JT_TAILBONE].xy() + (points[JT_RIGHT_SHOULDER] - tailpoint) / s;
    joints[JT_LEFT_HIP] = vector3(
        temp.x, temp.y,
        joints[JT_TAILBONE].z - relorder[2] * dz(points[JT_RIGHT_SHOULDER] - tailpoint, PELVIC / 2.0, s)
    );

    // right elbow
    temp = joints[JT_RIGHT_SHOULDER].xy() + (points[JT_LEFT_SHOULDER] - points[JT_HEAD]) / s;
    joints[JT_RIGHT_ELBOW] = vector3(
        temp.x, temp.y,
        joints[JT_RIGHT_SHOULDER].z - relorder[3] * dz(points[JT_LEFT_SHOULDER] - points[JT_HEAD], UPPER_ARM, s)
    );

    // left elbow
    temp = joints[JT_LEFT_SHOULDER].xy() + (points[JT_RIGHT_HIP] - points[JT_COLLARBONE]) / s;
    joints[JT_LEFT_ELBOW] = vector3(
        temp.x, temp.y,
        joints[JT_LEFT_SHOULDER].z - relorder[4] * dz(points[JT_RIGHT_HIP] - points[JT_COLLARBONE], UPPER_ARM, s)
    );

    // right knee
    temp = joints[JT_RIGHT_HIP].xy() + (points[JT_LEFT_HIP] - points[JT_TAILBONE]) / s;
    joints[JT_RIGHT_KNEE] = vector3(
        temp.x, temp.y,
        joints[JT_RIGHT_HIP].z - relorder[5] * dz(points[JT_LEFT_HIP] - points[JT_TAILBONE], THIGH, s)
    );

    // left knee
    temp = joints[JT_LEFT_HIP].xy() + (points[JT_RIGHT_ELBOW] - points[JT_RIGHT_SHOULDER]) / s;
    joints[JT_LEFT_KNEE] = vector3(
        temp.x, temp.y,
        joints[JT_LEFT_HIP].z - relorder[6] * dz(points[JT_RIGHT_ELBOW] - points[JT_RIGHT_SHOULDER], THIGH, s)
    );

    // right wrist
    temp = joints[JT_RIGHT_ELBOW].xy() + (points[JT_LEFT_ELBOW] - points[JT_LEFT_SHOULDER]) / s;
    joints[JT_RIGHT_WRIST] = vector3(
        temp.x, temp.y,
        joints[JT_RIGHT_ELBOW].z - relorder[7] * dz(points[JT_LEFT_ELBOW] - points[JT_LEFT_SHOULDER], FOREARM, s)
    );

    // left wrist
    temp = joints[JT_LEFT_ELBOW].xy() + (points[JT_RIGHT_KNEE] - points[JT_RIGHT_HIP]) / s;
    joints[JT_LEFT_WRIST] = vector3(
        temp.x, temp.y,
        joints[JT_LEFT_ELBOW].z - relorder[8] * dz(points[JT_RIGHT_KNEE] - points[JT_RIGHT_HIP], FOREARM, s)
    );

    // right ankle
    temp = joints[JT_RIGHT_KNEE].xy() + (points[JT_LEFT_KNEE] - points[JT_LEFT_HIP]) / s;
    joints[JT_RIGHT_ANKLE] = vector3(
        temp.x, temp.y,
        joints[JT_RIGHT_KNEE].z - relorder[9] * dz(points[JT_LEFT_KNEE] - points[JT_LEFT_HIP], FORELEG, s)
    );

    // left ankle
    temp = joints[JT_LEFT_KNEE].xy() + (points[JT_RIGHT_WRIST] - points[JT_RIGHT_ELBOW]) / s;
    joints[JT_LEFT_ANKLE] = vector3(
        temp.x, temp.y,
        joints[JT_LEFT_KNEE].z - relorder[10] * dz(points[JT_RIGHT_WRIST] - points[JT_RIGHT_ELBOW], FORELEG, s)
    );

    // compute the head position
    joints[JT_HEAD] = NECK / SPINE * (joints[JT_COLLARBONE] - joints[JT_TAILBONE]) + joints[JT_COLLARBONE];
}

void Pose3D::SetHeadPosition(const vector3& head_pos) {
    auto offset = head_pos - joints[JT_HEAD];
    for (auto& joint : joints) {
        joint -= offset;
    }
}

void Pose3D::SaveAsObj(const char* name) {
    std::ofstream out(name);

    // output the points
    for (const auto& joint : joints) {
        out << "v " << joint.x << " " << joint.y << " " << joint.z << " 1.0" << std::endl;
    }

    // output the lines
    for (const auto& pair : JOINT_PAIRS) {
        out << "l " << static_cast<int>(pair.first) << " " << static_cast<int>(pair.second) << std::endl;
    }
}

