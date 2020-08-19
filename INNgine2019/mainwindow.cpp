#include "Legacy/innpch.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSurfaceFormat>
#include <QDesktopWidget>

#include "renderwindow.h"
#include "QFileDialog"

#include <UI/materialcreator.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
    Init();
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete renderWindow_;
    delete ui_;
}

void MainWindow::Init()
{
    renderWindow_ = new RenderWindow(nullptr, this);

    //The OpenGL RenderWindow got made, so continuing the setup:
    //We put the RenderWindow inside a QWidget so we can put in into a
    //layout that is made in the .ui-file
    renderWindowContainer_ = QWidget::createWindowContainer(renderWindow_);
    ui_->OpenGLLayout->addWidget(renderWindowContainer_);

    //sets the keyboard input focus to the RenderWindow when program starts
    // - can be deleted, but then you have to click inside the renderwindow to get the focus
    renderWindowContainer_->setFocus();

    //Set size of program in % of available screen
    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);

    this->setWindowTitle("Alberto Engine");
    this->setWindowIcon(QIcon(QPixmap(QString(gsl::assetFilePath + "Icons/Alberto_Icon.png"))));

    timePerFrame_ = new QLabel(this);
    timePerFrame_->setAlignment(Qt::AlignCenter);
    timePerFrame_->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    verticesDrawn_ = new QLabel(this);
    verticesDrawn_->setAlignment(Qt::AlignCenter);
    verticesDrawn_->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    EntitiesDrawn_ = new QLabel(this);
    EntitiesDrawn_->setAlignment(Qt::AlignCenter);
    EntitiesDrawn_->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    FPS_ = new QLabel(this);
    FPS_->setAlignment(Qt::AlignCenter);
    FPS_->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);

    statusBar()->addWidget(timePerFrame_);
    statusBar()->addWidget(FPS_);
    statusBar()->addWidget(EntitiesDrawn_);
    statusBar()->addWidget(verticesDrawn_);

    CreateToolBar();
    CreateDockWindows();
}

void MainWindow::SetDocksHidden(bool arg)
{
    worldOutlinerDock->setHidden(arg);
    componentBrowserDock->setHidden(arg);
    propertiesDock->setHidden(arg);
    assetManagerDock->setHidden(arg);
}

void MainWindow::UpdateStatusBar()
{
    timePerFrame_->setText(" Time pr FrameDraw:" + QString::number(static_cast<int>(AssetManager::GetInstance()->deltaTime_)) + " ms");
    FPS_->setText("FPS (approximated): " + QString::number(static_cast<int>(1000/AssetManager::GetInstance()->deltaTime_)));
    EntitiesDrawn_->setText("Entities Drawn: " + QString::number(static_cast<int>(renderWindow_->renderSystem_.entitiesDrawn_)));
    verticesDrawn_->setText("Vertices Drawn: " + QString::number(static_cast<int>(renderWindow_->renderSystem_.verticesDrawn_)));
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        QString filePath =  event->mimeData()->urls().at(0).toString();
        QString filePathLower = filePath.toLower();
        filePath.remove("file:///");
        if(filePathLower.contains(".obj"))
            AssetManager::GetInstance()->meshManager_->AddMesh(FILE_MESH,filePath);
        else if(filePathLower.contains(".wav"))
            AssetManager::GetInstance()->audioManager_->AddSound(filePath);
        else if(filePathLower.contains(".bmp"))
            AssetManager::GetInstance()->textureManager_->AddTexture(filePath);
        else
            CreateMessageBox("Warning!","Filetype not recognized.");

        assetWidget->UpdateValues();
    }
}

