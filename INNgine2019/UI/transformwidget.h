#ifndef TRANSFORMWIDGET_H
#define TRANSFORMWIDGET_H
#include "basewidget.h"

class QCheckBox;
class SpinBoxProperty;

///The TransformWidget shows all information from the TransformComponent that the user can modify in runtime.
class TransformWidget : public BaseWidget
{
    Q_OBJECT
public:
    TransformWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    /**
     *Initializes the widget and creates all subwidgets needed.
     */
    void InitializeWidget() override;
    ///SpinBoxProperty to specify the active TransformComponent's position.
    SpinBoxProperty* positionSpinBoxes_{nullptr};
    ///SpinBoxProperty to specify the active TransformComponent's rotation.
    SpinBoxProperty* rotationSpinBoxes_{nullptr};
    ///SpinBoxProperty to specify the active TransformComponent's scale.
    SpinBoxProperty* scaleSpinBoxes_{nullptr};
    ///Checkbox to specify the active TransformComponent's follow landscape setting.
    QCheckBox* followLandscape_{nullptr};
    ///Checkbox to specify the active TransformComponent's orient rotation based on movement setting.
    QCheckBox* orientRotationBasedOnMovement_{nullptr};
    /**
     * Updates the values from the TransformComponent to the Widget.
     * @param transform the Active Entity's TransformComponent.
     */
    void UpdateTransformWidget(std::shared_ptr<TransformComponent> transform);
    /**
     * Updates the values from the Widget to the TransformComponent.
     * @param transform the Active Entity's TransformComponent.
     */
    void UpdateTransformComponent(std::shared_ptr<TransformComponent> transform);
};

#endif // TRANSFORMWIDGET_H
