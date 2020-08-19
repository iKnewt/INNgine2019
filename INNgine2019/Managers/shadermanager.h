#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <vector>
#include "Legacy/camera.h"
#include "Legacy/innpch.h"

/// Used to define what type a shader is quickly.
/// Especially when deciding what data to transfer to the shader when rendering.
enum ShaderType
{
    PHONG_SHADER,
    TEXTURE_SHADER,
    PLAIN_SHADER,
    MONO_COLOR_SHADER
};

/// Contains directinal light uniform locations.
/// Used to communicate with shaders.
struct DirLightUniform
{
    GLint direction{-1};

    GLint ambient{-1};
    GLint diffuse{-1};
    GLint specular{-1};
};

/// Contains spot light uniform locations.
/// Used to communicate with shaders.
struct SpotLightUniform
{
    GLint position{-1};
    GLint direction{-1};
    GLint cutOff{-1};
    GLint outerCutOff{-1};

    GLint ambient{-1};
    GLint diffuse{-1};
    GLint specular{-1};
};

/// Contains point light uniform locations.
/// Used to communicate with shaders.
struct PointLightUniform
{
    GLint position{-1};
    GLint constant{-1};
    GLint linear{-1};
    GLint quadratic{-1};

    GLint ambient{-1};
    GLint diffuse{-1};
    GLint specular{-1};
};

/// Contains material uniform locations.
/// Used to communicate with shaders.
struct MaterialUniform
{
    GLint diffuse{-1};
    GLint specular{-1};

    GLint color{-1};
    GLint shininess{-1};
};

/// Contains uniform locations and data connected to shaders.
struct Shader
{
    Shader(QString name, ShaderType type) : name_(name), type_(type){}

    /// Name of shader, used to identify shader in GUI and debug.
    QString name_;
    /// Type of shader, used to quickly identify the shader type to transmit correct data.
    ShaderType type_;

    /// program Id used with glUseProgram when updating uniforms in shader.
    GLuint program_{0};

    /// Model Matrix uniform location.
    GLint mMatrixUniform_{-1};
    /// View Matrix uniform location.
    GLint vMatrixUniform_{-1};
    /// Projection Matrix uniform location.
    GLint pMatrixUniform_{-1};

    /// Color or Texture Shader: object color uniform location.
    GLint objectColorUniform_{-1};
    /// Texture Shader: texture uniform location.
    GLint textureUniform_{-1};

    /// Model Matrix uniform location.
    GLint numberOfPointLights_{-1};
    /// View Matrix uniform location.
    GLint numberOfSpotLights_{-1};
    /// Projection Matrix uniform location.
    GLint numberOfDirectionalLights_{-1};

    /// Phong Shader: directional light uniform location.
    DirLightUniform dirLight_[gsl::MAX_NUMBER_OF_LIGHTS];
    /// Phong Shader: spot light uniform location.
    SpotLightUniform spotLight_[gsl::MAX_NUMBER_OF_LIGHTS];
    /// Phong Shader: point light uniform location.
    PointLightUniform pointLight_[gsl::MAX_NUMBER_OF_LIGHTS];
    /// Phong Shader: material uniform location.
    MaterialUniform material_;
    /// Phong Shader: Active camera to access camera's position
    //TODO move out of shader
    std::shared_ptr<Camera> currentCamera_{nullptr};
    /// Phong Shader: Viewer position uniform location.
    GLint viewPosUniform_{-1}; 

};

/// Keeps all the data and logic connected to shaders.
class ShaderManager : public QOpenGLFunctions_4_1_Core
{
public:
    ShaderManager();
    /// Vector containing all shaders.
    std::vector<std::shared_ptr<Shader>> shaders_;

public:
    /**
     * Sets active camera for all shaders.
     * @param camera Camera to be set as active camera in all shaders.
     */
    void SetActiveCamera(std::shared_ptr<Camera> camera);
    /**
     * Adds a shader of the specified type to shaders_.
     * This is hard coded for preset shaders and does not support custom shaders yet.
     * @param shaderType Shader type to be added.
     */
    void AddShader(ShaderType shaderType);
    /**
     * Transmits uniform data to shader, used when no material is available.
     * @param shaderID What shader to transmit data to.
     * @param modelMatrix Object's model matrix.
     * @param color Color to use for the object.
     */
    void TransmitUniformDataToShader(size_t shaderID, gsl::Matrix4x4 *modelMatrix, gsl::Vector3D color = {0.5,0.5,0.5});
    /**
     * Transmits uniform data to shader.
     * @param material Material to get the transmission data from.
     * @param modelMatrix Object's model matrix.
     */
    void TransmitUniformDataToShader(std::shared_ptr<Material> material, gsl::Matrix4x4* modelMatrix);
    /**
     * Loads shader from .vert and .frag files using preset shaderFilePath and shaderName.
     * Hardcoded so that the .vert and .frag must share filename.
     * This is because this project's doesnt current need custom shaders.
     * @param shaderName Name of shader, used in debugging and equals the filenames of the shaders.
     * @param geometryPath Used for geometry shader, not fully supported yet.
     */
    void LoadShadersFromFiles(std::string shaderName, const GLchar* geometryPath = nullptr);
    /**
     * Transmits uniform data from light source to shader.
     * Used for phong shader.
     * @param shaderID ID of shader to transmit data to.
     * @param position Position of the light source.
     * @param light The lightsource to transmit data from.
     */
    void TransmitUniformLightDataToShader(size_t shaderID, gsl::Vector3D position, std::shared_ptr<LightComponent> light);
    /**
     * Transmits uniform data about number of lights per type to shader.
     * @param shaderID ID of shader to transmit data to.
     * @param numberOfPointLights number of points lights in the current scene.
     * @param numberOfSpotLights number of spot lights in the current scene.
     * @param numberOfDirectionalLights number of directional lights in the current scene.
     */
    void TransmitUniformLightDataToShader(size_t shaderID, int numberOfPointLights, int numberOfSpotLights, int numberOfDirectionalLights);
};

#endif // SHADERMANAGER_H
