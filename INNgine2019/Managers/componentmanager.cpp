#include "assetmanager.h"
#include "componentmanager.h"
#include "Managers/entitymanager.h"

ComponentManager::ComponentManager()
{
    qDebug() << "\n\nINITIALIZING COMPONENT MANAGER";
}

void ComponentManager::UpdateShaderLightNumbers()
{
    int numberOfPointLights_{0};
    int numberOfSpotLights_{0};
    int numberOfDirectionalLights_{0};
    for(auto lightComponent : lightComponents_)
    {
        if(lightComponent)
        {
            switch (lightComponent->lightType_)
            {
            case POINT_LIGHT:
                lightComponent->lightIndexForShader_ = numberOfPointLights_;
                numberOfPointLights_++;
                break;
            case SPOT_LIGHT:
                lightComponent->lightIndexForShader_ = numberOfSpotLights_;
                numberOfSpotLights_++;
                break;
            case DIRECTIONAL_LIGHT:
                lightComponent->lightIndexForShader_ = numberOfDirectionalLights_;
                numberOfDirectionalLights_++;
                break;
            }
        }
    }
    AssetManager::GetInstance()->shaderManager_->TransmitUniformLightDataToShader(PHONG_SHADER, numberOfPointLights_, numberOfSpotLights_, numberOfDirectionalLights_);
    qDebug() << "numberOfPointLights_" << numberOfPointLights_ << "numberOfSpotLights_" << numberOfSpotLights_<< "numberOfDirectionalLights_" << numberOfDirectionalLights_;
}

void ComponentManager::ResizeComponentVectors(size_t newSize)
{
    numberOfEntities_ = newSize;
    transformComponents_.resize(newSize,nullptr);
    meshComponents_.resize(newSize,nullptr);
    audioComponents_.resize(newSize,nullptr);
    lightComponents_.resize(newSize,nullptr);
    aiComponents_.resize(newSize,nullptr);
}

void ComponentManager::AddComponent(ComponentType componentType, size_t entityID)
{
    if (entityID >= numberOfEntities_)
        ResizeComponentVectors(entityID + 1);

    switch (componentType) {
    case TRANSFORM:
        if(!transformComponents_[entityID])
            transformComponents_[entityID] = std::make_shared<TransformComponent>();
        break;
    case MESH:
        if(!meshComponents_[entityID])
            meshComponents_[entityID] = std::make_shared<MeshComponent>();
        break;
    case AUDIO:
        if(!audioComponents_[entityID])
            audioComponents_[entityID] = std::make_shared<AudioComponent>();
        break;
    case LIGHT:
    {
        if(!lightComponents_[entityID])
            lightComponents_[entityID] = std::make_shared<LightComponent>();
        UpdateShaderLightNumbers();
        break;
    }
    case AI:
        if(!aiComponents_[entityID])
            aiComponents_[entityID] = std::make_shared<AIComponent>();
        break;
    case NONE:
        break;
    }

    qDebug() << COMPONENT_TYPE_NAMES[componentType] << "created - EntityID: " <<  entityID;
}

void ComponentManager::DeleteAllComponentsForEntity(size_t entityID)
{
    if (entityID >= numberOfEntities_)
        return;
    numberOfEntities_ --;

    transformComponents_.erase(transformComponents_.begin() + static_cast<int>(entityID));
    meshComponents_.erase(meshComponents_.begin() + static_cast<int>(entityID));
    audioComponents_.erase(audioComponents_.begin() + static_cast<int>(entityID));
    lightComponents_.erase(lightComponents_.begin() + static_cast<int>(entityID));
    aiComponents_.erase(aiComponents_.begin() + static_cast<int>(entityID));

    UpdateShaderLightNumbers();
}

void ComponentManager::DeleteComponentFromEntity(size_t entityID, ComponentType componentType)
{
    if (entityID >= numberOfEntities_)
        return;

    switch (componentType) {
    case TRANSFORM:
        transformComponents_[entityID] = nullptr;
        break;
    case MESH:
        meshComponents_[entityID] = nullptr;
        break;
    case AUDIO:
        audioComponents_[entityID] = nullptr;
        break;
    case LIGHT:
        lightComponents_[entityID] = nullptr;
        UpdateShaderLightNumbers();
        break;
    case AI:
        aiComponents_[entityID] = nullptr;
        break;
    case NONE:
        break;
    }

    qDebug() << "Removed " << COMPONENT_TYPE_NAMES[componentType] << " From EntityID: " <<  entityID;
}

