#include "transformwidget.h"

#include <QCheckBox>
#include <QDoubleValidator>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include "spinboxproperty.h"
#include "comboboxproperty.h"

TransformWidget::TransformWidget(QWidget *parent, Qt::WindowFlags f)  : BaseWidget(parent,f)
{
    InitializeWidget();
}

void TransformWidget::InitializeWidget()
{
    int maxValue = 2147483647;
    positionSpinBoxes_ = new SpinBoxProperty("Position",3,-maxValue,maxValue,3,this);
    positionSpinBoxes_->setSpinBoxesFixedWidth(75);
    connect(positionSpinBoxes_,&SpinBoxProperty::AnyValueChanged,this,&BaseWidget::event_anyPropertyChanged);
    rotationSpinBoxes_ = new SpinBoxProperty("Rotation",3,-maxValue,maxValue,3,this);
    rotationSpinBoxes_->setSpinBoxesFixedWidth(75);
    connect(rotationSpinBoxes_,&SpinBoxProperty::AnyValueChanged,this,&BaseWidget::event_anyPropertyChanged);
    scaleSpinBoxes_ = new SpinBoxProperty("Scale",3,-maxValue,maxValue,3,this);
    scaleSpinBoxes_->setSpinBoxesFixedWidth(75);
    connect(scaleSpinBoxes_,&SpinBoxProperty::AnyValueChanged,this,&BaseWidget::event_anyPropertyChanged);

    QGroupBox* transformGroup = new QGroupBox("Transform",this);
    transformGroup->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    QVBoxLayout* transformLayout = CreateDefaultVLayout();
    transformLayout->addWidget(positionSpinBoxes_);
    transformLayout->addWidget(rotationSpinBoxes_);
    transformLayout->addWidget(scaleSpinBoxes_);
    transformGroup->setLayout(transformLayout);

    QGroupBox* otherGroup = new QGroupBox("Other",this);

    followLandscape_ = new QCheckBox("Follow Landscape",this);
    connect(followLandscape_,&QCheckBox::clicked,this,&TransformWidget::event_anyPropertyChanged);

    orientRotationBasedOnMovement_ = new QCheckBox("Orient Rotation Based On Movement",this);
    connect(orientRotationBasedOnMovement_,&QCheckBox::clicked,this,&BaseWidget::event_anyPropertyChanged);

    QVBoxLayout* otherLayout = CreateDefaultVLayout();
    otherLayout->addWidget(followLandscape_);
    otherLayout->addWidget(orientRotationBasedOnMovement_);
    otherGroup->setLayout(otherLayout);


    QVBoxLayout* finalLayout = CreateDefaultVLayout();
    finalLayout->addWidget(transformGroup);
    finalLayout->addWidget(otherGroup);

    this->setLayout(finalLayout);
}

void TransformWidget::UpdateTransformWidget(std::shared_ptr<TransformComponent> transform)
{
    SetSpinBoxValuesFromVector3D(positionSpinBoxes_,transform->position_relative_);
    SetSpinBoxValuesFromVector3D(rotationSpinBoxes_,transform->rotation_relative_);
    SetSpinBoxValuesFromVector3D(scaleSpinBoxes_,transform->scale_relative_);
    followLandscape_->setChecked(transform->followLandscape_);
    orientRotationBasedOnMovement_->setChecked(transform->orientRotationBasedOnMovement_);
}

void TransformWidget::UpdateTransformComponent(std::shared_ptr<TransformComponent> transform)
{
    transform->position_relative_ = SpinBoxToVector3D(positionSpinBoxes_);
    transform->rotation_relative_ = SpinBoxToVector3D(rotationSpinBoxes_);
    transform->scale_relative_ = SpinBoxToVector3D(scaleSpinBoxes_);
    transform->followLandscape_ = followLandscape_->isChecked();
    transform->orientRotationBasedOnMovement_ = orientRotationBasedOnMovement_->isChecked();
}
