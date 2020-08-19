#include "entitymanager.h"


EntityManager::EntityManager(QTreeWidget* entityTree) : entityTree_(entityTree)
{
    qDebug() << "\n\nINITIALIZING ENTITY MANAGER";
}

QString EntityManager::GetEntityName(size_t entityID) const
{
    if (entityID >= numberOfEntities_)
        return "";
    return entities_[static_cast<int>(entityID)]->text(0);
}

size_t EntityManager::GetParentEntityID(size_t entityID) const
{
    if (entityID >= numberOfEntities_)
        return gsl::INVALID_SIZE;

    if (entities_[static_cast<int>(entityID)]->parent())
        return entities_[static_cast<int>(entityID)]->parent()->text(1).toUInt();
    else
        return gsl::INVALID_SIZE;
}

std::vector<size_t> EntityManager::GetChildrenIDs(size_t entityID) const
{
    std::vector<size_t> temp;

    if (entityID >= numberOfEntities_)
        return temp;

    for(int i = 0; i < entities_[static_cast<int>(entityID)]->childCount(); i++)
        temp.push_back(entities_[static_cast<int>(entityID)]->child(i)->text(1).toUInt());

    return temp;
}

size_t EntityManager::GetLastEntityID() const
{return numberOfEntities_ - 1;}

void EntityManager::DeleteEntityWithChildren(size_t entityID)
{
    if(entityID >= numberOfEntities_)
        return;

    if (entityTree_->indexOfTopLevelItem(entities_[entityID]) >= 0)
    {
        delete entityTree_->takeTopLevelItem(entityTree_->indexOfTopLevelItem(entities_[entityID]));
    }
    else
    {
        delete entities_[entityID]->parent()->takeChild(entities_[entityID]->parent()->indexOfChild(entities_[entityID]));
    }

    numberOfEntities_ --;
    entities_.erase(entities_.begin() + static_cast<int>(entityID));

    for(int i = 0; i < entities_.size(); i++)
    {
        entities_[i]->setText(1, QString::number(i));
    }

}

void EntityManager::SetChild(size_t parentID, size_t childID)
{
    if (entityTree_->indexOfTopLevelItem(entities_[childID]) >= 0)
    {
        entities_[parentID]->addChild(entityTree_->takeTopLevelItem(entityTree_->indexOfTopLevelItem(entities_[childID])));
    }
    else
    {
        entities_[parentID]->addChild(entities_[childID]);
    }
}

void EntityManager::DeleteAllEntities()
{
    entities_.clear();
    entityTree_->clear();
    numberOfEntities_ = 0;
}

void EntityManager::read(const QJsonObject &json, size_t entityID)
{
    AddEntity(json["entityName"].toString());
}

void EntityManager::write(QJsonObject &json, size_t entityID) const
{
    json["entityID"] = static_cast<int>(entityID);
    json["entityName"] = GetEntityName(entityID);
    json["parentID"] = static_cast<int>(GetParentEntityID(entityID));
    if(entities_[entityID]->checkState(0) == Qt::Checked)
        json["visible"] = true;
    else
        json["visible"] = false;
}

void EntityManager::AddEntity(QString name, size_t parentEntityID)
{   
    if (parentEntityID == gsl::INVALID_SIZE) // doesn't have a parent
    {
        entities_.push_back(new QTreeWidgetItem);
        entityTree_->addTopLevelItem(entities_.back());
    }
    else // does have a parent.
    {
        if (parentEntityID >= numberOfEntities_)
        {
            qDebug() << name << " has invalid parent ID: " << parentEntityID;
            return;
        }
        entities_.push_back(new QTreeWidgetItem(entities_[static_cast<int>(parentEntityID)]));
    }

    qDebug() << "Entity created" << name << "EntityID: " << numberOfEntities_;

    entities_.back()->setText(0, name);
    entities_.back()->setText(1, QString::number(numberOfEntities_));
    entities_.back()->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
    entities_.back()->setCheckState(0, Qt::Checked);
    numberOfEntities_++;
}

void EntityManager::SelectEntityInTree(size_t entityID)
{
    QTreeWidgetItemIterator it(entityTree_);
    while (*it)
    {
        if ((*it)->text(1) == QString::number(entityID))
            (*it)->setSelected(true);
        else
            (*it)->setSelected(false);
        ++it;
    }
}

void EntityManager::SetEntityWidgetItemChecked(size_t entityID, bool checked)
{
    if (entityID >= numberOfEntities_)
        return;

    if (checked)
        entities_[static_cast<int>(entityID)]->setCheckState(0, Qt::Checked);
    else
        entities_[static_cast<int>(entityID)]->setCheckState(0, Qt::Unchecked);
}



