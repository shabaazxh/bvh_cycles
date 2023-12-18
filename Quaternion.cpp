#include "Quaternion.h"

Quaternion::Quaternion(float angle, const Cartesian3& v)
{
    float angleRadians = angle * M_PI / 180.0;
    float rotAngle = angleRadians / 2.0f;

    float cosAngle = std::cos(rotAngle);
    float sinAngle = std::sin(rotAngle);

    w = cosAngle;
    x = v.x * sinAngle;
    y = v.y * sinAngle;
    z = v.z * sinAngle;
}


Quaternion::Quaternion(float w, float x, float y, float z)
{
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}

// Turn quaternion rotation into a rotation matrix
Matrix4 Quaternion::ToRotationMatrix()
{
    float x2  = x + x;
    float y2  = y + y;
    float z2  = z + z;
    float xx2 = x * x2;
    float xy2 = x * y2;
    float xz2 = x * z2;
    float yy2 = y * y2;
    float yz2 = y * z2;
    float zz2 = z * z2;
    float sx2 = w * x2;
    float sy2 = w * y2;
    float sz2 = w * z2;

    Matrix4 ret = Matrix4::Identity();

    ret.coordinates[0][0] = 1 - (yy2 + zz2);
    ret.coordinates[1][0] = xy2 - sz2;
    ret.coordinates[2][0] = xz2 + sy2;
    ret.coordinates[3][0] = 0.0f;

    ret.coordinates[0][1] = xy2 + sz2;
    ret.coordinates[1][1] = 1 - (xx2 + zz2);
    ret.coordinates[2][1] = yz2 - sx2;
    ret.coordinates[3][1] = 0.0f;

    ret.coordinates[0][2] = xz2 - sy2;
    ret.coordinates[1][2] = yz2 + sx2;
    ret.coordinates[2][2] = 1 - (xx2 + yy2);
    ret.coordinates[3][2] = 0.0f;

    ret.coordinates[0][3] = 0.0f;
    ret.coordinates[1][3] = 0.0f;
    ret.coordinates[2][3] = 0.0f;
    ret.coordinates[3][3] = 1.0f;

    return ret;
}

void Quaternion::Normalize()
{
    float magnitude = sqrt(w*w + x*x + y*y + z*z);

    w /= magnitude;
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
}

Quaternion Quaternion::Conjugate()
{
    return {w, -x, -y, -z};
}

Quaternion Quaternion::operator*(float f) const {
    return Quaternion(w * f, x * f, y * f, z * f);
}

Quaternion Quaternion::operator-(const Quaternion& other) const
{
    return Quaternion(w - other.w, x - other.x, y - other.y, z - other.z);
}

Quaternion Quaternion::operator-() const
{
    return Quaternion(-w, -x, -y, -z);
}

Quaternion Quaternion::operator+(const Quaternion& other) const {
    return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
}