void ComponentManager::UpdateDefaultTransforms()
{
    for (size_t i = 0; i < transformComponents_.size(); i++)
    {
        if(!transformComponents_[i])
            continue;

        transformComponents_[i]->position_default_ = transformComponents_[i]->position_relative_;
        transformComponents_[i]->rotation_default_ = transformComponents_[i]->rotation_relative_;
        transformComponents_[i]->scale_default_ = transformComponents_[i]->scale_relative_;
    }

}

void ComponentManager::DeleteAllComponents()
{
    ResizeComponentVectors(0);
    UpdateShaderLightNumbers();
}

void ComponentManager::UpdateAudioID(unsigned int ID)
{
    for(auto audioComp : audioComponents_)
    {
        if(!audioComp)
            continue;
        if(audioComp->soundID_ < ID)
            continue;
        if(audioComp->soundID_ == ID)
            audioComp->soundID_ = 0;
        else if(audioComp->soundID_ > ID)
            audioComp->soundID_--;
        AssetManager::GetInstance()->audioManager_->SetSound(audioComp->soundID_,audioComp);
    }
}

void ComponentManager::UnloadAudioSound(unsigned int ID)
{
    for(auto audioComp : audioComponents_)
    {
        if(!audioComp)
            continue;
        if(ID == audioComp->soundID_)
        {
            alcMakeContextCurrent(AssetManager::GetInstance()->audioManager_->ALcontext_);
            alSourcei(audioComp->source_, AL_BUFFER, NULL);
        }

    }
}

void ComponentManager::read(const QJsonObject &json, size_t entityID)
{
    ResizeComponentVectors(entityID + 1);

    if(json.contains(COMPONENT_TYPE_NAMES[TRANSFORM]))
    {
        TransformComponent transformComponent;
        transformComponent.read(json[COMPONENT_TYPE_NAMES[TRANSFORM]].toObject());
        if(!transformComponents_[entityID])
            AddComponent(TRANSFORM, entityID);
        *transformComponents_[entityID] = transformComponent;
    }
    if(json.contains(COMPONENT_TYPE_NAMES[MESH]))
    {
        MeshComponent meshComponent;
        meshComponent.read(json[COMPONENT_TYPE_NAMES[MESH]].toObject());
        if(!meshComponents_[entityID])
            AddComponent(MESH, entityID);
        *meshComponents_[entityID] = meshComponent;
    }
    if(json.contains(COMPONENT_TYPE_NAMES[AUDIO]))
    {
        AudioComponent audioComponent;
        audioComponent.read(json[COMPONENT_TYPE_NAMES[AUDIO]].toObject());
        if(!audioComponents_[entityID])
            AddComponent(AUDIO, entityID);
        *audioComponents_[entityID] = audioComponent;
    }
    if(json.contains(COMPONENT_TYPE_NAMES[LIGHT]))
    {
        LightComponent lightComponent;
        lightComponent.read(json[COMPONENT_TYPE_NAMES[LIGHT]].toObject());
        if(!lightComponents_[entityID])
            AddComponent(LIGHT, entityID);
        *lightComponents_[entityID] = lightComponent;
        UpdateShaderLightNumbers();
    }
    if(json.contains(COMPONENT_TYPE_NAMES[AI]))
    {
        AIComponent aiComponent;
        aiComponent.read(json[COMPONENT_TYPE_NAMES[AI]].toObject());
        if(!aiComponents_[entityID])
            AddComponent(AI, entityID);
        *aiComponents_[entityID] = aiComponent;
    }
}

void ComponentManager::write(QJsonObject &json, size_t entityID) const
{
    if (entityID >= numberOfEntities_)
        return;

    if(transformComponents_[entityID])
    {
        QJsonObject componentObject;
        transformComponents_[entityID]->write(componentObject);
        json[COMPONENT_TYPE_NAMES[TRANSFORM]] = componentObject;
    }
    if(meshComponents_[entityID])
    {
        QJsonObject componentObject;
        meshComponents_[entityID]->write(componentObject);
        json[COMPONENT_TYPE_NAMES[MESH]] = componentObject;
    }
    if(audioComponents_[entityID])
    {
        QJsonObject componentObject;
        audioComponents_[entityID]->write(componentObject);
        json[COMPONENT_TYPE_NAMES[AUDIO]] = componentObject;
    }
    if(lightComponents_[entityID])
    {
        QJsonObject componentObject;
        lightComponents_[entityID]->write(componentObject);
        json[COMPONENT_TYPE_NAMES[LIGHT]] = componentObject;
    }
    if(aiComponents_[entityID])
    {
        QJsonObject componentObject;
        aiComponents_[entityID]->write(componentObject);
        json[COMPONENT_TYPE_NAMES[AI]] = componentObject;
    }
}