void MainWindow::CreateToolBar()
{
    QToolBar* toolBar = new QToolBar(this);
    toolBar->setMovable(false);
    toolBar->setAcceptDrops(true);

    QAction* actionSave = CreateAction("Save",new QKeySequence(Qt::CTRL + Qt::Key_S),false,false);
    connect(actionSave,&QAction::triggered,this,&MainWindow::event_actionSave_triggered);

    QAction* actionSaveAs = CreateAction("Save As",new QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S),false,false);
    connect(actionSaveAs,&QAction::triggered,this,&MainWindow::event_actionSaveAs_triggered);

    QAction* actionLoad = CreateAction("Load",new QKeySequence(Qt::CTRL + Qt::Key_O),false,false);
    connect(actionLoad,&QAction::triggered,this,&MainWindow::event_actionLoad_triggered);

    QAction* actionLoadTrophiesFromScript = CreateAction("Load Trophies From Script", nullptr, false, false);
    connect(actionLoadTrophiesFromScript,&QAction::triggered,this,&MainWindow::event_actionLoadTrophiesFromScript_triggered);

    QAction* actionTwoFacedCulling = CreateAction("Two Faced Culling");
    connect(actionTwoFacedCulling,&QAction::toggled,this,&MainWindow::event_actionTwofacedCulling_toggled);

    QAction* actionMinecraftCursor = CreateAction("Minecraft Cursor", new QKeySequence(Qt::Key_M,Qt::Key_I,Qt::Key_N,Qt::Key_E));
    connect(actionMinecraftCursor,&QAction::toggled,this,&MainWindow::event_actionMinecraftMouse_triggered);

    QAction* actionComicSans = CreateAction("Comic Sans");
    actionComicSans->setCheckable(false);
    connect(actionComicSans,&QAction::triggered,this,&MainWindow::event_actionComicSans_triggered);

    QAction* actionLOD = CreateAction("Level of Detail",nullptr,true,true);
    connect(actionLOD,&QAction::toggled,this,&MainWindow::event_actionLOD_toggled);

    QAction* actionFrustumCulling = CreateAction("Frustum Culling",nullptr,true,true);
    connect(actionFrustumCulling,&QAction::toggled,this,&MainWindow::event_actionFrustumCulling_toggled);

    QAction* actionShowCollision = CreateAction("Show Collision");
    connect(actionShowCollision,&QAction::toggled,this,&MainWindow::event_actionShowCollision_toggled);

    QComboBox* viewMode = new QComboBox(this);
    viewMode->addItem("Lit");
    viewMode->addItem("Unlit");
    viewMode->addItem("Wireframe");
    viewMode->addItem("Normals");
    viewMode->addItem("UV_Maps");
    viewMode->addItem("Light Only");
    connect(viewMode,QOverload<int>::of(&QComboBox::activated),this,&MainWindow::event_actionViewMode_changed);

    QMenu* File = new QMenu(this);
    File->addAction(actionSave);
    File->addAction(actionSaveAs);
    File->addAction(actionLoad);
    File->addAction(actionLoadTrophiesFromScript);

    QToolButton* Filebutton = new QToolButton(toolBar);
    Filebutton->setText("File");
    Filebutton->setMenu(File);
    Filebutton->setPopupMode(QToolButton::InstantPopup);

    QMenu* Extras = new QMenu(this);
    Extras->addAction(actionMinecraftCursor);
    Extras->addAction(actionComicSans);

    QToolButton* Extrasbutton = new QToolButton(toolBar);
    Extrasbutton->setText("Extras");
    Extrasbutton->setMenu(Extras);
    Extrasbutton->setPopupMode(QToolButton::InstantPopup);

    QMenu* Settings = new QMenu(this);
    Settings->addAction(actionFrustumCulling);
    Settings->addAction(actionLOD);
    Settings->addAction(actionShowCollision);
    Settings->addAction(actionTwoFacedCulling);

    QToolButton* Settingsbutton = new QToolButton(toolBar);
    Settingsbutton->setText("Settings");
    Settingsbutton->setMenu(Settings);
    Settingsbutton->setPopupMode(QToolButton::InstantPopup);


    Cameras = new QMenu(this);
    connect(Cameras,&QMenu::triggered,this,&MainWindow::event_newCameraSelected);
    QToolButton* Camerasbutton = new QToolButton(toolBar);
    Camerasbutton->setText("Cameras");
    Camerasbutton->setMenu(Cameras);
    Camerasbutton->setPopupMode(QToolButton::InstantPopup);


    QPushButton* playButton = new QPushButton("Play/Stop");
    playButton->setCheckable(true);
    connect(playButton,&QAbstractButton::toggled,this,&MainWindow::event_playButton_toggled);
    QPushButton* pauseButton = new QPushButton("Pause");
    pauseButton->setCheckable(true);
    connect(pauseButton,&QAbstractButton::toggled,this,&MainWindow::event_pauseButton_toggled);
    QPushButton* resetButton = new QPushButton("Reset");
    connect(resetButton,&QAbstractButton::clicked,this,&MainWindow::event_resetButton_clicked);

    toolBar->addWidget(Filebutton);
    toolBar->addWidget(viewMode);
    toolBar->addWidget(Settingsbutton);
    toolBar->addWidget(Extrasbutton);
    toolBar->addWidget(Camerasbutton);

    toolBar->addWidget(playButton);
    toolBar->addWidget(pauseButton);
    toolBar->addWidget(resetButton);

    this->addToolBar(toolBar);
}

