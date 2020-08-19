#include "comboboxproperty.h"
#include "lightwidget.h"
#include "spinboxproperty.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QStackedWidget>
#include <QVBoxLayout>

LightWidget::LightWidget(QWidget *parent, Qt::WindowFlags f) : BaseWidget(parent,f)
{
    InitializeWidget();
}

void LightWidget::InitializeWidget()
{
    lightType_ = new ComboBoxProperty("Light Type",1,this);
    lightType_->AddBoxValue(0,"Point Light");
    lightType_->AddBoxValue(0,"Directional Light");
    lightType_->AddBoxValue(0,"Spot Light");
    connect(lightType_,&ComboBoxProperty::AnyValueChanged,this,&LightWidget::event_anyPropertyChanged);
    QGroupBox* color = new QGroupBox("Color",this);
    QVBoxLayout* colorSettings = CreateDefaultVLayout();
    ambient_ = new SpinBoxProperty("Ambient",3,0,gsl::MAX_COLORS,5,this);
    ambient_->setSpinBoxesFixedWidth(75);
    colorSettings->addWidget(ambient_);
    connect(ambient_,&SpinBoxProperty::AnyValueChanged,this,&LightWidget::event_anyPropertyChanged);

    diffuse_ = new SpinBoxProperty("Diffuse",3,0,gsl::MAX_COLORS,5,this);
    diffuse_->setSpinBoxesFixedWidth(75);
    colorSettings->addWidget(diffuse_);
    connect(diffuse_,&SpinBoxProperty::AnyValueChanged,this,&LightWidget::event_anyPropertyChanged);

    specular_ = new SpinBoxProperty("Specular",3,0,gsl::MAX_COLORS,5,this);
    specular_->setSpinBoxesFixedWidth(75);
    colorSettings->addWidget(specular_);
    connect(specular_,&SpinBoxProperty::AnyValueChanged,this,&LightWidget::event_anyPropertyChanged);
    color->setLayout(colorSettings);

    QVBoxLayout* masterLayout = CreateDefaultVLayout();
    masterLayout->addWidget(lightType_);
    masterLayout->addWidget(color);
    masterLayout->addWidget(CreatePointLightProperties());
    masterLayout->addWidget(CreateDirectionalLightProperties());
    masterLayout->addWidget(CreateSpotLightProperties());
    this->setLayout(masterLayout);
}

void LightWidget::UpdateLightWidget(std::shared_ptr<LightComponent> light)
{
    SetSpinBoxValuesFromVector3D(ambient_,light->ambient_);
    SetSpinBoxValuesFromVector3D(diffuse_,light->diffuse_);
    SetSpinBoxValuesFromVector3D(specular_,light->specular_);
    constant_->SetSpinBoxValue(0,light->constant_);
    //Point Light
    linear_->SetSpinBoxValue(0,light->linear_);
    quadratic_->SetSpinBoxValue(0,light->quadratic_);
    SetCurrentLightType(light->lightType_);
    //Direction Light
    BaseWidget::SetSpinBoxValuesFromVector3D(direction_,light->direction_);
    useForwardVectorAsDirection_->setChecked(light->useEntityTransformForwardVectorAsDirection_);
    //Spot Light
    cutOff_->SetSpinBoxValue(0,light->cutOff_);
    outerCutOff_->SetSpinBoxValue(0,light->outerCutOff_);
}

void LightWidget::UpdateLightComponent(std::shared_ptr<LightComponent> light)
{
    light->ambient_ = SpinBoxToVector3D(ambient_);
    light->diffuse_ = SpinBoxToVector3D(diffuse_);
    light->specular_ = SpinBoxToVector3D(specular_);

    light->lightType_ = LightType(lightType_->GetBoxIndexValue(0));
    //needs to happen after lighttype has been set.
    SetCurrentLightType(light->lightType_);
    //Point Light
    light->constant_ = constant_->GetSpinBoxValue(0);
    light->linear_ = linear_->GetSpinBoxValue(0);
    light->quadratic_ = quadratic_->GetSpinBoxValue(0);
    //Directional Light
    useForwardVectorAsDirection_->isChecked() ? direction_->hide() : direction_->show();
    light->direction_ = gsl::Vector3D(direction_->GetSpinBoxValue(0),direction_->GetSpinBoxValue(1),direction_->GetSpinBoxValue(2));
    light->useEntityTransformForwardVectorAsDirection_ = useForwardVectorAsDirection_->isChecked();
    //Spot light
    light->cutOff_ = cutOff_->GetSpinBoxValue(0);
    light->outerCutOff_ = outerCutOff_->GetSpinBoxValue(0);
}

