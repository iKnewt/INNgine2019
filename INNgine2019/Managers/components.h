#ifndef COMPONENTS_H
#define COMPONENTS_H

#ifdef _WIN32
#include <al.h>
#include <alc.h>
#endif
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

#include <cstddef>
#include "GSL/vector3d.h"
#include "GSL/bsplinecurve.h"
#include <QJsonObject>
#include <QJsonArray>

class Mesh;
class Material;
class BSplineCurve;
class BaseComponent;

/// Used to define what type a component is quickly.
enum ComponentType
{
    TRANSFORM,
    MESH,
    AUDIO,
    LIGHT,
    AI,
    NONE
};

/// Used to print component type names when needed, corresponds to ComponentType enum.
static const QString COMPONENT_TYPE_NAMES[]
{
    "TransformComponent",
    "MeshComponent",
    "AudioComponent",
    "LightComponent",
    "AIComponent",
    "invalid Component type"
};

/// Holds data used by all component types.
/// Makes it easy to place all components in one vector for iteration later.
class BaseComponent
{
public:
    BaseComponent(){}
    /// Used to quickly identify what type of component an object is without casting.
    ComponentType componentType_{NONE};
};

/// Used to define what type of light a LightComponent is.
enum LightType
{
    POINT_LIGHT,
    DIRECTIONAL_LIGHT,
    SPOT_LIGHT
};

/// Contains info on a light source and how it affects a scene.
class LightComponent : public BaseComponent
{
public:
    LightComponent(): BaseComponent() { componentType_ = ComponentType::LIGHT; }

    /// Used to define type of light instead of having different inherited classes.
    LightType lightType_{POINT_LIGHT};
    /// Used to define what index the light is in the shader.
    /// Each light type has its own array, so it's the index within its light type.
    int lightIndexForShader_{0};

    /// Directional or Spotlight: direction the light is pointing.
    gsl::Vector3D direction_{0,-1,0};
    /// Directional of Spotlight: sets the light to point in the same direction as the entitys transform forward vectore.
    bool useEntityTransformForwardVectorAsDirection_{false};

    /// Spotlight: Angle (in radians) to stop cutting off light.
    float cutOff_{0.83f};
    /// Spotlight: Angle (in radians) to start cutting off light.
    float outerCutOff_{0.8f};

    /// Ambient color of the light in RGB (0-1 values).
    gsl::Vector3D ambient_{gsl::MAX_COLORS / 7};
    /// Diffuse color of the light in RGB (0-1 values).
    gsl::Vector3D diffuse_{gsl::MAX_COLORS / 4};
    /// Specular color of the light in RGB (0-1 values).
    gsl::Vector3D specular_{gsl::MAX_COLORS / 2};

    /// Pointlight: Constant Attenuation, the lower the number, the longer the distance.
    float constant_{1.0f};
    /// Pointlight: Linear Attenuation, the lower the number, the longer the distance.
    float linear_{0.09f};
    /// Pointlight: Quadratic Attenuation, the lower the number, the longer the distance.
    float quadratic_{0.032f};

    /**
     * reads component data from json object
     * @param json object to read from
     */
    void read(const QJsonObject& json);
    /**
     * writes component data to json object
     * @param json object to write to
     */
    void write(QJsonObject& json) const;
};

/// Used to define what type an object the mesh is.
/// Important in collision handling.
enum ObjectType
{
    DEFAULT,
    ENEMY,
    PLAYER,
    TROPHY,
    TAKEN_TROPHY
};

/// Contains info on how or if to render object and its collision.
class MeshComponent : public BaseComponent
{
public:
    MeshComponent(): BaseComponent() { componentType_ = ComponentType::MESH; }

    /// Type of object, used for collision handling.
    ObjectType objectType_{DEFAULT};
    /// The ID of mesh to be rendered.
    size_t meshID_{0};
    /// The ID of material used when rendering mesh.
    size_t materialID_{0};

    /// How many levels of detail the meshcomponent uses. should be in the mesh.
    unsigned int lodLevel_{0};
    /// How GL should render the object.
    GLenum mode_{GL_TRIANGLES};
    /// Whether object responds to collision or not.
    bool enableCollision_{true};
    /// Whether object responds to frustum culling or not.
    bool reactsToFrustumCulling_{true};

