#include "spinboxproperty.h"
#include <QVBoxLayout>
#include <qvalidator.h>
#include "valuebox.h"

SpinBoxProperty::SpinBoxProperty(QString propertyName, unsigned int amountOfBoxes, int min, int max, int incrementValue, QWidget *parent, Qt::WindowFlags f) : BaseProperty(parent,f)
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setMargin(3);
    layout->setSpacing(0);
    propertyName_ = new QLabel(propertyName);
    layout->addWidget(propertyName_,0,Qt::AlignLeft);



    for(unsigned int i = 0; i < amountOfBoxes; i++)
    {
        ValueBox* spinBox = new ValueBox(min,max,incrementValue,i,this);
        //        spinBox->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
        connect(spinBox,&ValueBox::boxChanged,this,&SpinBoxProperty::Event_AnyValueChanged);
        boxes_.push_back(spinBox);
        layout->addWidget(spinBox,0,Qt::AlignRight);
    }
    this->setLayout(layout);
}

void SpinBoxProperty::SetSpinBoxValue(unsigned int index, float value)
{
    if(index > boxes_.size())
        qDebug() << "ERROR IN FUNCTION: SetSpinBoxValue - No box found on specified index!";
    else
        boxes_[index]->setText(QString::number(double(value)));

}

float SpinBoxProperty::GetSpinBoxValue(unsigned int index)
{
    if(index > boxes_.size())
    {
        qDebug() << "ERROR IN FUNCTION: GetSpinBoxValue - No box found on specified index!";
        return 0;
    }
    return boxes_[index]->text().toFloat();

}

void SpinBoxProperty::setSpinBoxesFixedWidth(int size)
{
    for(unsigned int i = 0; i < boxes_.size(); i++)
    {
        boxes_[i]->setFixedWidth(size);
    }
}
