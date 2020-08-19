#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "Managers/components.h"

/// Keeps all the data and logic connected to components.
class ComponentManager
{
public:
    ComponentManager();

    /// Keeps track of the number of total entites.
    size_t numberOfEntities_{0};

    /// Vector of all TransformComponent, index defines what enitiy it belongs to, nullptr means no component.
    std::vector<std::shared_ptr<TransformComponent>> transformComponents_{nullptr};
    /// Vector of all MeshComponent, index defines what enitiy it belongs to, nullptr means no component.
    std::vector<std::shared_ptr<MeshComponent>> meshComponents_{nullptr};
    /// Vector of all AudioComponent, index defines what enitiy it belongs to, nullptr means no component.
    std::vector<std::shared_ptr<AudioComponent>> audioComponents_{nullptr};
    /// Vector of all LightComponent, index defines what enitiy it belongs to, nullptr means no component.
    std::vector<std::shared_ptr<LightComponent>> lightComponents_{nullptr};
    /// Vector of all AIComponent, index defines what enitiy it belongs to, nullptr means no component.
    std::vector<std::shared_ptr<AIComponent>> aiComponents_{nullptr};

    /**
     * Updates number of each light type in the phong shader.
     * This is needed to not count more lights than actually exist.
     */
    void UpdateShaderLightNumbers();
    /**
     * Resizes all the vectors containing components in the Component Manager.
     * Updates numberOfEntities_ as well as adds additional nullpointers to all component vectors.
     * This because all vectors contain as many elements as there are entities, a component's index defines what enitity it belongs to, nullptr means no component.
     * When iterating through all entites there needs to be at least a nullptr at the index to avoid crashes by trying to access invalid items in vector.
     * @param newSize Number used to resize all component vectors and numberOfEntities_.
     */
    void ResizeComponentVectors(size_t newSize);
    /**
     * Creates a new component of specified type and adds it to the corresponding vector.
     * @param componentType Defines what type of component is added and which component vector to add it to.
     * @param entityID Which index in the component vector to create the new component at, the index indicates the owning enitity.
     */
    void AddComponent(ComponentType componentType, size_t entityID);
    /**
     * Deletes all components within an entity.
     * @param entityID entityID to delete components from.
     */
    void DeleteAllComponentsForEntity(size_t entityID);
    /**
     * Deletes one component from an entity.
     * @param entityID entityID to delete component from.
     * @param componentType Component type to delete.
     */
    void DeleteComponentFromEntity(size_t entityID, ComponentType componentType);
    /**
     * Sets the default transforms of all TransformComponents to their current values.
     * Each TransformComponent contains a default data used when resetting it to a previous state.
     * Used for things such as loading or resetting scenes.
     */
    void UpdateDefaultTransforms();
    /**
     * Deletes all component data.
     */
    void DeleteAllComponents();

    void UpdateAudioID(unsigned int ID);
    void UnloadAudioSound(unsigned int ID);

    /**
     * reads component data from json object.
     * @param json object to read from.
     * @param entityID entityID to read data to.
     */
    void read(const QJsonObject& json, size_t entityID);
    /**
     * writes component data to json object.
     * @param json object to write to.
     * @param entityID entityID to write data from.
     */
    void write(QJsonObject& json, size_t entityID) const;
};

#endif // COMPONENTMANAGER_H
