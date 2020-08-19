#ifndef BASEPROPERTY_H
#define BASEPROPERTY_H

#include <QWidget>
#include <QLabel>

///The base of all custom-made property-based widgets.
class BaseProperty : public QWidget
{
    Q_OBJECT
public:
    BaseProperty(QWidget *parent, Qt::WindowFlags f = Qt::WindowFlags());

    QLabel* propertyName_;

public slots:
    /**
     * Runs whenever a value in any of the boxes get changed.
     * @param index gives the specific index of the box that has been changed.
     */
    void Event_AnyValueChanged(unsigned int index);
signals:
    /**
     * Signal to send to the respective Component-Widget to indicate that a value has changed.
     * @param value The specific index that has been changed in the box.
     */
    void AnyValueChanged(unsigned int value);
};

#endif // BASEPROPERTY_H
