#include "comboboxproperty.h"
#include "meshwidget.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>

#include <Managers/assetmanager.h>

MeshWidget::MeshWidget(QWidget *parent, Qt::WindowFlags f)  : BaseWidget(parent,f)
{
    InitializeWidget();
}

void MeshWidget::InitializeWidget()
{
    QGroupBox* selection =  new QGroupBox("Selection",this);
    QVBoxLayout* selectionLayout = CreateDefaultVLayout();
    selectMesh_ = new ComboBoxProperty("Select Mesh",1,this);
    selectMesh_->SetComboBoxFixedWidth(150);
    connect(selectMesh_,&ComboBoxProperty::AnyValueChanged,this,&MeshWidget::event_anyPropertyChanged);
    selectionLayout->addWidget(selectMesh_);

    selectMaterial_ = new ComboBoxProperty("Select Material",1,this);
    connect(selectMaterial_,&ComboBoxProperty::AnyValueChanged,this,&MeshWidget::event_anyPropertyChanged);
    selectMaterial_->SetComboBoxFixedWidth(150);
    selectionLayout->addWidget(selectMaterial_);

    selectGLDraw_ = new ComboBoxProperty("Select GL Draw Method",1,this);
    selectGLDraw_->SetComboBoxFixedWidth(150);
    connect(selectGLDraw_,&ComboBoxProperty::AnyValueChanged,this,&MeshWidget::event_anyPropertyChanged);
    selectionLayout->addWidget(selectGLDraw_);
    CreateGLSelection();

    selection->setLayout(selectionLayout);

    QGroupBox* other = new QGroupBox("Other",this);
    QVBoxLayout* otherLayout = CreateDefaultVLayout();

    selectObjectType = new ComboBoxProperty("Select Object Type",1,this);
    connect(selectObjectType,&ComboBoxProperty::AnyValueChanged,this,&MeshWidget::event_anyPropertyChanged);
    selectObjectType->AddBoxValue(0,"Default");
    selectObjectType->AddBoxValue(0,"Enemy");
    selectObjectType->AddBoxValue(0,"Player");
    selectObjectType->AddBoxValue(0,"Trophy");
    otherLayout->addWidget(selectObjectType);

    collisionEnabled_ = new QCheckBox("Collision Enabled",this);
    collisionEnabled_->setCheckable(true);
    connect(collisionEnabled_,&QCheckBox::clicked,this,&MeshWidget::event_anyPropertyChanged);
    otherLayout->addWidget(collisionEnabled_);

    other->setLayout(otherLayout);

    QVBoxLayout* finalLayout = CreateDefaultVLayout();
    finalLayout->addWidget(selection);
    finalLayout->addWidget(other);
    this->setLayout(finalLayout);
}

void MeshWidget::UpdateMeshWidget(std::shared_ptr<MeshComponent> mesh)
{
    UpdateComboBoxValues(mesh);
    collisionEnabled_->setChecked(mesh->enableCollision_);
    SetGLTypeInWidget(mesh);
    selectObjectType->SetBoxCurrentIndex(0,mesh->objectType_);
}

void MeshWidget::UpdateMeshComponent(std::shared_ptr<MeshComponent> mesh)
{
    mesh->meshID_ = selectMesh_->GetBoxIndexValue(0);
    mesh->materialID_ = selectMaterial_->GetBoxIndexValue(0);
    mesh->enableCollision_ = collisionEnabled_->isChecked();
    mesh->objectType_ = ObjectType(selectObjectType->GetBoxIndexValue(0));
    SetGLTypeInComponent(mesh);
}

void MeshWidget::UpdateComboBoxValues(std::shared_ptr<MeshComponent> mesh)
{
    selectMaterial_->ClearAllBoxes();
    unsigned int activeID = 0;
    for(unsigned int i = 0; i < AssetManager::GetInstance()->materialManager_->materials_.size(); i++)
    {
        QString name = AssetManager::GetInstance()->materialManager_->materials_[i]->name_;
        selectMaterial_->AddBoxValue(0,name);
        activeID = static_cast<unsigned int>(mesh->materialID_);
    }
    selectMaterial_->SetBoxCurrentIndex(0,activeID);
    activeID = 0;
    selectMesh_->ClearAllBoxes();
    for(unsigned int i = 0; i < AssetManager::GetInstance()->meshManager_->meshes_.size(); i++)
    {
        selectMesh_->AddBoxValue(0,AssetManager::GetInstance()->meshManager_->meshes_[i]->name_);
        activeID = static_cast<unsigned int>(mesh->meshID_);
    }
    selectMesh_->SetBoxCurrentIndex(0,activeID);
}

void MeshWidget::CreateGLSelection()
{
    selectGLDraw_->AddBoxValue(0,"GL_POINTS");        // 0
    selectGLDraw_->AddBoxValue(0,"GL_LINES");         // 1
    selectGLDraw_->AddBoxValue(0,"GL_LINE_STRIP");    // 2
    selectGLDraw_->AddBoxValue(0,"GL_LINE_LOOP");     // 3
    selectGLDraw_->AddBoxValue(0,"GL_TRIANGLES");     // 4
    selectGLDraw_->AddBoxValue(0,"GL_TRIANGLE_STRIP");// 5
    selectGLDraw_->AddBoxValue(0,"GL_TRIANGLE_FAN");  // 6
    selectGLDraw_->AddBoxValue(0,"GL_QUADS");         // 7
    selectGLDraw_->AddBoxValue(0,"GL_QUAD_STRIP");    // 8
    selectGLDraw_->AddBoxValue(0,"GL_POLYGON");       // 9
}

void MeshWidget::SetGLTypeInComponent(std::shared_ptr<MeshComponent> mesh)
{
    GLenum x = GL_TRIANGLES;
    switch(selectGLDraw_->GetBoxIndexValue(0))
    {
    case 0:
        x = GL_POINTS;
        break;
    case 1:
        x = GL_LINES;
        break;
    case 2:
        x = GL_LINE_STRIP;
        break;
    case 3:
        x = GL_LINE_LOOP;
        break;
    case 4:
        x = GL_TRIANGLES;
        break;
    case 5:
        x = GL_TRIANGLE_STRIP;
        break;
    case 6:
        x = GL_TRIANGLE_FAN;
        break;
    case 7:
        x = GL_QUADS;
        break;
    case 8:
        x = GL_QUAD_STRIP;
        break;
    case 9:
        x = GL_POLYGON;
        break;
    }
    mesh->mode_ = x;
}

void MeshWidget::SetGLTypeInWidget(std::shared_ptr<MeshComponent> mesh)
{


    unsigned int index = 0;
    switch(mesh->mode_)
    {
    case GL_POINTS:
        index = 0;
        break;
    case GL_LINES:
        index = 1;
        break;
    case GL_LINE_STRIP:
        index = 2;
        break;
    case GL_LINE_LOOP:
        index = 3;
        break;
    case GL_TRIANGLES:
        index = 4;
        break;
    case GL_TRIANGLE_STRIP:
        index = 5;
        break;
    case GL_TRIANGLE_FAN:
        index = 6;
        break;
    case GL_QUADS:
        index = 7;
        break;
    case GL_QUAD_STRIP:
        index = 8;
        break;
    case GL_POLYGON:
        index = 9;
        break;
    }
    selectGLDraw_->SetBoxCurrentIndex(0,index);
}