void MainWindow::event_entityTreeWidget_itemClicked(QTreeWidgetItem *item)
{
    size_t ID = item->text(1).toUInt();

    renderWindow_->sceneManager_->activeEntityID_ = ID;
    renderWindowContainer_->setFocus();
    LoadComponentsToList();
}

void MainWindow::event_addEntityButton_clicked()
{
    renderWindow_->sceneManager_->AddEntity(newEntityName_->text());
    newEntityName_->clear();
}

void MainWindow::event_removeEntityButton_clicked()
{
    renderWindow_->sceneManager_->DeleteEntity(ID());
}

void MainWindow::event_addComponentButton_clicked()
{
    renderWindow_->sceneManager_->componentManager_->AddComponent(static_cast<ComponentType>(componentTypeDropDown_->currentIndex()), renderWindow_->sceneManager_->activeEntityID_);
    LoadComponentsToList();
    renderWindow_->UpdateMovementSystem();
}

void MainWindow::event_deleteComponentButton_clicked()
{
    for(unsigned int i = 0; i < ComponentType::NONE;i++)
        if(componentList_->currentItem()->text() == COMPONENT_TYPE_NAMES[i])
        {
            renderWindow_->sceneManager_->componentManager_->DeleteComponentFromEntity(ID(),ComponentType(i));
            LoadComponentsToList();
            return;
        }
}

void MainWindow::event_landscapeEditor_clicked()
{
    LandscapeEditor* landscape = new LandscapeEditor(this);
    landscape->setModal(true);
    landscape->setAttribute( Qt::WA_DeleteOnClose );
    landscape->exec();
}

void MainWindow::event_actionTwofacedCulling_toggled(bool arg1)
{
    renderWindow_->toggleTwofacedCulling(arg1);
}

void MainWindow::event_actionLOD_toggled(bool arg1)
{
    renderWindow_->renderSystem_.useLOD_ = arg1;
}

void MainWindow::event_actionFrustumCulling_toggled(bool arg1)
{
    renderWindow_->renderSystem_.useFrustumCulling_ = arg1;
}

void MainWindow::event_actionShowCollision_toggled(bool arg1)
{
    renderWindow_->renderSystem_.showBoundingBoxes_ = arg1;
}

void MainWindow::CreateWorldOutlinerDock()
{

    worldOutlinerDock = new QDockWidget(tr("World Outliner"), this);
    QVBoxLayout* worldOutlinerLayout = new QVBoxLayout;

    entityTree_ = new QTreeWidget();
    entityTree_->setColumnCount(2);
    entityTree_->setAlternatingRowColors(true);
    entityTree_->setSortingEnabled(true);
    entityTree_->setDragDropMode(QAbstractItemView::DragDrop);
    entityTree_->setDefaultDropAction(Qt::DropAction::MoveAction);

    QList<QString> headers;
    headers.push_back("Entity Name");
    headers.push_back("ID");
    entityTree_->setHeaderLabels(headers);
    connect(entityTree_, &QTreeWidget::itemClicked, this, &MainWindow::event_entityTreeWidget_itemClicked);
    worldOutlinerLayout->addWidget(entityTree_);

    newEntityName_ = new QLineEdit();
    newEntityName_->setPlaceholderText("Name of New Entity");
    worldOutlinerLayout->addWidget(newEntityName_);

    QPushButton* addEntity = new QPushButton("Add Entity",this);
    connect(addEntity, SIGNAL (clicked()), this, SLOT (event_addEntityButton_clicked()));
    worldOutlinerLayout->addWidget(addEntity);

    QPushButton* removeEntity = new QPushButton("Remove Entity",this);
    connect(removeEntity,&QPushButton::clicked,this,&MainWindow::event_removeEntityButton_clicked);
    worldOutlinerLayout->addWidget(removeEntity);


    newItemDropDown_ = new QComboBox;
    newItemDropDown_->addItem("Empty Entity");
    worldOutlinerLayout->addWidget(newItemDropDown_);

    QPushButton* landscapeEditor = new QPushButton("Landscape Editor",this);
    connect(landscapeEditor,&QPushButton::clicked,this,&MainWindow::event_landscapeEditor_clicked);
    worldOutlinerLayout->addWidget(landscapeEditor);

    QWidget* worldOutliner = new QWidget;
    worldOutliner->setLayout(worldOutlinerLayout);

    worldOutlinerDock->setWidget(worldOutliner);
    addDockWidget(Qt::LeftDockWidgetArea, worldOutlinerDock);
}

