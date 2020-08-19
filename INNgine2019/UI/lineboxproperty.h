#ifndef LINEBOXPROPERTY_H
#define LINEBOXPROPERTY_H
#include "baseproperty.h"

class QLineEdit;
///The text custom-made Property-Widget.
class LineBoxProperty : public BaseProperty
{
    Q_OBJECT
public:
    /**
     * Constructor for the LineBoxProperty.
     * @param propertyName The name of the Property.
     * @param amountOfBoxes The amount of boxes needed for this Property.
     * @param parent The parent of this widget.
     * @param f The Qt::Windowflags used.
     */
    LineBoxProperty(QString propertyName, unsigned int amountOfBoxes,QWidget *parent, Qt::WindowFlags f = Qt::WindowFlags());
    std::vector<QLineEdit*> boxes_;
    /**
     * Sets the specified box's text.
     * @param index the index of the box.
     * @param text the text the box should have.
     */
    void SetBoxText(unsigned int index, QString text);
    QString GetBoxText(unsigned int index);
public slots:
    void event_LineBoxChanged();
};


#endif // LINEBOXPROPERTY_H
