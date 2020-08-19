#include "basewidget.h"
#include "lineboxproperty.h"

#include <QLineEdit>
#include <QVBoxLayout>

LineBoxProperty::LineBoxProperty(QString propertyName, unsigned int amountOfBoxes, QWidget *parent, Qt::WindowFlags f) : BaseProperty(parent,f)
{
    QHBoxLayout* layout = new QHBoxLayout;
    propertyName_ = new QLabel(propertyName,this);
    layout->addWidget(propertyName_,0,Qt::AlignLeft);

    for(unsigned int i = 0; i < amountOfBoxes; i++)
    {
        QLineEdit* box = new QLineEdit(this);
        layout->addWidget(box,0,Qt::AlignRight);
        connect(box,&QLineEdit::editingFinished,this,&LineBoxProperty::event_LineBoxChanged);
        boxes_.push_back(box);
    }
    this->setLayout(layout);


}

void LineBoxProperty::SetBoxText(unsigned int index, QString text)
{
    boxes_[index]->setText(text);
}

QString LineBoxProperty::GetBoxText(unsigned int index)
{
    return boxes_[index]->text();
}

void LineBoxProperty::event_LineBoxChanged()
{
    emit AnyValueChanged(0);
}
