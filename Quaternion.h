#include <iostream>
#include "Matrix4.h"
// Quaternion class to do rotations using quaternions
class Quaternion
{
public:
    float w, x, y, z;
    Quaternion() = default;
    Quaternion(float angle, const Cartesian3& v); // Rotate by an angle around some axis
    Quaternion(float w, float x, float y, float z); // Set quaternion values
    Matrix4 ToRotationMatrix(); // Convert the quaternion to a rotation matrix
    void Normalize(); // Normalize the quaternion 
    Quaternion Conjugate(); // Get the conjugate of the quaternion
    Quaternion operator*(float f) const;
    Quaternion operator-(const Quaternion& other) const;
    Quaternion operator-() const;
    Quaternion operator+(const Quaternion& other) const;
};

// Multiply Quaternion by another quaternion 
inline Quaternion operator*(const Quaternion& left, const Quaternion& right)
{
    float w = (left.w * right.w) - (left.x * right.x) - (left.y * right.y) - (left.z * right.z);
    float x = (left.x * right.w) + (left.w * right.x) + (left.y * right.z) - (left.z * right.y);
    float y = (left.y * right.w) + (left.w * right.y) + (left.z * right.x) - (left.x * right.z);
    float z = (left.z * right.w) + (left.w * right.z) + (left.x * right.y) - (left.y * right.x);

    return {w, x, y, z};
}

// Multiply quaternion with a vector
inline Quaternion operator*(const Quaternion& rot, const Cartesian3& v)
{
    float w = - (rot.x * v.x) - (rot.y * v.y) - (rot.z * v.z);
    float x =   (rot.w * v.x) + (rot.y * v.z) - (rot.z * v.y);
    float y =   (rot.w * v.y) + (rot.z * v.x) - (rot.x * v.z);
    float z =   (rot.w * v.z) + (rot.x * v.y) - (rot.y * v.x);

    return {w, x, y, z};
}


inline Quaternion operator*(float f, const Quaternion& q) {
    return Quaternion(f * q.w, f * q.x, f * q.y, f * q.z);
}


inline Quaternion Slerp(Quaternion q1, Quaternion q2, float t)
{
    // Compute the cosine of the angle between the two vectors.
    float dot = q1.w*q2.w + q1.x*q2.x + q1.y*q2.y + q1.z*q2.z;

    const float DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD)
    {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.
        Quaternion result = (1 - t) * q1 + t * q2;
        result.Normalize();
        return result;
    }

    //If the dot product is negative, slerp won't take
    // the shorter path. Note that v1 and -v1 are equivalent when
    // the negation is applied to all four components. Fix by 
    // reversing one quaternion.
    if (dot < 0.0f)
    {
        q2 = -q2;
        dot = -dot;
    }

    if (dot < -1) dot = -1;
    if (dot > 1) dot = 1;

    // theta0 = angle between input vectors
    // theta = angle between v0 and result 
    float theta0 = std::acos(dot);
    float theta = theta0*t;

    Quaternion q3 = q2 - q1*dot;
    q3.Normalize(); // { v0, v2 } plane
    return q1*std::cos(theta) + q3*std::sin(theta);

    // Quaternion result = (1 - t) * q1 + t * q2;
    // result.Normalize();
    // return result;
}
