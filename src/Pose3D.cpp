#include <algorithm>
#include <fstream>
#include <cmath>

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
 *
 * The front pose is used as the position we are going
 * to reconstruct while the side pose is going to be
 * used to tell the algorithm the depth order.
 */
Pose3D::Pose3D(const std::array<vector2, 15>& coco_pose, const std::array<int, 11>& relorder) {
    // use the point2pos to convert each of the front 2d points to a 3d
    // point and use the side picture to know the depth order
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
            s_constraint(points[0] - collarpoint, SHOULDER / 2),
            s_constraint(points[1] - collarpoint, SHOULDER / 2),
            s_constraint(collarpoint - tailpoint, SPINE),
            s_constraint(points[2] - tailpoint, PELVIC / 2.0),
            s_constraint(points[3] - tailpoint, PELVIC / 2.0),
            s_constraint(points[4] - points[0], UPPER_ARM),
            s_constraint(points[5] - points[1], UPPER_ARM),
            s_constraint(points[6] - points[2], THIGH),
            s_constraint(points[7] - points[3], THIGH),
            s_constraint(points[8] - points[4], FOREARM),
            s_constraint(points[9] - points[5], FOREARM),
            s_constraint(points[10] - points[6], FORELEG),
            s_constraint(points[11] - points[7], FORELEG),
            0
    };

    // Set to specified scale or minimum allowed scale
    float s = *std::max_element(smins.begin(), smins.end());

    /*
     * Now compute the 3D positions assuming the scale and using the
     * relative ordering.
     */

    // Set right shoulder at (0,0,0).
    joints[3] = vector3::zero();

    // left shoulder
    vector2 temp = joints[3].xy() + (points[1] - points[0]) / s;
    joints[1] = vector3(
            temp.x, temp.y,
            joints[3].z - relorder[0] * dz(points[1] - points[0], SHOULDER, s)
    );

    // collarbone
    joints[1] = (joints[3] + joints[4]) / 2.0;

    // tailbone
    temp = joints[1].xy() + (tailpoint - collarpoint) / s;
    joints[2] = vector3(
            temp.x, temp.y,
            joints[1].z - relorder[1] * dz(tailpoint - collarpoint, SPINE, s)
    );

    // right hip
    temp = joints[2].xy() + (points[2] - tailpoint) / s;
    joints[5] = vector3(
            temp.x, temp.y,
            joints[2].z - relorder[2] * dz(points[2] - tailpoint, PELVIC / 2.0, s)
    );

    // left hip
    temp = joints[2].xy() + (points[3] - tailpoint) / s;
    joints[6] = vector3(
            temp.x, temp.y,
            joints[2].z - relorder[2] * dz(points[3] - tailpoint, PELVIC / 2.0, s)
    );

    // right elbow
    temp = joints[3].xy() + (points[4] - points[0]) / s;
    joints[7] = vector3(
            temp.x, temp.y,
            joints[3].z - relorder[3] * dz(points[4] - points[0], UPPER_ARM, s)
    );

    // left elbow
    temp = joints[4].xy() + (points[5] - points[1]) / s;
    joints[8] = vector3(
            temp.x, temp.y,
            joints[4].z - relorder[4] * dz(points[5] - points[1], UPPER_ARM, s)
    );

    // right knee
    temp = joints[5].xy() + (points[6] - points[2]) / s;
    joints[9] = vector3(
            temp.x, temp.y,
            joints[5].z - relorder[5] * dz(points[6] - points[2], THIGH, s)
    );

    // left knee
    temp = joints[6].xy() + (points[7] - points[3]) / s;
    joints[10] = vector3(
            temp.x, temp.y,
            joints[6].z - relorder[6] * dz(points[7] - points[3], THIGH, s)
    );

    // right wrist
    temp = joints[7].xy() + (points[8] - points[4]) / s;
    joints[11] = vector3(
            temp.x, temp.y,
            joints[7].z - relorder[7] * dz(points[8] - points[4], FOREARM, s)
    );

    // left wrist
    temp = joints[8].xy() + (points[9] - points[5]) / s;
    joints[12] = vector3(
            temp.x, temp.y,
            joints[8].z - relorder[8] * dz(points[9] - points[5], FOREARM, s)
    );

    // right ankle
    temp = joints[9].xy() + (points[10] - points[6]) / s;
    joints[13] = vector3(
            temp.x, temp.y,
            joints[9].z - relorder[9] * dz(points[10] - points[6], FORELEG, s)
    );

    // left ankle
    temp = joints[10].xy() + (points[11] - points[7]) / s;
    joints[14] = vector3(
            temp.x, temp.y,
            joints[10].z - relorder[10] * dz(points[11] - points[7], FORELEG, s)
    );

    // compute the head position
    joints[0] = NECK / SPINE * (joints[1] - joints[2]) + joints[1];
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