void LightWidget::SetCurrentLightType(LightType lightType)
{
    switch(lightType)
    {
    case POINT_LIGHT:
        lightType_->SetBoxCurrentIndex(0,0);
        pointLightGroupBox_->show();
        spotLightGroupBox_->hide();
        directionGroupBox_->hide();
        break;
    case DIRECTIONAL_LIGHT:
        lightType_->SetBoxCurrentIndex(0,1);
        pointLightGroupBox_->hide();
        spotLightGroupBox_->hide();
        directionGroupBox_->show();
        break;
    case SPOT_LIGHT:
        lightType_->SetBoxCurrentIndex(0,2);
        pointLightGroupBox_->hide();
        spotLightGroupBox_->show();
        directionGroupBox_->show();
        break;
    }
}

QGroupBox *LightWidget::CreatePointLightProperties()
{
    pointLightGroupBox_ = new QGroupBox("Attenuation (Point light)");
    pointLightGroupBox_->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum);
    QVBoxLayout* attenuationLayout = CreateDefaultVLayout();
    constant_ = new SpinBoxProperty("Constant",1,0,1,5,this);
    constant_->setSpinBoxesFixedWidth(125);
    attenuationLayout->addWidget(constant_);
    connect(constant_,&SpinBoxProperty::AnyValueChanged,this,&LightWidget::event_anyPropertyChanged);

    linear_ = new SpinBoxProperty("Linear",1,0,1,5,this);
    linear_->setSpinBoxesFixedWidth(125);
    attenuationLayout->addWidget(linear_);
    connect(linear_,&SpinBoxProperty::AnyValueChanged,this,&LightWidget::event_anyPropertyChanged);

    quadratic_ = new SpinBoxProperty("Quadratic",1,0,1,5,this);
    quadratic_->setSpinBoxesFixedWidth(125);
    attenuationLayout->addWidget(quadratic_);
    connect(quadratic_,&SpinBoxProperty::AnyValueChanged,this,&LightWidget::event_anyPropertyChanged);
    pointLightGroupBox_->setLayout(attenuationLayout);
    return pointLightGroupBox_;

}

QGroupBox *LightWidget::CreateDirectionalLightProperties()
{
    directionGroupBox_ = new QGroupBox("Direction (Directional light)");
    directionGroupBox_->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum);
    QVBoxLayout* directionLayout = CreateDefaultVLayout();

    useForwardVectorAsDirection_ = new QCheckBox("Use Forward Vector as Directoion",this);
    connect(useForwardVectorAsDirection_,&QCheckBox::clicked,this,&LightWidget::event_anyPropertyChanged);

    direction_ = new SpinBoxProperty("Light Direction",3,-10000000,10000000,3,this);
    direction_->setSpinBoxesFixedWidth(75);
    connect(direction_,&SpinBoxProperty::AnyValueChanged,this,&LightWidget::event_anyPropertyChanged);

    directionGroupBox_->setLayout(directionLayout);
    directionLayout->addWidget(useForwardVectorAsDirection_);
    directionLayout->addWidget(direction_);
    return directionGroupBox_;
}

QGroupBox *LightWidget::CreateSpotLightProperties()
{
    spotLightGroupBox_ = new QGroupBox("Cut Off (Spotlight)");
    spotLightGroupBox_->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum);
    QVBoxLayout* cutOffLayout = CreateDefaultVLayout();
    spotLightGroupBox_->setLayout(cutOffLayout);

    cutOff_ = new SpinBoxProperty("Inner Cut Off",1,0,4,3,this);
    connect(cutOff_,&SpinBoxProperty::AnyValueChanged,this,&LightWidget::event_anyPropertyChanged);
    outerCutOff_ = new SpinBoxProperty("Outer Cut Off",1,0,4,3,this);
    connect(outerCutOff_,&SpinBoxProperty::AnyValueChanged,this,&LightWidget::event_anyPropertyChanged);
    cutOffLayout->addWidget(cutOff_);
    cutOffLayout->addWidget(outerCutOff_);

    return spotLightGroupBox_;
}
