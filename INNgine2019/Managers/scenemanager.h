#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Managers/componentmanager.h"
#include "Managers/entitymanager.h"
#include "Managers/assetmanager.h"
#include "Legacy/camera.h"
#include "script.h"
#include <QTime>
#include <QJsonDocument>

/// Saveformat when saving scene and assets.
enum SaveFormat {
    JSON,
    BINARY
};

/// Keeps all the data and logic connected to Scenes.
/// Most importantly entities and their components.
class SceneManager
{
public:
    SceneManager(QTreeWidget *entityTree);
    /// Manages the entity data, mainly connecting them to the GUI.
    std::shared_ptr<EntityManager> entityManager_;
    /// Manages the components.
    std::shared_ptr<ComponentManager> componentManager_;
    /// Defines which entity is selected and interactive, only one can be at a time.
    size_t activeEntityID_{0};
    /// Defines which entity is the player, can only be one at a time.
    size_t playerEntityID_{0};
    /// Script to load trophies from.
    std::shared_ptr<Script> TrophySpawnerScript_;
    /// Total amount of trophies in the scene.
    int trophiesCounter_{0};
    /// Times player has completed game.
    int roundsCleared_{0};
    /**
     * Adds entity to scene and creates room for components
     * @param entityName name of new entity.
     */
    void AddEntity(QString entityName = "new Entity");
    /**
     * Deletes entity from scene.
     * @param entityID ID of entity to delete.
     */
    void DeleteEntity(size_t entityID);
    /**
     * Deletes everything in the scene
     */
    void ResetScene();
    /**
     * Update AI b-spline paths based on the trophies in the scene.
     */
    void UpdateAIsBasedOnThropies();
    /**
     * Sets the current active entity to be player.
     */
    void SetActiveEntityToPlayer();
    /**
     * Sets the playerID.
     * @param EntityID entity ID to set as new player.
     */
    void SetPlayerID(size_t EntityID);
    /**
     * Reads entity, component and asset data from json object.
     * @param json object to read from.
     */
    void read(const QJsonObject& json);
    /**
     * Writes entity, component and asset data to json object.
     * @param json object to write to.
     */
    void write(QJsonObject& json) const;
    /// The last loaded or saved filepath.
    QString filePathOfCurrentScene{""};

    /**
     * Loads scene and asset data from file.
     * @param filePath filepath to load from.
     * @param saveFormat what fileformat the file is.
     * @return if loading was successful.
     */
    bool LoadScene(QString filePath = "", SaveFormat saveFormat = JSON);
    /**
     * Saves scene and asset data to file.
     * @param filePath path of file to save.
     * @param saveFormat What format to save in.
     * @return if saving was successful.
     */
    bool SaveScene(QString filePath = "", SaveFormat saveFormat = JSON);
    /**
     * Creates thopies based on data read from script.
     */
    void TrophiesFromScript();
};

#endif // SCENEMANAGER_H