void MainWindow::CreateComponentsDock()
{
    componentBrowserDock = new QDockWidget(tr("Components"), this);

    QVBoxLayout* componentBrowserLayout = new QVBoxLayout;

    componentList_ = new QListWidget();
    componentList_->setAlternatingRowColors(true);
    componentList_->setSortingEnabled(true);
    componentList_->setDefaultDropAction(Qt::DropAction::MoveAction);

    //    componentList->addItems(QStringList() << "Mesh Component" << "Transform Component");
    componentBrowserLayout->addWidget(componentList_);

    connect(componentList_, &QListWidget::itemClicked, this, &MainWindow::event_componentListItem_Clicked);

    QPushButton* addComponent = new QPushButton("Add Component");
    connect(addComponent, SIGNAL (clicked()), this, SLOT (event_addComponentButton_clicked()));
    componentBrowserLayout->addWidget(addComponent);
    QPushButton* removeComponent = new QPushButton("Delete Component");
    connect(removeComponent, SIGNAL (clicked()), this, SLOT (event_deleteComponentButton_clicked()));
    componentBrowserLayout->addWidget(removeComponent);

    componentTypeDropDown_ = new QComboBox;

    for (int componentType = 0; componentType < ComponentType::NONE; componentType++)
    {
        componentTypeDropDown_->addItem(COMPONENT_TYPE_NAMES[componentType]);
    }
    componentBrowserLayout->addWidget(componentTypeDropDown_);


    QWidget* componentBrowser = new QWidget;
    componentBrowser->setLayout(componentBrowserLayout);

    componentBrowserDock->setWidget(componentBrowser);
    addDockWidget(Qt::RightDockWidgetArea, componentBrowserDock);
}

TransformWidget* MainWindow::CreateTransformWidget()
{
    transformWidget_ = new TransformWidget(this);
    connect(transformWidget_,&TransformWidget::anyValueChanged,this,&MainWindow::event_transformWidget_anyValueChanged);
    return transformWidget_;
}

MeshWidget* MainWindow::CreateMeshWidget()
{
    meshWidget_ = new MeshWidget(this);
    connect(meshWidget_,&MeshWidget::anyValueChanged,this,&MainWindow::event_meshWidget_anyValueChanged);
    return meshWidget_;
}

AudioWidget *MainWindow::CreateAudioWidget()
{
    audioWidget_ = new AudioWidget(this);
    connect(audioWidget_,&AudioWidget::anyValueChanged,this,&MainWindow::event_audioWidget_anyValueChanged);
    connect(audioWidget_,&AudioWidget::playSound,this,&MainWindow::event_audioWidget_playSound);
    return audioWidget_;
}

LightWidget *MainWindow::CreateLightWidget()
{
    lightWidget_ = new LightWidget(this);
    connect(lightWidget_,&LightWidget::anyValueChanged,this,&MainWindow::event_lightWidget_anyValueChanged);
    return lightWidget_;
}

void MainWindow::OpenMaterialCreator(bool edit, size_t ID)
{
    qDebug()  << edit << ID;
    MaterialCreator* material;
    material = new MaterialCreator(edit,ID,this);
    material->setModal(true);
    material->setAttribute(Qt::WA_DeleteOnClose);
    material->exec();

}

