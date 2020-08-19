#include "scenemanager.h"
#include "Systems/movementsystem.h"

SceneManager::SceneManager(QTreeWidget* entityTree)
{
    qDebug() << "\n\nINITIALIZING SCENE MANAGER";
    entityManager_ = std::make_shared<EntityManager>(entityTree);
    componentManager_ = std::make_shared<ComponentManager>();
    componentManager_->numberOfEntities_ = static_cast<size_t>(entityManager_->numberOfEntities_);
    componentManager_->ResizeComponentVectors(static_cast<size_t>(entityManager_->numberOfEntities_));
}

void SceneManager::TrophiesFromScript()
{
    TrophySpawnerScript_ = std::make_shared<Script>();

    if(TrophySpawnerScript_->GetDeleteOldTrophies())
    {
        for (size_t i = componentManager_->numberOfEntities_ - 1; i != gsl::INVALID_SIZE; i--)
            if(componentManager_->meshComponents_[i]->objectType_ == TROPHY
                    || componentManager_->meshComponents_[i]->objectType_ == TAKEN_TROPHY)
                DeleteEntity(i);
    }


    /// find values from script
    QString meshNameToFind = TrophySpawnerScript_->GetMeshName();
    size_t meshID{0};
    for(size_t i = 0; i < AssetManager::GetInstance()->meshManager_->meshes_.size(); i++)
    {
        if (AssetManager::GetInstance()->meshManager_->meshes_[i]->name_ == meshNameToFind)
        {
            meshID = i;
            break;
        }
    }

    QString materialNameToFind = TrophySpawnerScript_->GetMaterialName();
    size_t materialID{0};
    for(size_t i = 0; i < AssetManager::GetInstance()->materialManager_->materials_.size(); i++)
    {
        if (AssetManager::GetInstance()->materialManager_->materials_[i]->name_ == materialNameToFind)
        {
            materialID = i;
            break;
        }
    }

    QString soundNameToFind = TrophySpawnerScript_->GetSoundName();
    size_t soundID{0};
    for(size_t i = 0; i < AssetManager::GetInstance()->audioManager_->sounds_.size(); i++)
    {
        if (AssetManager::GetInstance()->audioManager_->sounds_[i]->name_ == soundNameToFind)
        {
            soundID = i;
            break;
        }
    }


    std::vector<gsl::Vector3D> points = TrophySpawnerScript_->GetTrophiesXZPositions();
    for (size_t i = 0; i < points.size(); i++)
    {
        entityManager_->AddEntity("ScriptTrophy" + QString::number(i));
        componentManager_->numberOfEntities_ = static_cast<size_t>(entityManager_->numberOfEntities_);
        componentManager_->ResizeComponentVectors(static_cast<size_t>(entityManager_->numberOfEntities_));
        size_t index = componentManager_->numberOfEntities_ - 1;

        componentManager_->AddComponent(ComponentType::MESH, index);
        componentManager_->meshComponents_[index]->meshID_ = static_cast<size_t>(meshID);
        componentManager_->meshComponents_[index]->materialID_ = static_cast<size_t>(materialID);

        componentManager_->meshComponents_[index]->objectType_ = TROPHY;

        componentManager_->AddComponent(ComponentType::TRANSFORM, index);
        gsl::Vector3D position = points[i];
        position.y = MovementSystem::FindLandscapeYOnLocation(AssetManager::GetInstance()->landscape_, position) + 1;
        componentManager_->transformComponents_[index]->position_relative_ = {position};
        componentManager_->transformComponents_[index]->rotation_relative_ = {0, static_cast<GLfloat>(gsl::RandomNumber(0, 180)), 0};

        componentManager_->AddComponent(ComponentType::AUDIO, index);
        componentManager_->audioComponents_[index]->soundID_ = static_cast<unsigned int>(soundID);
        componentManager_->audioComponents_[index]->maxDistance_ = 1;

        componentManager_->AddComponent(ComponentType::LIGHT, index);
    }
    componentManager_->UpdateDefaultTransforms();
    componentManager_->UpdateShaderLightNumbers();
    UpdateAIsBasedOnThropies();
}

