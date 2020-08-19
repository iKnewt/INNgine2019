#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include <QWidget>
#include "Managers/componentmanager.h"
class SpinBoxProperty;
class QVBoxLayout;

///The Base of all Component-based Widgets.
class BaseWidget : public QWidget
{
    Q_OBJECT
public:
    BaseWidget(QWidget *parent, Qt::WindowFlags f = Qt::WindowFlags())  : QWidget(parent,f){}

    /**
     *Initializes the widget and creates all information needed.
     *BaseWidget cannot use this function.
     */
    virtual void InitializeWidget() = 0;

    /**
     * Creates a default VBoxLayout with the correct settings needed by the widgets.
     * @return a VBoxLayout with the default settings wanted by the widget.
     */
    static QVBoxLayout* CreateDefaultVLayout();
    /**
     * Creates a Vector3d out of the values of of a SpinBoxProperty with 3 boxes.
     * @param spinbox The specified Spinbox.
     * @return Vector3d of the values of of a SpinBoxProperty with 3 boxes.
     */
    static gsl::Vector3D SpinBoxToVector3D(SpinBoxProperty* spinbox);
    /**
     * Sets all values from a vector 3d into a SpinBoxProperty with 3 boxes.
     * @param spinbox the spinbox to set the values in
     * @param values the Vector3D values
     */
    static void SetSpinBoxValuesFromVector3D(SpinBoxProperty* spinbox, gsl::Vector3D values);

public slots:
    /**
     * Runs whenever a property inside the widget is changed.
     * Emits the signal anyValueChanged to MainWindow.
     */
    void event_anyPropertyChanged(){emit anyValueChanged();}

signals:
    /**
     * A signal to emit to MainWindow whenever a property in the widget has changed.
     */
    void anyValueChanged();
};

#endif // BASEWIDGET_H
