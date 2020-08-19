#include "materialmanager.h"

MaterialManager::MaterialManager()
{
    qDebug() << "\n\nINITIALIZING MATERIAL MANAGER";
    // Materials that cannot be deleted
    AddMaterial("defaultPhongMaterial"); // 0
    materials_[0]->shaderID_ = PHONG_SHADER;
    materials_[0]->shininess_ = 10.f;
    materials_[0]->color_ = {169, 169, 169};

    AddMaterial("defaultTextureMaterial"); // 1
    materials_[1]->shaderID_ = TEXTURE_SHADER;
    materials_[1]->color_ = {169, 169, 169};

    AddMaterial("defaultNormalMaterial"); // 2
    materials_[2]->shaderID_ = PLAIN_SHADER;

    AddMaterial("LandscapeMaterial");
    materials_[3]->shaderID_ = PHONG_SHADER;
    materials_[3]->color_ = {99, 133, 49};
    materials_[3]->shininess_ = 2.0f;

    UpdateDefaults();
}

void MaterialManager::AddMaterial(QString name, gsl::Vector3D color, float shininess, size_t shaderID, size_t textureID, size_t specularMapID)
{
    materials_.push_back(std::make_shared<Material>(name, color, shininess, shaderID, textureID, specularMapID));
    qDebug() << "Material created:" << name;
}

void MaterialManager::SetShaderForAllMaterials(size_t shaderID)
{
    for(auto material : materials_)
    {
        material->shaderID_ = shaderID;
    }
}

void MaterialManager::SetTextureForAllMaterials(size_t textureID)
{
    for(auto material : materials_)
    {
        material->textureID_ = textureID;
    }
}

void MaterialManager::SetColorForAllMaterials(gsl::Vector3D color)
{
    for(auto material : materials_)
    {
        material->color_ = color;
    }
}

void MaterialManager::ResetToDefaults()
{
    for(auto material : materials_)
    {
        material->color_ = material->default_color_;
        material->shaderID_ = material->default_shaderID_;
        material->textureID_ = material->default_textureID_;
    }

}

void MaterialManager::UpdateDefaults()
{
    for(auto material : materials_)
    {
        material->default_color_ = material->color_;
        material->default_shaderID_ = material->shaderID_;
        material->default_textureID_ = material->textureID_;
    }
}

void MaterialManager::DeleteAllCustomMaterials()
{
    for (int i = materials_.size() - 1; i >= numberOfDefaultMaterials_; i--)
    {
        materials_.erase(materials_.begin() + i);
    }
}

void MaterialManager::DeleteMaterial(size_t materialID)
{
    materials_.erase(materials_.begin() + materialID);
}


