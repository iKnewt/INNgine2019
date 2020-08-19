
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>

#include <QListWidget>
#include <QTextEdit>
#include <QtWidgets>
#include <QAbstractItemView>
#include <UI/transformwidget.h>
#include <UI/meshwidget.h>
#include <UI/audiowidget.h>
#include <UI/lightwidget.h>
#include <UI/assetmanagerwidget.h>
#include <UI/landscapeeditor.h>

class QWidget;
class RenderWindow;

namespace Ui {
class MainWindow;
}

///Maindow is the core of the program. It holds the UI and the RenderWindow.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent =nullptr);
    ~MainWindow();

    QWindow* GLWindowContainer;

private slots:

    // **** World Outliner ****
    void event_entityTreeWidget_itemClicked(QTreeWidgetItem *item);
    void event_addEntityButton_clicked();
    void event_removeEntityButton_clicked();
    void event_componentListItem_Clicked(QListWidgetItem* item);

    // **** Toolbar ****
    void event_actionTwofacedCulling_toggled(bool arg1);
    void event_actionLOD_toggled(bool arg1);
    void event_actionFrustumCulling_toggled(bool arg1);
    void event_actionShowCollision_toggled(bool arg1);
    void event_actionMinecraftMouse_triggered(bool checked);
    void event_actionComicSans_triggered(bool checked);
    void event_newCameraSelected(QAction *action);
    void event_actionSave_triggered();
    void event_actionSaveAs_triggered();
    void event_actionLoad_triggered();
    void event_actionLoadTrophiesFromScript_triggered();
    void event_actionViewMode_changed(unsigned int index);

    void event_addComponentButton_clicked();
    void event_deleteComponentButton_clicked();
    void event_landscapeEditor_clicked();

    // ** Play States **
    void event_playButton_toggled(bool arg1);
    void event_pauseButton_toggled(bool arg1);
    void event_resetButton_clicked();


    // **** Component Widgets ****
    void event_transformWidget_anyValueChanged();
    void event_meshWidget_anyValueChanged();
    void event_audioWidget_anyValueChanged();
    void event_audioWidget_playSound();
    void event_lightWidget_anyValueChanged();

    void event_assetManagerWidget_addOrRemoveAsset(AssetType asset, ButtonPress button);
public:
    /**
     * Initializes the whole program.
     */
    void Init();

    Ui::MainWindow* ui_{nullptr};
    /**
     * Updates which camera you're viewing through.
     */
    void UpdateCameraSelection();
    /**
     * Finds every component the Ative Entity has, and shows them in the list.
     */
    void LoadComponentsToList();
    /**
     * Sets the docks hidden or unhidden, based on the parameter.
     * @param if true, the docks gets hidden.
     */
    void SetDocksHidden(bool arg);
    /**
     * Updates the status bar underneath the UI.
     * Currently sets the time between frames, the FPS, entites drawn and vertices drawn.
     */
    void UpdateStatusBar();
    QTreeWidget* entityTree_{nullptr};
    QWidget* renderWindowContainer_{nullptr};
    AssetManagerWidget* assetWidget{nullptr};

    //NEW WIDGETS
    TransformWidget* transformWidget_{nullptr};
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
private:
    size_t ID();

    //Creation of all UI objects.
    void CreateToolBar();
    void CreateDockWindows();
    void CreateWorldOutlinerDock();
    void CreateComponentsDock();
    void CreatePropertiesDock();
    void CreateAssetManagerDock();
    TransformWidget* CreateTransformWidget();
    MeshWidget* CreateMeshWidget();
    AudioWidget* CreateAudioWidget();
    LightWidget* CreateLightWidget();

    void OpenMaterialCreator(bool edit = false, size_t ID = 0);


    QAction* CreateAction(QString name, QKeySequence* shortcut = nullptr, bool checkable = true, bool checked = false);

    //Docks to keep the widgets in
    QDockWidget* worldOutlinerDock{nullptr};
    QDockWidget* componentBrowserDock{nullptr};
    QDockWidget* propertiesDock{nullptr};
    QDockWidget* assetManagerDock{nullptr};


    QString savePathLocation = "";

    RenderWindow* renderWindow_{nullptr}; // qWindow

    QListWidget* componentList_{nullptr};

    QStackedWidget* propertiesStack{nullptr};

    QComboBox* newItemDropDown_{nullptr};
    QComboBox* componentTypeDropDown_{nullptr};

    // user input
    QLineEdit* newEntityName_{nullptr};

    //Menu camera to hold each camera
    QMenu* Cameras{nullptr};

    // View Modes
    QAction* actionLit{nullptr};
    QAction* actionWireframe{nullptr};
    bool twoFacedAlreadyActivated{false};


    QLabel* timePerFrame_{nullptr};
    QLabel* verticesDrawn_{nullptr};
    QLabel* EntitiesDrawn_{nullptr};
    QLabel* FPS_{nullptr};


    MeshWidget* meshWidget_{nullptr};
    AudioWidget* audioWidget_{nullptr};
    LightWidget* lightWidget_{nullptr};
};

#endif // MAINWINDOW_H
