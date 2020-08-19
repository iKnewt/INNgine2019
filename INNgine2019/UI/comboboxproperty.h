#ifndef COMBOBOXPROPERTY_H
#define COMBOBOXPROPERTY_H
#include "baseproperty.h"

class QComboBox;
///The drop-down menu custom-made Property-Widget.
class ComboBoxProperty : public BaseProperty
{
    Q_OBJECT
public:
    ComboBoxProperty(QString propertyName, unsigned int amountOfBoxes, QWidget* parent, Qt::WindowFlags f = Qt::WindowFlags());
    ///The vector containing QComboBoxes.
    std::vector<QComboBox*> boxes_;
    /**
     *Clears all boxes in the ComboBoxProperty.
     */
    void ClearAllBoxes();
    /**
     * Adds a item into the specified ComboBox.
     * @param index To specify which box to change.
     * @param value The text to add in the specified box.
     */
    void AddBoxValue(unsigned int index, QString value);
    /**
     * Gets the current index of the specified box.
     * @param index To specify which box to get the index from.
     * @return The current index of the specified box.
     */
    unsigned int GetBoxIndexValue(unsigned int index);
    /**
     * Sets the current index of the specified box.
     * @param boxIndex To specify which box.
     * @param index The index to select.
     */
    void SetBoxCurrentIndex(unsigned int boxIndex, unsigned int index);
    /**
     * Sets every box in the ComboBoxProperty's fixed width.
     * @param size the new width.
     */
    void SetComboBoxFixedWidth(int size);
    /**
     *Gets the box specified's current text.
     * @param boxIndex The specified box to get the text from.
     * @return The QString in the index of the specified box.
     */
    QString GetCurrentBoxText(unsigned int boxIndex);

};

#endif // COMBOBOXPROPERTY_H
