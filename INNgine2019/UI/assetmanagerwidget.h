#ifndef ASSETMANAGERWIDGET_H
#define ASSETMANAGERWIDGET_H
#include "basewidget.h"
#include "Managers/assetmanager.h"
#include "assetviewer.h"
///Determines each AssetViewer's function.
enum AssetType
{
    ASSET_MESH,
    ASSET_SOUND,
    ASSET_MATERIAL,
    ASSET_TEXTURE
};

///The Asset Manager widget. Used to add and remove any asset.
class AssetManagerWidget : public BaseWidget
{
    Q_OBJECT
public:
    AssetManagerWidget(QWidget *parent, Qt::WindowFlags f = Qt::WindowFlags());
    /**
     *Initializes the widget and creates all information needed.
     * */
    void InitializeWidget() override;

    //Each asset we've decided to show.
    AssetViewer* meshes_{nullptr};
    AssetViewer* sounds_{nullptr};
    AssetViewer* materials_{nullptr};
    AssetViewer* textures_{nullptr};

    /**
     * Updates the values of each AssetViewer.
     * Called if another asset has been added.
     */
    void UpdateValues();


public slots:
    void event_soundBoxButtonPressed(ButtonPress button);
    void event_meshBoxButtonPressed(ButtonPress button);
    void event_textureBoxButtonPressed(ButtonPress button);
    void event_materialBoxButtonPressed(ButtonPress button);
signals:
    void addOrRemoveObject(AssetType assettype, ButtonPress button);



signals:

};

#endif // ASSETMANAGERWIDGET_H