QAction *MainWindow::CreateAction(QString name, QKeySequence *shortcut, bool checkable, bool checked)
{
    QAction* action = new QAction(name,this);
    action->setCheckable(checkable);
    action->setChecked(checked);
    if(shortcut)
        action->setShortcut(*shortcut);
    return action;
}

void MainWindow::CreatePropertiesDock()
{
    propertiesDock = new QDockWidget(tr("Properties"), this);
    propertiesDock->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum);
    //        propertiesDock->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    propertiesStack = new QStackedWidget;

    propertiesStack->addWidget(new QLabel("")); // 0
    propertiesStack->addWidget(CreateTransformWidget()); // 1
    propertiesStack->addWidget(CreateMeshWidget()); // 2
    propertiesStack->addWidget(CreateAudioWidget()); // 3
    propertiesStack->addWidget(CreateLightWidget()); // 4

    propertiesDock->setWidget(propertiesStack);

    addDockWidget(Qt::RightDockWidgetArea, propertiesDock);

    this->resizeDocks({propertiesDock}, {300}, Qt::Orientation::Horizontal);
}

void MainWindow::CreateAssetManagerDock()
{
    assetManagerDock = new QDockWidget("Asset Manager",this);
    assetWidget = new AssetManagerWidget(this);
    connect(assetWidget,&AssetManagerWidget::addOrRemoveObject,this,&MainWindow::event_assetManagerWidget_addOrRemoveAsset);
    assetManagerDock->setWidget(assetWidget);
    addDockWidget(Qt::BottomDockWidgetArea,assetManagerDock);
    assetManagerDock->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    //    assetManagerDock->setFloating(true);
}

void MainWindow::UpdateCameraSelection()
{
    Cameras->clear();
    for(size_t i = 0; i < renderWindow_->cameras_.size(); i++)
    {
        QAction* x = new QAction;
        x->setText(renderWindow_->cameras_[i]->name_);
        Cameras->addAction(x);
    }
}

size_t MainWindow::ID()
{
    return renderWindow_->sceneManager_->activeEntityID_;
}

void MainWindow::CreateDockWindows()
{
    CreateWorldOutlinerDock();
    CreateComponentsDock();
    CreatePropertiesDock();
    CreateAssetManagerDock();
}

void MainWindow::event_actionMinecraftMouse_triggered(bool checked)
{
    if(checked)
        qApp->setOverrideCursor(QCursor(QPixmap(QString(gsl::assetFilePath + "Icons/Alberto_MC_Cursor.png"))));
    else
        qApp->restoreOverrideCursor();
}

void MainWindow::event_actionComicSans_triggered(bool checked)
{
    if(checked)
        qApp->setFont(QFont("Comic Sans MS"));
    qApp->setFont(QFont("Sans Serif"));
    renderWindow_->HUDelement_->font_ = QFont("Comic Sans MS", 35);
}


void MainWindow::event_playButton_toggled(bool arg1)
{
    if(arg1)
        renderWindow_->Play();
    else
        renderWindow_->Stop();
}

void MainWindow::event_pauseButton_toggled(bool arg1)
{
    renderWindow_->Pause(arg1);
    qDebug() << arg1;
}

void MainWindow::event_resetButton_clicked()
{
    renderWindow_->sceneManager_->ResetScene();
}

void MainWindow::event_newCameraSelected(QAction *action)
{
    for(int i = 0; i < Cameras->actions().size(); i++)
        if(Cameras->actions()[i] == action)
            renderWindow_->SetActiveCamera(static_cast<size_t>(i));
}

void MainWindow::event_actionSave_triggered()
{
    if(savePathLocation == "")
    {
        savePathLocation = QFileDialog::getSaveFileName(this,
                                                        tr("Save Scene"), gsl::scriptFilePath,
                                                        tr("JSON(*.json) ;; DAT(*.dat)"));
    }
    if(!(savePathLocation == ""))
        renderWindow_->sceneManager_->SaveScene(savePathLocation);
}

void MainWindow::event_actionSaveAs_triggered()
{
    savePathLocation = QFileDialog::getSaveFileName(this,
                                                    tr("Save Scene"), gsl::scriptFilePath,
                                                    tr("JSON(*.json) ;; DAT(*.dat)"));
    if(!(savePathLocation == ""))
        renderWindow_->sceneManager_->SaveScene(savePathLocation);
}

