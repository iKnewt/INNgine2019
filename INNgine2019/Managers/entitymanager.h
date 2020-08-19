#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <QTreeWidgetItem>

/// Keep all data and logic connected to entites.
/// This class hides communication between the engine and QTreeWidget.
class EntityManager
{
public:
    /**
     * EntityManager Constructor.
     * @param entityTree EntityManager uses a QTreeWidget as a base.
     */
    EntityManager(QTreeWidget* entityTree);
    /// Total number of entites.
    size_t numberOfEntities_{0};
    /// QTreeWidget to hold entites data.
    /// Used for parent/child relationships and to display in GUI.
    QTreeWidget* entityTree_;
    /// Pointers to all entites so they can be edited through the manager or in the GUI through the entityTree_.
    /// Entites contain a name and an ID.
    /// The ID is the same as their index in this QList.
    QList<QTreeWidgetItem*> entities_;
    /**
     * Adds new item to entites_ and updates numberOfEntities_.
     * Creates a custom QTreeWidgetItem based on the data we want an entity to have.
     * Automatically creates an entityID.
     * @param name Used to give the entity a name to be printed in GUI and debug.
     * @param parentEntityID Optional, entityID of the parent to add child to, if no parent given creates a top level item.
     */
    void AddEntity(QString name, size_t parentEntityID = gsl::INVALID_SIZE);
    /**
     * Selects an entity in the entityTree_ and deselcts all others.
     * Used to update GUI menus to display correct information based on selected entity.
     * @param entityID ID of entity to select.
     */
    void SelectEntityInTree(size_t entityID);
    /**
     * Sets an entity's check state.
     * @param entityID ID of entity to edit.
     * @param checked Whether to checked or unchecked.
     */
    void SetEntityWidgetItemChecked(size_t entityID, bool checked);
    /**
     * Gets the ID of an entity's parent if any.
     * @param entityID ID of entity to find parent of.
     * @return ID of parent, gsl::INVALID_SIZE if no parent is found.
     */
    size_t GetParentEntityID(size_t entityID) const;
    /**
     * Gets name of an entity.
     * @param entityID ID of entity to get name of.
     * @return Name of specified entity.
     */
    QString GetEntityName(size_t entityID) const;
    /**
     * Gets the IDs of an entity's children if any.
     * @param entityID ID of entity to find children of.
     * @return Vector of all children entity IDs of specified parent entity, empty vector if no children are found.
     */
    std::vector<size_t> GetChildrenIDs(size_t entityID) const;
    /**
     * Gets ID of the last entity in entities_.
     * Depends on the numberOfEntities_ always being correct.
     * @return ID of last entity.
     */
    size_t GetLastEntityID() const;
    /**
     * Deletes entity end its children from entityTree and entities_.
     * @param entityID ID of entity to delete.
     */
    void DeleteEntityWithChildren(size_t entityID);
    /**
     * Makes an entity into a child after initializatiion
     * @param parentID entityID of entity to be parent
     * @param childID entityID of enitity to be child
     */
    void SetChild(size_t parentID, size_t childID);
    /**
     * Deletes all entities
     */
    void DeleteAllEntities();

    /**
     * reads entity data from json object.
     * @param json object to read from.
     * @param entityID entityID to read data to.
     */
    void read(const QJsonObject& json, size_t entityID);
    /**
     * writes entity data to json object.
     * @param json object to write to.
     * @param entityID entityID to write data from.
     */
    void write(QJsonObject& json, size_t entityID) const;
};

#endif // ENTITYMANAGER_H
