#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include "vector3d.h"
#include "vector4d.h"
#include "Legacy/gltypes.h"
#include <iostream>
#include <iomanip>

namespace gsl
{

class Matrix2x2;
class Matrix3x3;

class Matrix4x4
{
public:
    Matrix4x4(bool isIdentity = false);
    Matrix4x4(std::initializer_list<GLfloat> values);

    static Matrix4x4 identity();
    void setToIdentity();

    bool inverse();

    void translateX(GLfloat x = 0.f);
    void translateY(GLfloat y = 0.f);
    void translateZ(GLfloat z = 0.f);

    void setPosition(GLfloat x = 0.f, GLfloat y = 0.f, GLfloat z = 0.f);
    gsl::Vector3D getPosition();

    gsl::Vector3D GetForwardVector();

    //Rotate using EulerMatrix
    void rotateX(GLfloat degrees = 0.f);
    void rotateY(GLfloat degrees = 0.f);
    void rotateZ(GLfloat degrees = 0.f);

    /**
     * Rotates the matrix based on a quaternion.
     * @param x the X value fo the quaternion.
     * @param y the Y value fo the quaternion.
     * @param z the Z value fo the quaternion.
     * @param w the W value fo the quaternion.
     */
    void rotQuat(float x, float y, float z, float w);

    void scale(Vector3D s);
    void scale(GLfloat uniformScale);
    void scale(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ);

    GLfloat* constData();

    void transpose();

    void ortho(GLfloat l, GLfloat r, GLfloat b, GLfloat t, GLfloat nearPlane, GLfloat farPlane);
    void frustum(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    void perspective(GLfloat fieldOfView, GLfloat aspectRatio, GLfloat nearPlane, GLfloat farPlane);

    void lookAt(const Vector3D &eye, const Vector3D &center, const Vector3D &up_axis);

    void setRotationToVector(const Vector3D &direction, Vector3D up = Vector3D(0.f,1.f,0.f));

    void translate(GLfloat x = 0.f, GLfloat y = 0.f, GLfloat z = 0.f);
    void translate(Vector3D positionIn);

    Matrix2x2 toMatrix2();
    Matrix3x3 toMatrix3() const;

    GLfloat& operator()(const int &y, const int &x);
    GLfloat operator()(const int &y, const int &x) const;

    Matrix4x4 operator*(const Matrix4x4 &other);
    Vector4D operator*(const Vector4D &other);
    GLfloat operator[](const unsigned int index);

    GLfloat getFloat(int space);

    friend std::ostream& operator<<(std::ostream &output, const Matrix4x4 &mIn)
    {
        output << std::setprecision(4) <<
                  "{" << mIn.matrix[0] << "\t, " << mIn.matrix[4] << "\t, " << mIn.matrix[8] << "\t, " << mIn.matrix[12] << "}\n" <<
                  "{" << mIn.matrix[1] << "\t, " << mIn.matrix[5] << "\t, " << mIn.matrix[9] << "\t, " << mIn.matrix[13] << "}\n" <<
                  "{" << mIn.matrix[2] << "\t, " << mIn.matrix[6] << "\t, " << mIn.matrix[10] << "\t, " << mIn.matrix[14] << "}\n" <<
                  "{" << mIn.matrix[3] << "\t, " << mIn.matrix[7] << "\t, " << mIn.matrix[11] << "\t, " << mIn.matrix[15] << "}\n";
        return output;
    }
    Matrix4x4 inversed();
private:
    GLfloat matrix[16];
};

} //namespace

#endif // MATRIX4X4_H