void SceneManager::ResetScene()
{
    qDebug() << "RESETTING SCENE...";

    for (size_t i = 0; i < componentManager_->transformComponents_.size(); i++)
    {
        if (componentManager_->transformComponents_[i])
        {
            componentManager_->transformComponents_[i]->position_relative_ = (componentManager_->transformComponents_[i]->position_default_);
            componentManager_->transformComponents_[i]->rotation_relative_ = (componentManager_->transformComponents_[i]->rotation_default_);
            componentManager_->transformComponents_[i]->scale_relative_    = (componentManager_->transformComponents_[i]->scale_default_);
        }
        if(componentManager_->aiComponents_[i])
        {
            componentManager_->aiComponents_[i]->locationOnSpline_ = 0;
        }
        if(!componentManager_->meshComponents_[i])
            continue;
        if(componentManager_->meshComponents_[i]->objectType_ == TAKEN_TROPHY)
        {
            componentManager_->meshComponents_[i]->objectType_ = TROPHY;
            componentManager_->meshComponents_[i]->enableCollision_ = true;
        }

    }
    UpdateAIsBasedOnThropies();
}

void SceneManager::UpdateAIsBasedOnThropies()
{
    trophiesCounter_ = 0;
    std::vector<gsl::Vector3D> points;
    points.push_back({0});
    for(size_t i = 0; i < componentManager_->meshComponents_.size(); i++)
    {
        if(!componentManager_->meshComponents_[i])
            continue;
        if(componentManager_->meshComponents_[i]->objectType_ == TROPHY)
        {
            if(componentManager_->transformComponents_[i])
            {
                points.push_back(componentManager_->transformComponents_[i]->position_world_);
                trophiesCounter_++;
            }
        }
    }
    points.push_back({0});
    for(size_t i = 0; i < componentManager_->meshComponents_.size(); i++)
    {
        if(componentManager_->aiComponents_[i] && componentManager_->transformComponents_[i])
        {
            if(!componentManager_->aiComponents_[i]->spline_)
                componentManager_->aiComponents_[i]->spline_ = std::make_shared<BSplineCurve>();

            points.front() = componentManager_->transformComponents_[i]->position_world_;
            points.back() = componentManager_->transformComponents_[i]->position_default_;

            componentManager_->aiComponents_[i]->spline_->OverrideControlpoints(points);
            componentManager_->aiComponents_[i]->spline_->RandomizeControlpointOrder();
            componentManager_->aiComponents_[i]->locationOnSpline_ = 0;

        }
    }
}

void SceneManager::SetActiveEntityToPlayer()
{
    for(unsigned int i = 0; i < componentManager_->meshComponents_.size(); i++)
    {
        if(!componentManager_->meshComponents_[i])
            continue;
        if(i == activeEntityID_)
        {
            if(componentManager_->meshComponents_[i]->objectType_ != ObjectType::PLAYER)
                componentManager_->meshComponents_[i]->objectType_ = ObjectType::PLAYER;
            continue;
        }
        if(componentManager_->meshComponents_[i]->objectType_ == ObjectType::PLAYER)
            componentManager_->meshComponents_[i]->objectType_ = ObjectType::DEFAULT;
    }
    playerEntityID_ = activeEntityID_;
}

void SceneManager::SetPlayerID(size_t EntityID)
{
    if(EntityID >= entityManager_->numberOfEntities_)
        return;

    for(unsigned int i = 0; i < componentManager_->meshComponents_.size(); i++)
    {
        if(i == EntityID)
        {
            componentManager_->meshComponents_[i]->objectType_ = ObjectType::PLAYER;
            continue;
        }
        if(componentManager_->meshComponents_[i]->objectType_ == ObjectType::PLAYER)
            componentManager_->meshComponents_[i]->objectType_ = ObjectType::DEFAULT;
    }
    playerEntityID_ = EntityID;
}

