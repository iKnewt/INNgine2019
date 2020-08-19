#include "bsplinecurve.h"
#include <random>

//My = knoten som er nærmest bak punktet (Dette kan vel bare bli funnet ut i kode, no?)
gsl::Vector3D BSplineCurve::EvaluateBSpline(int knotIndex, float locationOnBSpline)
{
    gsl::Vector3D posOnSpline[20]; // forutsetter da at n+d+1 <= 20
    for (size_t j = 0; j <= degree_; j++)
    {
        posOnSpline[degree_ - j] = controlPoints_[static_cast<unsigned int >(knotIndex) - j];
    }

    for (size_t k = degree_; k > 0; k--)
    {
        size_t j = static_cast<size_t>(knotIndex) - k;
        for (size_t i = 0; i < k; i++)
        {
            j++;
            float w = (locationOnBSpline - knots_[j]) / (knots_[j + k] - knots_[j]);
            posOnSpline[i] = posOnSpline[i] * (1 - w) + posOnSpline[i + 1] * w;
        }
    }
    return posOnSpline[0];
}

void BSplineCurve::CreateKnots()
{
    knots_.clear();
    size_t amountOfKnots = degree_ + controlPoints_.size() + 1;
    //How many
    size_t numberOfEndKnots = degree_ + 1;
    size_t numberOfMiddleKnots = amountOfKnots - (numberOfEndKnots * 2);

    for(size_t i = 1; i < amountOfKnots + 1; i++)
    {
        if(i <= degree_ + 1)
        {
            knots_.push_back(0);
            continue;
        }
        else if(i > amountOfKnots - (degree_ + 1))
        {
            knots_.push_back(1);
            continue;
        }
        else
        {
            float knot = 1 * (i - numberOfEndKnots);
            knot /= numberOfMiddleKnots + 1;
            knots_.push_back(knot);
            continue;
        }
    }
}

int BSplineCurve::findKnotInterval(float locationOnBSpline)
{
    int index = 0;
    for(size_t i = 0; i < knots_.size(); i++)
    {
        if(locationOnBSpline >= knots_[i])
            index = int(i);
    }
    return index;
}

void BSplineCurve::AddControlPoint(gsl::Vector3D controlPoint)
{
    controlPoints_.push_back(controlPoint);
    UpdateDefaultControlPoints();
    CreateKnots();
}

void BSplineCurve::RandomizeControlpointOrder()
{
    if(controlPoints_.size() > 3)
    {
//        std::random_device rd;
//        std::mt19937 g(rd());
//        std::shuffle(controlPoints_.begin() + 1, controlPoints_.end() - 1, g);
        std::random_shuffle(controlPoints_.begin() + 1, controlPoints_.end() - 1);
    }
    CreateKnots();
}

void BSplineCurve::SetStartToEnd()
{
    controlPoints_.front() = controlPoints_.back();
}

void BSplineCurve::RemoveControlPoints()
{
    if(!controlPointIndexesToRemove_.empty())
    {
        std::sort(controlPointIndexesToRemove_.begin(), controlPointIndexesToRemove_.end(), std::greater<int>());
        for(auto controlPointIndex : controlPointIndexesToRemove_)
        {
            controlPoints_.erase(controlPoints_.begin() + static_cast<long long>(controlPointIndex));
        }
        controlPointIndexesToRemove_.clear();
    }
}

void BSplineCurve::UpdateDefaultControlPoints()
{
    controlPoints_default_ = controlPoints_;
    CreateKnots();
}

void BSplineCurve::ResetControlPoints()
{
    controlPoints_ = controlPoints_default_;
    controlPointIndexesToRemove_.clear();
    CreateKnots();
}

void BSplineCurve::UpdatePoint(gsl::Vector3D value, unsigned int index)
{
    controlPoints_[index] = value;
    UpdateDefaultControlPoints();
}

void BSplineCurve::OverrideControlpoints(std::vector<gsl::Vector3D> controlpoints)
{
    controlPoints_ = controlpoints;
    controlPoints_default_ = controlPoints_;
    CreateKnots();
}
