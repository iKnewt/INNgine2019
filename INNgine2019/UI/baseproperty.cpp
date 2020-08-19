#include "baseproperty.h"

BaseProperty::BaseProperty(QWidget *parent, Qt::WindowFlags f) : QWidget(parent,f)
{
}

void BaseProperty::Event_AnyValueChanged(unsigned int index)
{
    emit AnyValueChanged(index);
}
