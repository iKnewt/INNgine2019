#ifndef LIGHTWIDGET_H
#define LIGHTWIDGET_H
#include "basewidget.h"

class QGroupBox;
class ComboBoxProperty;
class QStackedWidget;
///The LightWidget shows all information from the LightComponent that the user can modify in runtime.
class LightWidget : public BaseWidget
{
public:
    LightWidget(QWidget *parent, Qt::WindowFlags f = Qt::WindowFlags());
    /**
     *Initializes the widget and creates all subwidgets needed.
     */
    void InitializeWidget() override;
    /**
     * Updates the values from the LightComponent to the Widget.
     * @param light the Active Entity's LightComponent.
     */
    void UpdateLightWidget(std::shared_ptr<LightComponent> light);
    /**
     * Updates the values from the Widget to the LightComponent.
     * @param light the Active Entity's LightComponent.
     */
    void UpdateLightComponent(std::shared_ptr<LightComponent> light);

    /**
     * Sets the ComboBox to its correct light type position.
     * @param lightType the LightComponent's enum lightType_
     */
    void SetCurrentLightType(LightType lightType);

    QGroupBox* CreatePointLightProperties();
    QGroupBox* CreateDirectionalLightProperties();
    QGroupBox* CreateSpotLightProperties();

    //Used by directional light and spot light
    QGroupBox* directionGroupBox_{nullptr};
    QGroupBox* spotLightGroupBox_{nullptr};
    QGroupBox* pointLightGroupBox_{nullptr};

    ///SpinBoxProperty to specify the light type in the LightComponent.
    ComboBoxProperty* lightType_{nullptr};
    ///SpinBoxProperty to specify the active LightComponent's ambient.
    SpinBoxProperty* ambient_{nullptr};
    ///SpinBoxProperty to specify the active LightComponent's diffuse.
    SpinBoxProperty* diffuse_{nullptr};
    ///SpinBoxProperty to specify the active LightComponent's specular.
    SpinBoxProperty* specular_{nullptr};

    //Point Light
    ///SpinBoxProperty to specify the active LightComponent's constant in a spotlight.
    SpinBoxProperty* constant_{nullptr};
    ///SpinBoxProperty to specify the active LightComponent's linear in a spotlight.
    SpinBoxProperty* linear_{nullptr};
    ///SpinBoxProperty to specify the active LightComponent's quadratic in a spotlight.
    SpinBoxProperty* quadratic_{nullptr};

    //Spot light
    ///SpinBoxProperty to specify the active LightComponent's Cut off value. Only used in a Spotlight.
    SpinBoxProperty* cutOff_{nullptr};
    ///SpinBoxProperty to specify the active LightComponent's Other Cut off value. Only used in a Spotlight.
    SpinBoxProperty* outerCutOff_{nullptr};

    //Directional + spotlight
    ///SpinBoxProperty to specify the active LightComponent's light direction. Used in Directional light + Spotlight.
    SpinBoxProperty* direction_{nullptr};
    ///SpinBoxProperty to specify if the active LightComponent should get a forward vector from transform.
    QCheckBox* useForwardVectorAsDirection_{nullptr};
};

#endif // LIGHTWIDGET_H