    /**
     * reads component data from json object.
     * @param json object to read from.
     */
    void read(const QJsonObject& json);
    /**
     * writes component data to json object.
     * @param json object to write to.
     */
    void write(QJsonObject& json) const;
};

/// States an AI can be in
enum AIStates
{
    AI_CHASE,
    AI_PATROL
};

/// Contains info on how an entity can move along a path from a bspline.
class AIComponent : public BaseComponent
{
public:
    AIComponent() : BaseComponent() { componentType_ = ComponentType::AI; }
    /// Path / bspline the entity moves along.
    std::shared_ptr<BSplineCurve> spline_{nullptr};
    /// Movement speed of the entity along the bspline.
    float speed_{0.0004f};
    /// Enity's current location on the bspline (0-1 value).
    float locationOnSpline_{0};
    /// The distance needed between the player and the AI before the AI changes its state to AI_CHASE.
    float distanceBeforeChase_ = 10.f;
    /// State the AI is currently in.
    AIStates aiState_{AI_PATROL};

    /**
     * reads component data from json object.
     * @param json object to read from.
     */
    void read(const QJsonObject& json);
    /**
     * writes component data to json object.
     * @param json object to write to.
     */
    void write(QJsonObject& json) const;
};

/// Contains info on one audio file and how to play it in scene.
class AudioComponent : public BaseComponent
{
public:
    AudioComponent(): BaseComponent() { componentType_ = ComponentType::AUDIO; }
    /// Contains source position.
    gsl::Vector3D position_;
    /// Contains source velocity.
    gsl::Vector3D velocity_;
    /// To define if the audio should loop.
    bool isLooping_{false};
    /// To define the volume of the audio, higher value = higher volume.
    float gain_{1.f};
    /// To define the ID of the source.
    ALuint source_{0};
    /// To define the buffer the source uses at this moment.
    ALuint currentBuffer_{0};
    /// To easily change the sound in the UI.
    ALuint soundID_{0};
    /// To define how much distance from object matters. the higher the number, the closer you have to be.
    ALfloat maxDistance_{5000};
    /// To define if the audio component has been initialized.
    bool initialized_{false};

    /**
     * reads component data from json object.
     * @param json object to read from.
     */
    void read(const QJsonObject& json);
    /**
     * writes component data to json object.
     * @param json object to write to.
     */
    void write(QJsonObject& json) const;

};

/// Contains info on entity position/scale/rotation in 3D space in scene.
class TransformComponent : BaseComponent
{
public:
    TransformComponent(): BaseComponent() { componentType_ = ComponentType::TRANSFORM; }
    /// Entity's model matrix.
    gsl::Matrix4x4 transform_;
    /// If transform has changed since last check, used to avoid unnecessary matrix operations.
    bool hasChanged_{true};
    bool orientRotationBasedOnMovement_{false};
    /// Whether entity should use barycentric coordinates to follow landscape surface Y-position.
    bool followLandscape_{false};

    /// Position relative to parent entity.
    gsl::Vector3D position_relative_{0,0,0};
    /// Rotation relative to parent entity.
    gsl::Vector3D rotation_relative_{0,0,0};
    /// Scale relative to parent entity.
    gsl::Vector3D scale_relative_{1,1,1};

    /// Position relative to world.
    gsl::Vector3D position_world_{0,0,0};
    /// Rotation relative to world.
    gsl::Vector3D rotation_world_{0,0,0};
    /// Scale relative to world.
    gsl::Vector3D scale_world_{1,1,1};

    /// Original value of position_world_, used when resetting transform.
    gsl::Vector3D position_default_{0,0,0};
    /// Original value of rotation_world_, used when resetting transform.
    gsl::Vector3D rotation_default_{0,0,0};
    /// Original value of scale_world_, used when resetting transform.
    gsl::Vector3D scale_default_{1,1,1};

    gsl::Vector3D newForwardVector_{0,0,0};

    /**
     * reads component data from json object.
     * @param json object to read from.
     */
    void read(const QJsonObject& json);
    /**
     * writes component data to json object.
     * @param json object to write to.
     */
    void write(QJsonObject& json) const;
};

#endif // COMPONENTS_H
