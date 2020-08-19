#include "assetmanagerwidget.h"

#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>

AssetManagerWidget::AssetManagerWidget(QWidget *parent, Qt::WindowFlags f) : BaseWidget(parent,f)
{
    InitializeWidget();
}

void AssetManagerWidget::InitializeWidget()
{
    //    this->setFixedHeight(170);
    QGroupBox* meshGroupBox = new QGroupBox("Meshes",this);
    QVBoxLayout* meshGroupBoxLayout = CreateDefaultVLayout();
    meshGroupBox->setLayout(meshGroupBoxLayout);
    meshes_ = new AssetViewer("Mesh",false,this);
    connect(meshes_,&AssetViewer::buttonPressed,this,&AssetManagerWidget::event_meshBoxButtonPressed);
    meshGroupBoxLayout->addWidget(meshes_);

    QGroupBox* soundGroupBox = new QGroupBox("Sounds",this);
    QVBoxLayout* soundGroupBoxLayout = CreateDefaultVLayout();
    soundGroupBox->setLayout(soundGroupBoxLayout);
    sounds_ = new AssetViewer("Sound",false,this);
    connect(sounds_,&AssetViewer::buttonPressed,this,&AssetManagerWidget::event_soundBoxButtonPressed);
    soundGroupBoxLayout->addWidget(sounds_);

    QGroupBox* materialGroupBox = new QGroupBox("Materials",this);
    QVBoxLayout* materialGroupBoxLayout = CreateDefaultVLayout();
    materialGroupBox->setLayout(materialGroupBoxLayout);
    materials_ = new AssetViewer("Material",true,this);
    connect(materials_,&AssetViewer::buttonPressed,this,&AssetManagerWidget::event_materialBoxButtonPressed);
    materialGroupBoxLayout->addWidget(materials_);

    QGroupBox* textureGroupBox = new QGroupBox("Textures",this);
    QVBoxLayout* textureGroupBoxLayout = CreateDefaultVLayout();
    textureGroupBox->setLayout(textureGroupBoxLayout);
    textures_ = new AssetViewer("Texture",false,this);
    connect(textures_,&AssetViewer::buttonPressed,this,&AssetManagerWidget::event_textureBoxButtonPressed);

    textureGroupBoxLayout->addWidget(textures_);

    QHBoxLayout* masterLayout = new QHBoxLayout;
    masterLayout->addWidget(meshGroupBox);
    masterLayout->addWidget(soundGroupBox);
    masterLayout->addWidget(materialGroupBox);
    masterLayout->addWidget(textureGroupBox);

    this->setLayout(masterLayout);
}

void AssetManagerWidget::UpdateValues()
{
    meshes_->ClearAllAssets();
    std::shared_ptr<MeshManager> meshManager_ = AssetManager::GetInstance()->meshManager_;
    for(size_t i = meshManager_->numberOfDefaultMeshes_; i < meshManager_->meshes_.size(); i++)
        meshes_->AddAsset(meshManager_->meshes_[i]->name_);

    sounds_->ClearAllAssets();
    std::shared_ptr<AudioManager> audioManager = AssetManager::GetInstance()->audioManager_;
    for(size_t i = audioManager->numberOfDefaultSounds_; i < audioManager->sounds_.size(); i++)
        sounds_->AddAsset(audioManager->sounds_[i]->name_);

    materials_->ClearAllAssets();
    std::shared_ptr<MaterialManager> materialManager_ = AssetManager::GetInstance()->materialManager_;
    for(size_t i = materialManager_->numberOfDefaultMaterials_; i < materialManager_->materials_.size(); i++)
        materials_->AddAsset(materialManager_->materials_[i]->name_);

    textures_->ClearAllAssets();
    std::shared_ptr<TextureManager> textureManager_ = AssetManager::GetInstance()->textureManager_;
    for(size_t i = textureManager_->numberOfDefaultTextures_; i < textureManager_->textures_.size(); i++)
        textures_->AddAsset(textureManager_->textures_[i]->name_);
}

void AssetManagerWidget::event_soundBoxButtonPressed(ButtonPress button)
{
    emit addOrRemoveObject(ASSET_SOUND,button);
}

void AssetManagerWidget::event_meshBoxButtonPressed(ButtonPress button)
{
    emit addOrRemoveObject(ASSET_MESH,button);
}

void AssetManagerWidget::event_textureBoxButtonPressed(ButtonPress button)
{
    emit addOrRemoveObject(ASSET_TEXTURE,button);
}

void AssetManagerWidget::event_materialBoxButtonPressed(ButtonPress button)
{
    emit addOrRemoveObject(ASSET_MATERIAL,button);
}

