#include "innpch.h"
#include "camera.h"

Camera::Camera()
{
    viewMatrix_.setToIdentity();
    projectionMatrix_.setToIdentity();
    CameraTransform_ = std::make_shared<TransformComponent>();
    FrustumTransform_ = std::make_shared<TransformComponent>();
    UpdateForwardVector();
}

void Camera::Pitch(float degrees)
{
    //  rotate around mRight
    pitch_ -= degrees;
    if(thirdPersonCamera_)
    pitch_ = gsl::clamp(pitch_, -75,-25);
    UpdateForwardVector();
}

void Camera::Yaw(float degrees)
{
    // rotate around mUp
    yaw_ -= degrees;
    UpdateForwardVector();
}

void Camera::UpdateRightVector()
{
    right_ = forward_ ^ up_;
    right_.normalize();

    if(thirdPersonCamera_)
    {
        rightXZ_ = right_;
        rightXZ_.y = 0;
        rightXZ_.normalize();
    }
}

void Camera::UpdateForwardVector()
{
    right_ = gsl::Vector3D(1.f, 0.f, 0.f);
    right_.rotateY(yaw_);
    right_.normalize();
    up_ = gsl::Vector3D(0.f, 1.f, 0.f);
    up_.rotateX(pitch_);
    up_.normalize();
    forward_ = up_ ^ right_;
    if(thirdPersonCamera_)
    {
        forwardXZ_ = forward_;
        forwardXZ_.y = 0;
        forwardXZ_.normalize();
    }

    UpdateRightVector();
}

void Camera::Update(std::shared_ptr<TransformComponent> transform)
{
    gsl::Matrix4x4 yawMatrix_;
    gsl::Matrix4x4 pitchMatrix_;
    yawMatrix_.setToIdentity();
    pitchMatrix_.setToIdentity();

    pitchMatrix_.rotateX(pitch_);
    yawMatrix_.rotateY(yaw_);
    if(thirdPersonCamera_)
    {
        viewMatrix_.setToIdentity();
        viewMatrix_ = pitchMatrix_* yawMatrix_;
        float horDist = -distanceFromPlayer_ * std::cos(gsl::deg2radf(pitch_));
        float virDist = -distanceFromPlayer_ * std::sin(gsl::deg2radf(pitch_));
        float x = horDist * std::sin(gsl::deg2radf(yaw_));
        float z = horDist * std::cos(gsl::deg2radf(yaw_));
        position_ = {transform->position_world_.x - x, transform->position_world_.y + virDist + 2, transform->position_world_.z - z};
        viewMatrix_.translate(-position_);
    }
    else
    {
        yawMatrix_.setToIdentity();
        pitchMatrix_.setToIdentity();

        pitchMatrix_.rotateX(pitch_);
        yawMatrix_.rotateY(yaw_);

        position_ -= forward_ * speed_;

        viewMatrix_ = pitchMatrix_* yawMatrix_;
        viewMatrix_.translate(-position_);



    }
    CameraTransform_->position_relative_ = position();
    CameraTransform_->rotation_relative_ = {pitch_,yaw_,0};
    UpdateForwardVector();

    //Visualizing Frustum
    gsl::Matrix4x4 tempView = viewMatrix_;
    gsl::Matrix4x4 tempProjection = projectionMatrix_;
    //the function inversed() does not work
    tempView.inverse();
    tempProjection.inverse();
    FrustumTransform_->transform_ = tempView * tempProjection;
    FrustumTransform_->transform_.scale({2,2,2});
    UpdateFrustum();
}

void Camera::SetProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane)
{
    projectionMatrix_.perspective(fov, aspect, nearPlane, farPlane);
}

void Camera::SetProjectionMatrix()
{
    projectionMatrix_.perspective(fieldOfView_, aspectRatio_, nearPlane_, farPlane_);
}

void Camera::UpdateFrustum()
{    
    gsl::Matrix4x4 vpMatrix = projectionMatrix_ * viewMatrix_;

    gsl::Vector3D col1(vpMatrix[0], vpMatrix[1], vpMatrix[2]);
    gsl::Vector3D col2(vpMatrix[4], vpMatrix[5], vpMatrix[6]);
    gsl::Vector3D col3(vpMatrix[8], vpMatrix[9], vpMatrix[10]);
    gsl::Vector3D col4(vpMatrix[12], vpMatrix[13], vpMatrix[14]);

    //Layout //left, right, top, bottom, farPlane, nearPlane
    frustum_[0].normal = col4 + col1;
    frustum_[1].normal = col4 - col1;
    frustum_[2].normal = col4 - col2;
    frustum_[3].normal = col4 + col2;
    frustum_[4].normal = col4 - col3;
    frustum_[5].normal = col3;

    //Layout //left, right, top, bottom, farPlane, nearPlane
    frustum_[0].distanceToOrigo = vpMatrix[15] + vpMatrix[3];
    frustum_[1].distanceToOrigo = vpMatrix[15] - vpMatrix[3];
    frustum_[2].distanceToOrigo = vpMatrix[15] - vpMatrix[7];
    frustum_[3].distanceToOrigo = vpMatrix[15] + vpMatrix[7];
    frustum_[4].distanceToOrigo = vpMatrix[15] - vpMatrix[11];
    frustum_[5].distanceToOrigo = vpMatrix[11];

    for (int i = 0; i < 6; ++i)
    {
        float magnitude = 1.0f / frustum_[i].normal.length();
        frustum_[i].normal = frustum_[i].normal * magnitude;
        frustum_[i].distanceToOrigo *= magnitude;
    }
}

void Camera::SetPosition(const gsl::Vector3D &position)
{
    position_ = position;
}

void Camera::SetSpeed(float speed)
{
    speed_ = speed;
}

void Camera::UpdateHeight(float deltaHeigth)
{
    position_.y += deltaHeigth;
}

void Camera::MoveRight(float delta)
{
    //This fixes a bug in the up and right calculations
    //so camera always holds its height when straifing
    //should be fixed thru correct right calculations!
    gsl::Vector3D right = right_;
    right.y = 0.f;
    position_ += right * delta;
}

gsl::Vector3D Camera::position() const
{
    return position_;
}

gsl::Vector3D Camera::up() const
{
    return up_;
}

gsl::Vector3D Camera::forward() const
{
    return forward_;
}
void Camera::FocusOnLocation(gsl::Vector3D position)
{
    SetPosition(position - forward() * 5);
}
