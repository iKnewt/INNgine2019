#include "quaternion.h"
#include "gsl_math.h"

namespace gsl
{

float gsl::Quaternion::Length()
{
    return std::sqrt(x * x + y * y + z * z + w * w);
}

void gsl::Quaternion::Normalize()
{
    float temp = Length();
    x /= temp;
    y /= temp;
    z /= temp;
    w /= temp;

}

Quaternion Quaternion::Conjugate()
{
    return Quaternion(-x,-y,-z,w);
}

Quaternion Quaternion::Rotate(float angle, Vector3D axis)
{
    float sinAngleHalf = std::sin(gsl::deg2radf(angle / 2));
    float cosAngleHalf = std::cos(gsl::deg2radf(angle / 2));
    float mx = axis.x * sinAngleHalf;
    float my = axis.y * sinAngleHalf;
    float mz = axis.z * sinAngleHalf;
    float mw = cosAngleHalf;
    return Quaternion(mx,my,mz,mw);
}

}
