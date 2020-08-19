#include "assetmanager.h"

AssetManager* AssetManager::instance_ = nullptr;

AssetManager::AssetManager()
{
    qDebug() << "\n\nINITIALIZING ASSET MANAGER";
    shaderManager_ = std::make_shared<ShaderManager>();
    textureManager_ = std::make_shared<TextureManager>();
    materialManager_ = std::make_shared<MaterialManager>();
    meshManager_ = std::make_shared<MeshManager>();
    audioManager_ = std::make_shared<AudioManager>();
    updateLandscape(gsl::meshFilePath + "Alberto_Landscape.obj");
}

AssetManager* AssetManager::GetInstance()
{
    if(!instance_)
    {
        instance_ = new AssetManager();
    }
    return instance_;
}

std::shared_ptr<Material> AssetManager::GetMaterial(size_t materialID)
{
    if(materialManager_->materials_[materialID])
        return materialManager_->materials_[materialID];
    return nullptr;
}

std::shared_ptr<Texture> AssetManager::GetTexture(size_t textureID)
{
    //    if(textureManager_->textures_[textureID])
    if(textureManager_->textures_.size() > textureID)
        return textureManager_->textures_[textureID];
    return nullptr;
}

std::shared_ptr<Mesh> AssetManager::GetMesh(size_t meshID)
{
    if(meshManager_->meshes_[meshID])
        return meshManager_->meshes_[meshID];
    return nullptr;
}

void AssetManager::DeleteTexture(size_t textureID)
{
    for(auto material : materialManager_->materials_)
    {
        if(material->textureID_ == textureID)
            material->textureID_ = 0;
        else if(material->textureID_ > textureID)
            material->textureID_--;
        if(material->specularMapID_ == textureID)
            material->textureID_ = 0;
        else if(material->specularMapID_ > textureID)
            material->specularMapID_--;
    }
    textureManager_->DeleteTexture(textureID);
    materialManager_->UpdateDefaults();
}

void AssetManager::DeleteMesh(size_t meshID, std::vector<std::shared_ptr<MeshComponent> > meshComponents)
{
    for(auto meshComponent : meshComponents)
    {
        if(meshComponent->meshID_ == meshID)
            meshComponent->meshID_ = 0;
        else if(meshComponent->meshID_ > meshID)
            meshComponent->meshID_--;
    }
    meshManager_->DeleteMesh(meshID);
}

void AssetManager::DeleteMaterial(size_t materialID, std::vector<std::shared_ptr<MeshComponent> > meshComponents)
{
    for(auto meshComponent : meshComponents)
    {
        if(meshComponent->materialID_ == materialID)
            meshComponent->materialID_ = 0;
        else if(meshComponent->materialID_ > materialID)
            meshComponent->materialID_--;
    }
    materialManager_->DeleteMaterial(materialID);
}

void AssetManager::updateLandscape(QString fileWithPath)
{
    if(!landscape_)
        landscape_ = std::make_shared<Landscape>();
    std::pair <std::vector<Vertex>,std::vector<GLuint>> data;
    data = meshManager_->updateLandscapeMesh(fileWithPath);
    landscape_->vertices_ = data.first;
    landscape_->indices_ = data.second;
}

void AssetManager::read(const QJsonObject &json)
{
    // array of strings for each filepath
    qDebug() << "\n\nREADING TEXTURES FROM FILE";
    QJsonArray textures = json["textures"].toArray();
    textureManager_->DeleteAllCustomTextures();
    for (auto texture : textures)
    {
        textureManager_->AddTexture(gsl::textureFilePath + texture.toString());
    }

    qDebug() << "\n\nREADING MESHES FROM FILE";
    QJsonArray meshes = json["meshes"].toArray();
    meshManager_->DeleteAllCustomMeshes();
    for (auto mesh : meshes)
    {
        meshManager_->AddMesh(FILE_MESH, gsl::meshFilePath + mesh.toString());
    }

    qDebug() << "\n\nREADING AOUNDS FROM FILE";
    QJsonArray sounds = json["sounds"].toArray();
    audioManager_->DeleteAllCustomSounds();
    for (auto sound : sounds)
    {
        audioManager_->AddSound(gsl::soundFilePath + sound.toString());
    }

    qDebug() << "\n\nREADING MATERIALS FROM FILE";
    QJsonArray materials = json["materials"].toArray();
    materialManager_->DeleteAllCustomMaterials();
    for (auto material : materials)
    {
        QJsonObject materialObject = material.toObject();
        materialManager_->AddMaterial
                (
                    materialObject["name"].toString(),
                materialObject["color"].toArray(),
                static_cast<float>(materialObject["shininess"].toDouble()),
                static_cast<size_t>(materialObject["shaderID"].toInt()),
                static_cast<size_t>(materialObject["textureID"].toInt()),
                static_cast<size_t>(materialObject["specularMapID"].toInt())
                );
        materialManager_->UpdateDefaults();
    }
}

void AssetManager::write(QJsonObject &json) const
{
    QJsonArray textures;
    for(size_t i = textureManager_->numberOfDefaultTextures_; i < textureManager_->textures_.size(); i++)
    {
        textures.append(textureManager_->textures_[i]->name_);
    }
    json["textures"] = textures;

    QJsonArray meshes;
    for(size_t i = meshManager_->numberOfDefaultMeshes_; i < meshManager_->meshes_.size(); i++)
    {
        meshes.append(meshManager_->meshes_[i]->name_);
    }
    json["meshes"] = meshes;

    QJsonArray sounds;
    for(size_t i = audioManager_->numberOfDefaultSounds_; i < audioManager_->sounds_.size(); i++)
    {
        sounds.append(audioManager_->sounds_[i]->name_);
    }
    json["sounds"] = sounds;

    QJsonArray materialsArray;
    QJsonObject materialObject;
    for(size_t i = materialManager_->numberOfDefaultMaterials_; i < materialManager_->materials_.size(); i++)
    {
        materialObject["name"] =  materialManager_->materials_[i]->name_;
        materialObject["color"] = materialManager_->materials_[i]->color_.ToQJsonArray();
        materialObject["shininess"] = static_cast<double>( materialManager_->materials_[i]->shininess_);
        materialObject["shaderID"] = static_cast<int>( materialManager_->materials_[i]->shaderID_);
        materialObject["textureID"] = static_cast<int>( materialManager_->materials_[i]->textureID_);
        materialObject["specularMapID"] = static_cast<int>( materialManager_->materials_[i]->specularMapID_);

        materialsArray.append(materialObject);
    }

    json["materials"] = materialsArray;
}
