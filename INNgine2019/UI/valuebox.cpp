#include "valuebox.h"

#include <QDoubleValidator>

ValueBox::ValueBox(int min, int max, int incrementValue, unsigned int index, QWidget *parent) : QLineEdit(parent)
{
    boxIndex = index;
    this->setValidator(new QDoubleValidator(min, max,incrementValue,this));
    connect(this,&QLineEdit::editingFinished,this,&ValueBox::valueBox_EditingFinished);
}

ValueBox::~ValueBox()
{

}

void ValueBox::valueBox_EditingFinished()
{
    emit boxChanged(boxIndex);
}
