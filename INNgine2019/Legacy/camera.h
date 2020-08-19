#ifndef CAMERA_H
#define CAMERA_H

#include "Managers/componentmanager.h"
#include "innpch.h"
#ifdef _WIN32
#include <al.h>
#include <alc.h>
#endif
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

class Material;
class Mesh;
/**
 * The Camera containing the View and Projection matrices.
 */
class Camera
{
public:
    Camera();
    /**
     * rotates the camera in the pitch direction with the specified amount given.
     * @param degrees the amount to rotate the camera.
     */
    void Pitch(float degrees);
    /**
     * rotates the camera in the yaw direction with the specified amount given.
     * @param degrees the amount to rotate the camera.
     */
    void Yaw(float degrees);
    /**
     * Updates the camera's right vector.
     */
    void UpdateRightVector();
    /**
     *Updates the camera's forward vector, use the Forward() function to get the forward vector.
     */
    void UpdateForwardVector();
    /**
     * Updates the camera based on either movement from the Entity it follows or from mouse input.
     * @param transform if the camera is a thid person camera, the transform of the entity to target.
     */
    void Update(std::shared_ptr<TransformComponent> transform = nullptr);
    /**
     * sets the projection matrix based on the values defined.
     * @param fov the Field of View.
     * @param aspect the Aspect Ratio.s
     * @param nearPlane How close a rendered object is allowed to be the camera.
     * @param farPlane How far away a rendered object is allowed to be the camera before it does not get rendered anymore.
     */
    void SetProjectionMatrix(float fov, float aspect, float nearPlane = 0.5f, float farPlane = 200.f);
    /**
     * Sets the projection matrix based on the values given in the camera instance.
     */
    void SetProjectionMatrix();
    /**
     * Updates the frustum.
     */
    void UpdateFrustum();

    ///The name of the camera. Used in the UI to differenciate each camera.
    QString name_;
    ///Indicates if the camera is allowed to move or not. If false, the camera won't move.
    bool movable_{true};
    ///Indicates if the camera is a third person camera. if true, it will follow a specified Entity.
    bool thirdPersonCamera_{false};

    ///View matrix of the camera.
    gsl::Matrix4x4 viewMatrix_;
    ///Projection matrix of the camera.
    gsl::Matrix4x4 projectionMatrix_;

    /**
     * Sets the new position of the camera.
     * @param position The new position of the camera.
     */
    void SetPosition(const gsl::Vector3D &position);
    /**
     * Sets the movement speed of the camera.
     * @param speed The speed specified.
     */
    void SetSpeed(float speed);
    /**
     * Updates the height of the camera.
     * @param deltaHeigth To define the new height of the camera.
     */
    void UpdateHeight(float deltaHeigth);
    /**
     * Move the camera in the right direction.
     * @param delta The amount to move.
     */
    void MoveRight(float delta);

    /**
     * @return The position vector of the camera.
     */
    gsl::Vector3D position() const;
    /**
     * @return The up vector of the camera.
     */
    gsl::Vector3D up() const;
    /**
     * @return The forward vector of the camera.
     */
    gsl::Vector3D forward() const;
    /**
     * Focuses on a specified positon.
     * @param position The position to focus on.
     */
    void FocusOnLocation(gsl::Vector3D position);

    /// The forward vector of the camera.
    gsl::Vector3D forward_{0.f, 0.f, -1.f};
    /// The forward vector of the camera, without any information of the Y value.
    gsl::Vector3D forwardXZ_{0.f,0.f,0.f};
    /// The Right vector of hte camera.
    gsl::Vector3D right_{1.f, 0.f, 0.f};
        /// The right vector of the camera, without any information of the Y value.
    gsl::Vector3D rightXZ_{0.f,0.f,0.f};
    /// The up vector of the camera.
    gsl::Vector3D up_{0.f, 1.f, 0.f};
    /// If third person, this variable decides how far away the camera is from the entity.
    float distanceFromPlayer_{20.f};

    /// The position of the camera.
    gsl::Vector3D position_{0.f, 0.f, 0.f};
    /// The pitch rotation of the camera.
    float pitch_{0.f};
    /// The yaw rotation of the camera.
    float yaw_{0.f};

    float nearPlane_{0.5f};
    float farPlane_{5000.f};
    float fieldOfView_{45.f};
    float aspectRatio_{1.33f};

    /// frustum planes of camera.
    /// Order: left, right, top, bottom, farPlane, nearPlane.
    gsl::Plane frustum_[6]{};

    /// Speed of camera along the forward_ vector.
    float speed_{0.f};

    std::shared_ptr<TransformComponent> CameraTransform_{nullptr};
    std::shared_ptr<TransformComponent> FrustumTransform_{nullptr};
};

#endif // CAMERA_H
