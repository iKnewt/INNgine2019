#include "basewidget.h"
#include "materialcreator.h"
#include "ui_materialcreator.h"
#include "lineboxproperty.h"
#include "spinboxproperty.h"
#include "comboboxproperty.h"
#include "Legacy/constants.h"

#include <QGraphicsPixmapItem>
#include <QGroupBox>
#include <QPainter>
#include <QPicture>
#include <QPushButton>
#include <QVBoxLayout>

#include <Managers/assetmanager.h>

MaterialCreator::MaterialCreator(bool edit, size_t id, QWidget *parent) :
    QDialog(parent),
    editingMaterial(edit),
    ID(id),
    ui(new Ui::MaterialCreator)
{

    ui->setupUi(this);
    InitializeWindow();
    if(editingMaterial)
        loadMaterialData();
    this->setWindowTitle("Material Creator - Alberto Engine");
}

void MaterialCreator::InitializeWindow()
{
    name_ = new LineBoxProperty("Material Name",1,this);

    QGroupBox* colorBox = new QGroupBox("Color",this);
    QHBoxLayout* colorBoxLayout = new QHBoxLayout;

    colorBox->setLayout(colorBoxLayout);
    color_ = new SpinBoxProperty("Color",3,0,gsl::MAX_COLORS,0,this);
    color_->SetSpinBoxValue(0,gsl::MAX_COLORS);
    color_->SetSpinBoxValue(1,gsl::MAX_COLORS);
    color_->SetSpinBoxValue(2,gsl::MAX_COLORS);
    connect(color_,&SpinBoxProperty::AnyValueChanged,this,&MaterialCreator::event_colorChanged);

    colorshower_ = QPixmap(100,100);
    colorshower_.fill(QColor(gsl::MAX_COLORS,gsl::MAX_COLORS,gsl::MAX_COLORS));

    colorShowerLabel_ = new QLabel(this);
    colorShowerLabel_->setPixmap(colorshower_);

    colorBoxLayout->addWidget(color_);
    colorBoxLayout->addWidget(colorShowerLabel_);

    QGroupBox* comboboxBox = new QGroupBox("Texture and Shader Selection",this);
    QVBoxLayout* comboboxBoxLayout = BaseWidget::CreateDefaultVLayout();
    shaderSelection_ = new ComboBoxProperty("Shader",1,this);
    textureSelection_ = new ComboBoxProperty("Texture",1,this);

    comboboxBoxLayout->addWidget(shaderSelection_);
    comboboxBoxLayout->addWidget(textureSelection_);
    comboboxBox->setLayout(comboboxBoxLayout);
    QGroupBox* otherBox = new QGroupBox("Other",this);
    QVBoxLayout* otherBoxLayout = BaseWidget::CreateDefaultVLayout();

    shininess_ = new SpinBoxProperty("Shininess",1,0,10000000,3,this);
    shininess_->SetSpinBoxValue(0,10);

    specularMapSelection_ = new ComboBoxProperty("Specular Map",1,this);


    otherBoxLayout->addWidget(shininess_);
    otherBoxLayout->addWidget(specularMapSelection_);
    otherBox->setLayout(otherBoxLayout);

    QString prefix;
    editingMaterial ? prefix = "Edit" : prefix = "Create";
    QPushButton* button = new QPushButton(prefix +" Material",this);
    connect(button,&QPushButton::clicked,this,&MaterialCreator::event_createMaterial);

    QVBoxLayout* masterLayout = BaseWidget::CreateDefaultVLayout();
    masterLayout->addWidget(name_);
    masterLayout->addWidget(colorBox);
    masterLayout->addWidget(comboboxBox);
    masterLayout->addWidget(otherBox);
    masterLayout->addWidget(button);
    ui->verticalLayout->addLayout(masterLayout);
    UpdateComboBoxes();
}

void MaterialCreator::UpdateComboBoxes()
{
    textureSelection_->ClearAllBoxes();
    for(unsigned int i = 0; i < AssetManager::GetInstance()->textureManager_->textures_.size(); i++)
    {
        QString name = AssetManager::GetInstance()->textureManager_->textures_[i]->name_;
        textureSelection_->AddBoxValue(0,name);
        specularMapSelection_->AddBoxValue(0,name);
    }
    shaderSelection_->ClearAllBoxes();
    for(unsigned int i = 0; i < AssetManager::GetInstance()->shaderManager_->shaders_.size(); i++)
    {
        QString name = AssetManager::GetInstance()->shaderManager_->shaders_[i]->name_;
        shaderSelection_->AddBoxValue(0,name);
    }
}

MaterialCreator::~MaterialCreator()
{
    delete ui;
}

void MaterialCreator::event_colorChanged()
{
    colorshower_.fill(QColor(
                          static_cast<int>(color_->GetSpinBoxValue(0)),
                          static_cast<int>(color_->GetSpinBoxValue(1)),
                          static_cast<int>(color_->GetSpinBoxValue(2))));
    //    colorshower_.fromImage(image);
    colorShowerLabel_->setPixmap(colorshower_);
    //    colorShowerLabel->update();
}

void MaterialCreator::event_createMaterial()
{
    if(name_->GetBoxText(0) == "")
        name_->SetBoxText(0,"Untitled Material");
    if(editingMaterial)
    {
        std::shared_ptr<Material> material = AssetManager::GetInstance()->GetMaterial(ID);
        material->name_ = name_->GetBoxText(0);
        material->color_ = BaseWidget::SpinBoxToVector3D(color_);
        material->shininess_ = shininess_->GetSpinBoxValue(0);
        material->shaderID_ = shaderSelection_->GetBoxIndexValue(0);
        material->textureID_ = textureSelection_->GetBoxIndexValue(0);
        material->specularMapID_ = specularMapSelection_->GetBoxIndexValue(0);
    }
    else
    {
        AssetManager::GetInstance()->materialManager_->AddMaterial(
                    name_->GetBoxText(0),
                    BaseWidget::SpinBoxToVector3D(color_),
                    shininess_->GetSpinBoxValue(0),
                    shaderSelection_->GetBoxIndexValue(0),
                    textureSelection_->GetBoxIndexValue(0),
                    specularMapSelection_->GetBoxIndexValue(0));
    }
    this->close();

}

void MaterialCreator::loadMaterialData()
{
    std::shared_ptr<Material> material = AssetManager::GetInstance()->GetMaterial(ID);
    name_->SetBoxText(0,material->name_);
    color_->SetSpinBoxValue(0,material->color_.x);
    color_->SetSpinBoxValue(1,material->color_.y);
    color_->SetSpinBoxValue(2,material->color_.z);
    shininess_->SetSpinBoxValue(0,material->shininess_);
    shaderSelection_->SetBoxCurrentIndex(0,static_cast<unsigned int>(material->shaderID_));
    textureSelection_->SetBoxCurrentIndex(0,static_cast<unsigned int>(material->textureID_));
    specularMapSelection_->SetBoxCurrentIndex(0,static_cast<unsigned int>(material->specularMapID_));

}
