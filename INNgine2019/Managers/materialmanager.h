#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "Managers/shadermanager.h"
#include "Managers/texturemanager.h"

/// Contains material data.
struct Material
{
    Material(QString name = "New Material", gsl::Vector3D color = {gsl::MAX_COLORS}, float shininess = 10, size_t shaderID = 0, size_t textureID = 1, size_t specularMapID = 1)
        : name_(name), color_(color), shininess_(shininess), shaderID_(shaderID), textureID_(textureID), specularMapID_(specularMapID) {}
    /// Material name used to refer to this material in editor or debugging.
    QString name_;
    /// The color of the material in RGB value (0-1), is added on top of texture if any.
    gsl::Vector3D color_{gsl::MAX_COLORS};
    /// Shininess of material, affects the way it reflects light, higher values means less scattering.
    float shininess_{10};
    /// ID of Shader used by material.
    size_t shaderID_{0};
    /// ID of Texture used by material.
    size_t textureID_{1};
    /// ID of Specular map used by material, defines shininess of fragment based on texture value.
    size_t specularMapID_{1};

    /// Default color to revert to after temporary.
    gsl::Vector3D default_color_{gsl::MAX_COLORS};
    /// Default shaderID to revert to after temporary.
    size_t default_shaderID_{1};
    /// Default textureID to revert to after temporary.
    size_t default_textureID_{1};
};

/// Keeps all the data and logic connected to Materials.
class MaterialManager
{
public:
    /**
     * Constructor that also generates some basic materials.
     */
    MaterialManager();

    /// Number of defaults, used to make sure default are not deleted or edited.
    const size_t numberOfDefaultMaterials_{4};
    /// Vector containing all materials.
    std::vector<std::shared_ptr<Material>> materials_;
    /**
     * Creates new material and adds it to materials_ vector.
     * @param name Name of the material to be created.
     */
    void AddMaterial(QString name = "New Material", gsl::Vector3D color = {gsl::MAX_COLORS}, float shininess = 10, size_t shaderID = 0, size_t textureID = 1, size_t specularMapID = 1);
    /**
     * Sets one shader for all materials.
     * @param shaderID ShaderID to use.
     */
    void SetShaderForAllMaterials(size_t shaderID);
    /**
     * Sets one texture for all materials.
     * @param textureID ID of texture to use.
     */
    void SetTextureForAllMaterials(size_t textureID);
    /**
     * Sets one color for all materials.
     * @param color Color to use.
     */
    void SetColorForAllMaterials(gsl::Vector3D color);
    /**
     * Updates the current material to its default values.
     */
    void ResetToDefaults();
    /**
     * Updates the material default values to be the current values.
     */
    void UpdateDefaults();
    /**
     * Deletes all added materials.
     */
    void DeleteAllCustomMaterials();
    /**
     * Deletes a material.
     * @param materialID ID of material to delete.
     */
    void DeleteMaterial(size_t materialID);
};

#endif // MATERIALMANAGER_H