void SceneManager::read(const QJsonObject &json)
{

    if(json.contains("assets"))
    {
        QJsonObject assets = json["assets"].toObject();

        AssetManager::GetInstance()->read(assets);
    }
    if(json.contains("entites"))
    {

        QJsonArray entities = json["entites"].toArray();
        entityManager_->DeleteAllEntities();
        componentManager_->DeleteAllComponents();

        std::vector<std::vector<size_t>> entityParentsAndChildren;
        entityParentsAndChildren.resize(static_cast<size_t>(entities.size()));

        for (size_t entityID = 0; entityID < static_cast<size_t>(entities.size()); entityID++)
        {
            QJsonObject entity = entities[static_cast<int>(entityID)].toObject();

            entityManager_->read(entity["1_ENTITY_INFO"].toObject(), entityID);
            componentManager_->read(entity["2_COMPONENTS"].toObject(), entityID);

            size_t parentID = static_cast<size_t>(entity["1_ENTITY_INFO"].toObject()["parentID"].toInt());
            if (parentID != gsl::INVALID_SIZE)
                entityParentsAndChildren[parentID].push_back(entityID);
            if (!entity["1_ENTITY_INFO"].toObject()["visible"].toBool())
                entityManager_->SetEntityWidgetItemChecked(entityID, false);
        }

        // connects the child/parent relationships of entities
        // doing this after all entities have been initialized to make sure all parents are initialized when being assigned children
        for (size_t parentID = 0; parentID < entityParentsAndChildren.size(); parentID++)
        {
            for (size_t childID = 0; childID < entityParentsAndChildren[parentID].size(); childID++)
            {
                qDebug() << "parent: " << parentID << " child: " << entityParentsAndChildren[parentID][childID];
                entityManager_->SetChild(parentID, entityParentsAndChildren[parentID][childID]);
            }
        }
    }
}

void SceneManager::write(QJsonObject &json) const
{
    QJsonArray entities;
    for (size_t entityID = 0; entityID < entityManager_->numberOfEntities_; entityID++)
    {
        QJsonObject entity;

        QJsonObject entityInfo;
        entityManager_->write(entityInfo, entityID);
        entity["1_ENTITY_INFO"] = entityInfo;

        QJsonObject components;
        componentManager_->write(components, entityID);
        entity["2_COMPONENTS"] = components;

        entities.append(entity);
    }

    json["entites"] = entities;

    QJsonObject assets;
    AssetManager::GetInstance()->write(assets);
    json["assets"] = assets;

}

bool SceneManager::LoadScene(QString filePath, SaveFormat saveFormat)
{
    if(filePath == "")
        filePath = QString(gsl::scriptFilePath + "autoSave.json");

    QFile loadFile(filePath);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(saveFormat == JSON
                          ? QJsonDocument::fromJson(saveData)
                          : QJsonDocument::fromBinaryData(saveData));

    read(loadDoc.object());

    qDebug() << "Loaded scene " << filePath;
    return true;
}

bool SceneManager::SaveScene(QString filePath, SaveFormat saveFormat)
{
    if(filePath == "")
        filePath = QString(gsl::scriptFilePath + "autoSave.json");

    QFile saveFile(filePath);

    if (!saveFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonObject gameObject;
    write(gameObject);
    QJsonDocument saveDoc(gameObject);
    saveFile.write(saveFormat == JSON
                   ? saveDoc.toJson()
                   : saveDoc.toBinaryData());
    filePathOfCurrentScene = filePath;
    qDebug() << "Scene saved to " << filePath;
    return true;
}

void SceneManager::AddEntity(QString entityName)
{
    entityManager_->AddEntity(entityName);
    componentManager_->ResizeComponentVectors(entityManager_->numberOfEntities_);
}

void SceneManager::DeleteEntity(size_t entityID)
{
    for(auto childID : entityManager_->GetChildrenIDs(entityID))
    {
        DeleteEntity(childID);
    }

    if(entityManager_->GetChildrenIDs(entityID).size() == 0)
    {
        qDebug() << "Deleting Entity " << entityID;
        entityManager_->DeleteEntityWithChildren(entityID);
        componentManager_->DeleteAllComponentsForEntity(entityID);
    }
}