void MainWindow::event_actionLoad_triggered()
{
    savePathLocation = QFileDialog::getOpenFileName(this,
                                                    tr("Load Scene"), gsl::scriptFilePath,
                                                    tr("JSON(*.json) ;; DAT(*.dat)"));
    if(!(savePathLocation == ""))
        renderWindow_->sceneManager_->LoadScene(savePathLocation);
    assetWidget->UpdateValues();
    renderWindow_->UpdateMovementSystem();
}

void MainWindow::event_actionLoadTrophiesFromScript_triggered()
{
    renderWindow_->sceneManager_->TrophiesFromScript();
    renderWindow_->UpdateMovementSystem();
}

void MainWindow::event_actionViewMode_changed(unsigned int index)
{
    renderWindow_->renderSystem_.SetRenderStyle(RenderStyle(index));
}

void MainWindow::event_componentListItem_Clicked(QListWidgetItem *item)
{
    qDebug() << "component clicked " << item->text();

    int index = 0;

    if(item->text() == COMPONENT_TYPE_NAMES[TRANSFORM])
    {
        index = 1;
        transformWidget_->UpdateTransformWidget(renderWindow_->sceneManager_->componentManager_->transformComponents_[ID()]);
    }
    else if (item->text() == COMPONENT_TYPE_NAMES[MESH]){
        index = 2;
        meshWidget_->UpdateMeshWidget(
                    renderWindow_->sceneManager_->componentManager_->meshComponents_[ID()]);
    }
    else if (item->text() == COMPONENT_TYPE_NAMES[AUDIO])
    {
        index = 3;
        audioWidget_->UpdateAudioWidget(renderWindow_->sceneManager_->componentManager_->audioComponents_[ID()]);
    }
    else if (item->text() == COMPONENT_TYPE_NAMES[LIGHT])
    {
        index = 4;
        lightWidget_->UpdateLightWidget(renderWindow_->sceneManager_->componentManager_->lightComponents_[ID()]);
    }
    else if (item->text() == COMPONENT_TYPE_NAMES[AI])
    {
        index = 0;
        //        lightWidget_->UpdateLightWidget(renderWindow_->sceneManager_->componentManager_->lightComponents_[ID()]);
    }

    qDebug() << "component index " << index;

    propertiesStack->setCurrentIndex(index);

}

void MainWindow::event_transformWidget_anyValueChanged()
{
    transformWidget_->UpdateTransformComponent(renderWindow_->sceneManager_->componentManager_->transformComponents_[ID()]);
    renderWindow_->movementSystem_.UpdateTransform(renderWindow_->sceneManager_->entityManager_,renderWindow_->sceneManager_->componentManager_->transformComponents_,ID());
}

void MainWindow::event_meshWidget_anyValueChanged()
{
    meshWidget_->UpdateMeshComponent(renderWindow_->sceneManager_->componentManager_->meshComponents_[ID()]);
    if(renderWindow_->sceneManager_->componentManager_->meshComponents_[ID()]->objectType_ == ObjectType::PLAYER)
        renderWindow_->sceneManager_->SetActiveEntityToPlayer();
}

void MainWindow::event_audioWidget_anyValueChanged()
{
    audioWidget_->UpdateAudioComponent(renderWindow_->sceneManager_->componentManager_->audioComponents_[ID()],renderWindow_->audioSystem_);
}

void MainWindow::event_audioWidget_playSound()
{
    renderWindow_->audioSystem_->Play(renderWindow_->sceneManager_->componentManager_->audioComponents_[ID()]);
}

void MainWindow::event_lightWidget_anyValueChanged()
{
    lightWidget_->UpdateLightComponent(renderWindow_->sceneManager_->componentManager_->lightComponents_[ID()]);
    renderWindow_->sceneManager_->componentManager_->UpdateShaderLightNumbers();
}

