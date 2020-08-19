#include "comboboxproperty.h"

#include <QComboBox>
#include <QVBoxLayout>

ComboBoxProperty::ComboBoxProperty(QString propertyName, unsigned int amountOfBoxes, QWidget *parent, Qt::WindowFlags f) : BaseProperty(parent,f)
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setMargin(3);
    layout->setSpacing(0);
    propertyName_ = new QLabel(propertyName);
    layout->addWidget(propertyName_,0,Qt::AlignLeft);

    for(unsigned int i = 0; i < amountOfBoxes; i++)
    {
        QComboBox* comboBox = new QComboBox(this);
        comboBox->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum);
        connect(comboBox,QOverload<int>::of(&QComboBox::activated),this,&ComboBoxProperty::Event_AnyValueChanged);
        boxes_.push_back(comboBox);
        layout->addWidget(comboBox,0,Qt::AlignRight);
    }
    this->setLayout(layout);

}

void ComboBoxProperty::ClearAllBoxes()
{
    for(unsigned int i = 0; i < boxes_.size(); i++)
    {
        boxes_[i]->clear();
    }
}

void ComboBoxProperty::AddBoxValue(unsigned int index, QString value)
{
    if(index > boxes_.size())
    {
        qDebug() << "ERROR IN FUNCTION: SetBoxValue - Box " << propertyName_ << "only has " << boxes_.size() << "boxes! Returning...";
        return;
    }
    boxes_[0]->addItem(value);

}

unsigned int ComboBoxProperty::GetBoxIndexValue(unsigned int index)
{
    if(index > boxes_.size() - 1)
    {
        qDebug() << "ERROR IN FUNCTION: GetBoxIndexValue - Box " << propertyName_ << "only has " << boxes_.size() << "boxes! Returning 0...";
        return 0;
    }
    return static_cast<unsigned int>(boxes_[index]->currentIndex());
}

void ComboBoxProperty::SetBoxCurrentIndex(unsigned int boxIndex, unsigned int index)
{
    if(boxIndex > boxes_.size() - 1)
    {
        qDebug() << "ERROR IN FUNCTION: GetBoxIndexValue - Box " << propertyName_ << "only has " << boxes_.size() << "boxes! Returning...";
        return ;
    }
    if(static_cast<int>(index) > boxes_[boxIndex]->count())
    {
        qDebug() << "ERROR IN FUNCTION: GetBoxIndexValue - Box " << propertyName_ << "only has " << boxes_[boxIndex]->count() << "items in it! Returning...";
        return ;
    }
    boxes_[boxIndex]->setCurrentIndex(static_cast<int>(index));
}

void ComboBoxProperty::SetComboBoxFixedWidth(int size)
{
    for(unsigned int i = 0; i < boxes_.size(); i++)
    {
        boxes_[i]->setFixedWidth(size);
    }
}

QString ComboBoxProperty::GetCurrentBoxText(unsigned int boxIndex)
{
    if(boxIndex > boxes_.size() - 1)
    {
        qDebug() << "ERROR IN FUNCTION: GetBoxIndexValue - Box " << propertyName_ << "only has " << boxes_.size() << "boxes! Returning...";
        return "";
    }
    return boxes_[boxIndex]->currentText();
}
