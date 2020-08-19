#ifndef GSL_MATH_H
#define GSL_MATH_H

#include "vector2d.h"
#include "vector3d.h"
#include "vector4d.h"
#include "math_constants.h"
#include "Legacy/gltypes.h"
#include "matrix3x3.h"
#include <vector>

namespace gsl
{
//Functions
GLdouble rad2deg(GLdouble rad);
GLdouble deg2rad(GLdouble deg);
GLfloat rad2degf(GLfloat rad);
GLfloat deg2radf(GLfloat deg);
GLfloat clamp(GLfloat x, GLfloat min, GLfloat max);

/**
 * Projects points on a vector specified.
 * @param points the points to project.
 * @param vector The vector to project it on.
 * @return the points projected on the vector.
 */
std::vector<gsl::Vector3D> ProjectPointsOnVector(std::vector<gsl::Vector3D> points, gsl::Vector3D vector);
/**
 * Finds the max and min value of all vectors given in X.
 * @param points The vector specified.
 * @return the min and max value, in a std::pair<min,max>.
 */
std::pair<float,float> findMinAndMaxInX(std::vector<gsl::Vector3D> points);
/**
 * Finds the max and min value of all vectors given in Y.
 * @param points The vector specified.
 * @return the min and max value, in a std::pair<min,max>.
 */
std::pair<float,float> findMinAndMaxInY(std::vector<gsl::Vector3D> points);
/**
 * Finds the max and min value of all vectors given in Z.
 * @param points The vector specified.
 * @return the min and max value, in a std::pair<min,max>.
 */
std::pair<float,float> findMinAndMaxInZ(std::vector<gsl::Vector3D> points);
/**
 * Gives a random integer.
 * @param min The minimum value the integer can be.
 * @param max The maximum value the integer can be.
 * @return a random integer.
 */
int RandomNumber(int min, int max);


//Interpolation
//Remember time can only be between 0 and 1
Vector2D lerp2D(GLfloat time, Vector2D start, Vector2D end); // Move a straight line with linear time-step between points
Vector3D lerp3D(GLfloat time, Vector3D start, Vector3D end);

//Curves
Vector3D bezierCurve(std::vector<Vector3D> points, GLfloat t, unsigned long long degree = 3);
Vector3D bSpline(const std::vector<Vector3D> &points, const std::vector<GLfloat> &t, GLfloat x, unsigned long long degree = 3);

//Basic vector directions
Vector3D up();
Vector3D right();
Vector3D forward();
Vector3D one();
Vector3D zero();

float distanceToPlane(const Vector3D &point, const Vector3D &normal, const Vector3D &pointInPlane);
bool withinPlane(const Vector3D &point, Matrix4x4 &modelMatrix, Vector2D upright, Vector2D downleft);

Vector3D barycentricCoordinates(const Vector2D &p1, const Vector2D &p2, const Vector2D &p3, const Vector2D playerXZ);

} //namespace

#endif // GSL_MATH_H