void MainWindow::event_assetManagerWidget_addOrRemoveAsset(AssetType asset, ButtonPress button)
{
    renderWindow_->makeCurrent();

    if(asset == ASSET_MESH)
    {
        if(button == ADD_ASSET)
        {
            QString filepath =QFileDialog::getOpenFileName(this,
                                                           tr("Load Mesh"), gsl::meshFilePath,
                                                           tr("OBJ (*.obj)"));
            if(!(filepath == ""))
                AssetManager::GetInstance()->meshManager_->AddMesh(FILE_MESH,filepath);
        }
        else
        {
            AssetManager::GetInstance()->DeleteMesh(assetWidget->meshes_->getCurrentSelectedIndex() +
                                                    AssetManager::GetInstance()->meshManager_->numberOfDefaultMeshes_,
                                                    renderWindow_->sceneManager_->componentManager_->meshComponents_);
            //            renderWindow_->sceneManager_->componentManager_->UpdateMeshID(assetWidget->meshes_->getCurrentSelectedIndex());

        }
    }
    if(asset == ASSET_SOUND)
    {
        if(button == ADD_ASSET)
        {
            QString filepath =QFileDialog::getOpenFileName(this,
                                                           tr("Load Sound"), gsl::soundFilePath,
                                                           tr("WAV (*.wav)"));
            if(!(filepath == ""))
                AssetManager::GetInstance()->audioManager_->AddSound(filepath);
        }
        else
        {
            //I have to unload the sound.
            renderWindow_->sceneManager_->componentManager_->UnloadAudioSound(assetWidget->sounds_->getCurrentSelectedIndex());
            AssetManager::GetInstance()->audioManager_->DeleteSound(assetWidget->sounds_->getCurrentSelectedIndex() +
                                                                    static_cast<unsigned int >(AssetManager::GetInstance()->audioManager_->numberOfDefaultSounds_));
            renderWindow_->sceneManager_->componentManager_->UpdateAudioID(assetWidget->sounds_->getCurrentSelectedIndex());
        }
    }
    if(asset == ASSET_TEXTURE)
    {
        if(button == ADD_ASSET)
        {
            QString filepath =QFileDialog::getOpenFileName(this,
                                                           tr("Load Texture"), gsl::textureFilePath,
                                                           tr("BMP (*.bmp)"));
            if(!(filepath == ""))
                AssetManager::GetInstance()->textureManager_->AddTexture(filepath);

        }
        else
        {
            AssetManager::GetInstance()->DeleteTexture(assetWidget->textures_->getCurrentSelectedIndex()+
                                                       static_cast<unsigned int >(AssetManager::GetInstance()->textureManager_->numberOfDefaultTextures_));
        }
    }
    if(asset == ASSET_MATERIAL)
    {
        if(button == ADD_ASSET)
            OpenMaterialCreator();

        else
        {
            if(button == EDIT_ASSET)
                OpenMaterialCreator(true,assetWidget->materials_->getCurrentSelectedIndex() + static_cast<unsigned int>(AssetManager::GetInstance()->materialManager_->numberOfDefaultMaterials_));
            else
                AssetManager::GetInstance()->DeleteMaterial(assetWidget->materials_->getCurrentSelectedIndex() +
                                                            static_cast<unsigned int >(AssetManager::GetInstance()->materialManager_->numberOfDefaultMaterials_),
                                                            renderWindow_->sceneManager_->componentManager_->meshComponents_);
        }
    }
    assetWidget->UpdateValues();
}

void MainWindow::LoadComponentsToList()
{
    componentList_->clear();

    if(renderWindow_->sceneManager_->componentManager_->transformComponents_[ID()])
        componentList_->addItem(COMPONENT_TYPE_NAMES[TRANSFORM]);
    if(renderWindow_->sceneManager_->componentManager_->meshComponents_[ID()])
        componentList_->addItem(COMPONENT_TYPE_NAMES[MESH]);
    if(renderWindow_->sceneManager_->componentManager_->audioComponents_[ID()])
        componentList_->addItem(COMPONENT_TYPE_NAMES[AUDIO]);
    if(renderWindow_->sceneManager_->componentManager_->lightComponents_[ID()])
        componentList_->addItem(COMPONENT_TYPE_NAMES[LIGHT]);
    if(renderWindow_->sceneManager_->componentManager_->aiComponents_[ID()])
        componentList_->addItem(COMPONENT_TYPE_NAMES[AI]);
}
