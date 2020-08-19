#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "Managers/shadermanager.h"
#include "Managers/texturemanager.h"
#include "Managers/materialmanager.h"
#include "Managers/meshmanager.h"
#include "Managers/audiomanager.h"

///The terrain of the engine.
class Landscape
{
public:
    Landscape(){}
    /// ID of material used by landscape
    size_t materialID_{3};
    /**
     * The vertex data of tahe landscape.
     * Used for calculations like Barycentric Coordinates.
     */
    std::vector<Vertex> vertices_;
    /**
     * The indice data of the landscape.
     * Used for calculations like Barycentric Coordinates.
     */
    std::vector<GLuint> indices_;
};

/// Singleton class containing all assets to be used in engine.
class AssetManager
{
public:
    AssetManager();
    static AssetManager* GetInstance();

    /// Landscape to use in scene
    std::shared_ptr<Landscape> landscape_{nullptr};
    /**
     * Updates landscape mesh
     * @param fileWithPath filepath of new mesh.
     */
    void updateLandscape(QString fileWithPath);

    // Managers
    std::shared_ptr<ShaderManager> shaderManager_;
    std::shared_ptr<TextureManager> textureManager_;
    std::shared_ptr<MaterialManager> materialManager_;
    std::shared_ptr<MeshManager> meshManager_;
    std::shared_ptr<AudioManager> audioManager_;

    /**
     * Gets material.
     * @param materialID ID of material to get.
     * @return material with that ID.
     */
    std::shared_ptr<Material> GetMaterial(size_t materialID);
    /**
     * Gets texture.
     * @param textureID ID of texture to get.
     * @return texture with that ID.
     */
    std::shared_ptr<Texture> GetTexture(size_t textureID);
    /**
     * Gets mesh.
     * @param meshID ID of mesh to get.
     * @return mesh with that ID.
     */
    std::shared_ptr<Mesh> GetMesh(size_t meshID);

    // Deleters
    /**
     * Deletes texture, and corrects the texture IDs in materials accordingly
     * @param textureID ID of texture to delete
     */
    void DeleteTexture(size_t textureID);
    /**
     * Deletes mesh, and corrects all mesh components to reflect the change
     * @param meshID ID of mesh to delete
     * @param meshComponents Mesh components to update based on the deletion
     */
    void DeleteMesh(size_t meshID, std::vector<std::shared_ptr<MeshComponent>> meshComponents);
    /**
     * Deletes material, and corrects all mesh components to reflect the change
     * @param materialID ID of the material to delete
     * @param meshComponents Mesh components to update based on the deletion
     */
    void DeleteMaterial(size_t materialID, std::vector<std::shared_ptr<MeshComponent>> meshComponents);

    // Saving Loading
    /**
     * reads asset data from json object
     * @param json object to read from
     */
    void read(const QJsonObject& json);
    /**
     * writes asset data to jason object
     * @param json object to write to
     */
    void write(QJsonObject& json) const;

    // event
    /// list of entityIDs who have triggered events during gameplay
    /// temporarily placed in this class to have global access
    std::vector<unsigned int> events_;
    /**
     * adds event to events_
     * @param entityID ID of entity that triggered event
     */
    void AddEvent(unsigned int entityID) { events_.push_back(entityID); }
    /// Used to correct movement based on framerate
    /// temporarily placed in this class to have global access
    float deltaTime_{0};

private:
    static AssetManager* instance_;

};

#endif // ASSETMANAGER_H
