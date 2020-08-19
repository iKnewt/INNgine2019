#ifndef ASSETVIEWER_H
#define ASSETVIEWER_H

#include <QWidget>
///Determines which button that has been pressed. Sends through a signal.
enum ButtonPress{
    ADD_ASSET,
    REMOVE_ASSET,
    EDIT_ASSET
};

class QListWidget;
class QPushButton;

/**
 * The AssetViewer is a widget created to showcase a list of assets.
 * The AssetViewer in itself does not have any functionality, except for showcasing each asset.
 */
class AssetViewer : public QWidget
{
    Q_OBJECT
public:
    /**
     * Constructor for the Asset Viewer.
     * @param assetName The name of the specified asset to show.
     * @param editoption To determine if the Edit button should be showed.
     * @param parent parent of this widget.
     */
    explicit AssetViewer(QString assetName, bool editoption,QWidget *parent = nullptr);

    QListWidget* assetList_{nullptr};
    QPushButton* addasset_{nullptr};
    QPushButton* removeasset_{nullptr};
    QPushButton* editasset_{nullptr};
    /**
     * Adds an asset on the list.
     * @param name name of the asset to add.
     */
    void AddAsset(QString name);
    /**
     * Clears the entire list.
     */
    void ClearAllAssets();
    /**
     * Gets the current index of the asset selected.
     * @return The index of the asset selected.
     */
    unsigned int getCurrentSelectedIndex();

signals:
    void buttonPressed(ButtonPress);
public slots:
    void event_addAsset_clicked();
    void event_removeAsset_clicked();
    void event_editAsset_clicked();
};

#endif // ASSETVIEWER_H
