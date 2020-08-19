#include "basewidget.h"
#include "spinboxproperty.h"

#include <QVBoxLayout>


QVBoxLayout *BaseWidget::CreateDefaultVLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(0);
    layout->setMargin(5);
    return layout;
}

gsl::Vector3D BaseWidget::SpinBoxToVector3D(SpinBoxProperty *spinbox)
{
    return gsl::Vector3D(spinbox->GetSpinBoxValue(0),spinbox->GetSpinBoxValue(1),spinbox->GetSpinBoxValue(2));
}

void BaseWidget::SetSpinBoxValuesFromVector3D(SpinBoxProperty *spinbox, gsl::Vector3D values)
{
    if(spinbox->boxes_.size() > 3)
    {
        qDebug() << "ERROR IN FUNCTION: SetTransformValues - Spinbox parameter has more than 3 slots! Returning...";
        return;
    }
    spinbox->SetSpinBoxValue(0,values.x);
    spinbox->SetSpinBoxValue(1,values.y);
    spinbox->SetSpinBoxValue(2,values.z);
}
