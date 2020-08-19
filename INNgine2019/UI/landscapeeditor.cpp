#include "basewidget.h"
#include "comboboxproperty.h"
#include "landscapeeditor.h"
#include "ui_landscapeeditor.h"
#include "Managers/assetmanager.h"
#include "Legacy/constants.h"

#include <QFileDialog>
#include <QPushButton>

LandscapeEditor::LandscapeEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LandscapeEditor)
{
    ui->setupUi(this);
    InitalizeWindow();
}

LandscapeEditor::~LandscapeEditor()
{
    delete ui;
}

void LandscapeEditor::InitalizeWindow()
{
    QVBoxLayout* masterLayout = BaseWidget::CreateDefaultVLayout();
    QPushButton* setMesh = new QPushButton("Update Mesh",this);
    masterLayout->addWidget(setMesh);
    connect(setMesh,&QPushButton::clicked,this,&LandscapeEditor::event_setMesh_clicked);
    selectMaterial_ = new ComboBoxProperty("name",1,this);
    connect(selectMaterial_,&ComboBoxProperty::AnyValueChanged,this,&LandscapeEditor::event_selectMaterial_indexChanged);
    masterLayout->addWidget(selectMaterial_);
    selectMaterial_->SetComboBoxFixedWidth(150);
    UpdateComboBox();
    ui->verticalLayout->addLayout(masterLayout);
}

void LandscapeEditor::UpdateComboBox()
{
    selectMaterial_->ClearAllBoxes();
    for(unsigned int i = 0; i < AssetManager::GetInstance()->materialManager_->materials_.size(); i++)
    {
        QString name = AssetManager::GetInstance()->materialManager_->materials_[i]->name_;
        selectMaterial_->AddBoxValue(0,name);
    }
    selectMaterial_->SetBoxCurrentIndex(0,static_cast<unsigned int>(AssetManager::GetInstance()->landscape_->materialID_));
}

void LandscapeEditor::event_setMesh_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this,
                                                    tr("Load Landscape"), gsl::textureFilePath,
                                                    tr("OBJ (*.obj)"));
    if(filepath != "")
    {
        AssetManager::GetInstance()->updateLandscape(filepath);
        this->close();
    }
    else
    {
        CreateMessageBox("Warning!","ERROR: Invalid Filepath!");
    }
}

void LandscapeEditor::event_selectMaterial_indexChanged()
{
    qDebug() << "this hapesn";
    AssetManager::GetInstance()->landscape_->materialID_ = selectMaterial_->GetBoxIndexValue(0);
}
