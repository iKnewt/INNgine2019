#ifndef BSPLINECURVE_H
#define BSPLINECURVE_H
#include "vector3d.h"
#include <vector>

///Used for movement for any Entity with the AI Component.
class BSplineCurve
{
public:
    /**
     * Constructor for B-Splines.
     * @param controlpoints Used to define the control points the B-Spline has.
     * @param degree Used to define the Degree of the B-Spline.
     */
    BSplineCurve(std::vector<gsl::Vector3D> controlpoints = {0}, size_t degree = 2) :
        controlPoints_(controlpoints), controlPoints_default_(controlpoints), degree_(degree)
    {
        CreateKnots();
    }
    /**
     * Finds a specified point in the B-Spline, and returns it.
     * @param knotInterval The index of the knot closest behind the point. The Function FindKnotInterval is used for this.
     * @param locationOnBSpline Used to define the specific point on the B-Spline. Can be from 0 to 1.
     * @return the point on the B-Spline.
     */
    gsl::Vector3D EvaluateBSpline(int knotInterval, float locationOnBSpline);
    /**
     * Creates the knots of the B-Spline.
     */
    void CreateKnots();
    /**
     * Finds the index of the knot closest behind the location of the B-Spline. primarily used with the EvaluateBSpline function.
     * @param locationOnBSpline Used to define the specific point on the B-Spline. Can be from 0 to 1.
     * @return returns the index of the knot closest behind of the location of the B-Spline.
     */
    int findKnotInterval(float locationOnBSpline);
    /**
     * Adds a control point in the B-Spline.
     * @param controlPoint The location to add.
     */
    void AddControlPoint(gsl::Vector3D controlPoint);
    /**
     * Randomizes the control points of the B-Spline, ultimately creating a new B-Spline.
     */
    void RandomizeControlpointOrder();
    /**
     * Swaps first and last control points.
     */
    void SetStartToEnd();
    /**
     * Removes all control points specified in the controlPointIndexesToRemove_ container.
     */
    void RemoveControlPoints();
    /**
     * Saves the default control points.
     */
    void UpdateDefaultControlPoints();
    /**
     * Sets the control points to be the default.
     */
    void ResetControlPoints();
    /**
     * Updates a control point's location.
     * @param value the new location of the selected control point.
     * @param index the control point's index in the vector.
     */
    void UpdatePoint(gsl::Vector3D value, unsigned int index = 0);
    /**
     * Overrides controlpoints with new controlpoints.
     * @param controlpoints New controlpoints.
     */
    void OverrideControlpoints(std::vector<gsl::Vector3D> controlpoints);

    ///The control points the B-Spline has.
    std::vector<gsl::Vector3D> controlPoints_;
    ///The default control points, gets set when the game gets started.
    std::vector<gsl::Vector3D> controlPoints_default_;
    ///The degree of the B-Spline.
    size_t degree_{2};
    ///The knots of the B-Spline. Created automatically.
    std::vector<float> knots_;
    ///The index of the control points that are to be removed.
    std::vector<size_t> controlPointIndexesToRemove_;
};

#endif // BSPLINECURVE_H
