#ifndef QUATERNION_H
#define QUATERNION_H

#include "vector3d.h"

namespace gsl
{
/**
 * quaternion class used for rotations.
 */
class Quaternion
{
public:
    Quaternion(float xIn, float yIn, float zIn, float wIn) : x(xIn),y(yIn),z(zIn),w(wIn){}
    float x;
    float y;
    float z;
    float w;

    /**
     * Calculates the length of the quaternion.
     * @return the length of the quaternion.
     */
    float Length();
    /**
     * Normalizes the quaternion.
     */
    void Normalize();
    /**
     * Creates a conjugated Quaternion.
     * @return the conjugated Quaternion.
     */
    Quaternion Conjugate();
    /**
     * Rotates the Quaternion based on Euler angles.
     * @param angle the angle specified.
     * @param axis The axis to be rotated.
     * @return the rotated Quaternion.
     */
    Quaternion Rotate(float angle, gsl::Vector3D axis);

    //Vector3D operator*(GLfloat rhs) const;
    Quaternion operator*(Quaternion r)
    {
        float w_ = w * r.w - x * r.x - y * r.y - z * r.z;
        float x_ = x * r.w + w * r.x + y * r.z - z * r.y;
        float y_ = y * r.w + w * r.y + z * r.x - x * r.z;
        float z_ = z * r.w + w * r.z + x * r.y - y * r.x;
        return Quaternion(x_,y_,z_,w_);
    }
    Quaternion operator*(gsl::Vector3D r)
    {
        float x_ =  w * r.x + y * r.z - z * r.y;
        float y_ =  w * r.y + z * r.x - x * r.z;
        float z_ =  w * r.z + x * r.y - y * r.x;
        float w_ = -x * r.x - y * r.y - z * r.z;
        return Quaternion(x_,y_,z_,w_);
    }

};
}
#endif // QUATERNION_H
