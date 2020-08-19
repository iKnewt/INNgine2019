#ifndef SPINBOXPROPERTY_H
#define SPINBOXPROPERTY_H
#include "baseproperty.h"


class ValueBox;
class QLabel;
///The value custom-made Property-Widget.
class SpinBoxProperty : public BaseProperty
{
    Q_OBJECT
public:
    SpinBoxProperty(QString propertyName, unsigned int amountOfBoxes, int min, int max, int incrementValue, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    /**
     * Sets the specified box's value.
     * @param index the specified box's index.
     * @param value the value to set.
     */
    void SetSpinBoxValue(unsigned int index, float value);
    /**
     * Gets the specified box's value.
     * @param index the specified box's index.
     * @param value the value to get.
     */
    float GetSpinBoxValue(unsigned int index);
    /**
     * Sets every box in the SpinBoxProperty's fixed width.
     * @param size the new width.
     */
    void setSpinBoxesFixedWidth(int size);
    ///The vector containing ValueBoxes.
    std::vector<ValueBox*> boxes_;
};

#endif // SPINBOXPROPERTY_H
