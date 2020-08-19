#include "assetviewer.h"
#include "basewidget.h"

#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

AssetViewer::AssetViewer(QString assetName, bool editoption, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* layoutMaster = BaseWidget::CreateDefaultVLayout();
    assetList_ = new QListWidget(this);
    assetList_->setAlternatingRowColors(true);
    assetList_->setSortingEnabled(false);
    assetList_->setDefaultDropAction(Qt::DropAction::MoveAction);
    layoutMaster->addWidget(assetList_);

    addasset_ = new QPushButton(QString("Add " + assetName),this);
    connect(addasset_,&QPushButton::clicked,this,&AssetViewer::event_addAsset_clicked);
    layoutMaster->addWidget(addasset_);

    removeasset_ = new QPushButton(QString("Remove " + assetName),this);
    connect(removeasset_,&QPushButton::clicked,this,&AssetViewer::event_removeAsset_clicked);
    layoutMaster->addWidget(removeasset_);

    if(editoption)
    {
        editasset_ = new QPushButton(QString("Edit " + assetName),this);
        connect(editasset_,&QPushButton::clicked,this,&AssetViewer::event_editAsset_clicked);
        layoutMaster->addWidget(editasset_);

    }


    this->setLayout(layoutMaster);
}

void AssetViewer::AddAsset(QString name)
{
    assetList_->addItem(name);
}

void AssetViewer::ClearAllAssets()
{
    assetList_->clear();
}

unsigned int AssetViewer::getCurrentSelectedIndex()
{

    return static_cast<unsigned int>(assetList_->currentIndex().row());
}

void AssetViewer::event_addAsset_clicked()
{
    emit buttonPressed(ADD_ASSET);
}

void AssetViewer::event_removeAsset_clicked()
{
    emit buttonPressed(REMOVE_ASSET);
}

void AssetViewer::event_editAsset_clicked()
{
    emit(buttonPressed(EDIT_ASSET));
}
