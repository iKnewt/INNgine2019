#include "Legacy/innpch.h"
#include "renderwindow.h"
#include <QTimer>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QStatusBar>
#include <chrono>
#include "mainwindow.h"
#include "GSL/bsplinecurve.h"

RenderWindow::~RenderWindow()
{
}

void RenderWindow::initializeGL()
{
    initializeOpenGLFunctions();

    setSurfaceType(QWindow::OpenGLSurface);

    //This will contain the setup of the OpenGL surface we will render into
    QSurfaceFormat format;
    //OpenGL v 4.1 - (Ole Flatens Mac does not support higher than this...)
    //you can try other versions, but then have to Update RenderWindow and Shader
    //to inherit from other than QOpenGLFunctions_4_1_Core
    format.setVersion(4, 1);
    //Using the main profile for OpenGL - no legacy code permitted
    format.setProfile(QSurfaceFormat::CoreProfile);
    //A QSurface can be other types that OpenGL
    format.setRenderableType(QSurfaceFormat::OpenGL);
    //This should activate OpenGL debug Context used in RenderWindow::startOpenGLDebugger().
    //This line (and the startOpenGLDebugger() and checkForGLerrors() in RenderWindow class)
    //can be deleted, but it is nice to have OpenGL debug info!
    format.setOption(QSurfaceFormat::DebugContext);
    // The renderer will need a depth buffer - (not requiered to set in glfw-tutorials)
    format.setDepthBufferSize(24);
    //Set the number of samples used for multisampling
    format.setSamples(8);
    //Turn off VSync. If this is set to 1, VSync is on - default behaviour
    format.setSwapInterval(0);
    qDebug() << "Requesting surface format: " << format;

    setFormat(format);
    context()->setFormat(requestedFormat());
    qDebug() << "Vendor: " << glGetString(GL_VENDOR);
    qDebug() << "Renderer: " << glGetString(GL_RENDERER);
    qDebug() << "Version: " << glGetString(GL_VERSION);

    //general OpenGL stuff:
    glEnable(GL_DEPTH_TEST);    //enables depth sorting - must use GL_DEPTH_BUFFER_BIT in glClear
    glEnable(GL_CULL_FACE);     //draws only front side of models - usually what you want -
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);    //color used in glClear GL_COLOR_BUFFER_BIT

    glEnable(GL_STENCIL_TEST); //enable
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

    // setting up assets
    AssetManager::GetInstance();

    // setting up initial scene
    sceneManager_ = std::make_shared<SceneManager>(mainWindow_->entityTree_);

    //********************** Set up camera **********************
    cameras_.push_back(std::make_shared<Camera>());
    cameras_.back()->SetPosition(gsl::Vector3D(0.f, 50.f, 50.f));
    cameras_.back()->pitch_ = -45.f;
    cameras_.back()->name_ = "Editor Camera";
    cameras_.push_back(std::make_shared<Camera>());
    cameras_.back()->SetPosition(gsl::Vector3D( 0 , 44.5268f , 34.5522f ));
    cameras_.back()->pitch_ = -55.1f;
    cameras_.back()->name_ = "Game Camera";
    cameras_.back()->movable_ = true;
    cameras_.back()->thirdPersonCamera_ = true;
    SetActiveCamera(0);

    //Setting up Audio
    audioSystem_ = std::make_shared<AudioSystem>();

    UpdateMovementSystem();
    sceneManager_->UpdateAIsBasedOnThropies();

    HUDelement_ = new HUDelement;
    mainWindow_->assetWidget->UpdateValues();

    //This is just to support modern screens with "double" pixels
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));
    aspectratio_ = static_cast<float>(width()) / height();
}

void RenderWindow::paintHUD()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_STENCIL_TEST);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setFont(HUDelement_->font_);
    painter.setPen(HUDelement_->pen_);

    painter.drawImage(QPoint(0, height() - HUDelement_->image_.height()), HUDelement_->image_);
    //        throphie counter in HUD
    if (sceneManager_->trophiesCounter_ > 0)
    {
        painter.drawText(QPoint(HUDelement_->image_.width() + 10, height() - 10), HUDelement_->textBeforeTrophieCount_ + QString::number(sceneManager_->trophiesCounter_));
        elapsedTime_ = gameTimer_.elapsed();
    }
    else
    {
        painter.drawText(QPoint(HUDelement_->image_.width() + 10, height() - 10), HUDelement_->textWhenWinning_);
    }
    painter.end();

    // timer in hud
    QPainter painterTimer(this);
    painterTimer.setFont(HUDelement_->font_);
    painterTimer.setPen(HUDelement_->pen_);

    painterTimer.drawText(QPoint(HUDelement_->image_.width() + 10, height() - 65), "Time: " + QString::number(static_cast<double>(elapsedTime_/1000.f)));
    painterTimer.end();
}

void RenderWindow::paintGL()
{
    //general OpenGL stuff:
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);    //color used in glClear GL_COLOR_BUFFER_BIT
    glEnable(GL_DEPTH_TEST);    //enables depth sorting - must use GL_DEPTH_BUFFER_BIT in glClear
    glEnable(GL_CULL_FACE);     //draws only front side of models - usually what you want -
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_STENCIL_TEST); //enable

    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    //to clear the screen for each redraw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //    //    std::setprecision(20);
    //    //calculate the time since last render-call
    //    //this should be the same as xxx in the mRenderTimer->start(xxx) set in RenderWindow::exposeEvent(...)
    auto start = std::chrono::high_resolution_clock::now();

    //input
    HandleInput();
    //Events
    if(!AssetManager::GetInstance()->events_.empty())
        HandleEvents();


    if(movementSystem_.update_)
    {
//        sceneManager_->componentManager_->transformComponents_[0]->rotation_relative_.y += 0.1f;
        UpdateMovementSystem();
    }


    UpdateCameras();

    glStencilMask(0x00);

    renderSystem_.Update(cameras_,
                         activeCameraID_,
                         sceneManager_->activeEntityID_,
                         sceneManager_->entityManager_,
                         sceneManager_->componentManager_->meshComponents_,
                         sceneManager_->componentManager_->transformComponents_,
                         sceneManager_->componentManager_->lightComponents_,
                         AssetManager::GetInstance()->shaderManager_);

    audioSystem_->Update(cameras_[activeCameraID_],
                         sceneManager_->componentManager_->transformComponents_,
                         sceneManager_->componentManager_->audioComponents_);

    //    //Calculate framerate before
    //    // checkForGLerrors() because that takes a long time
    //    // and before swapBuffers(), else it will show the vsync time
    CalculateFramerate();
    //    //using our expanded OpenGL debugger to check if everything is OK.
    //    //    checkForGLerrors();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> deltaTime  = end - start;
    while(deltaTime.count()* 1000 < 10)
    {
        end = std::chrono::high_resolution_clock::now();
        deltaTime  = end - start;
    }

    AssetManager::GetInstance()->deltaTime_ = deltaTime.count() * 1000;

    if(HUDelement_ && showHUD_)
        paintHUD();
    update();
}

void RenderWindow::resizeGL(int w, int h)
{
    QOpenGLWindow::resizeGL(w, h);
    aspectratio_ = static_cast<float>(w) / h;
}

void RenderWindow::toggleTwofacedCulling(bool TwoFacedCullingOn)
{
    if (TwoFacedCullingOn)
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);
    twoFacedCulling_ = TwoFacedCullingOn;
}

//The way this is set up is that we start the clock before doing the draw call,
//and check the time right after it is finished (done in the render function)
//This will approximate what framerate we COULD have.
//The actual frame rate on your monitor is limited by the vsync and is probably 60Hz
void RenderWindow::CalculateFramerate()
{
    static int frameCount{0};                       //counting actual frames for a quick "timer" for the statusbar

    if (mainWindow_)    //if no mainWindow, something is really wrong...
    {
        ++frameCount;
        if (frameCount > 60) //once pr 30 frames = update the message twice pr second (on a 60Hz monitor)
        {
            mainWindow_->UpdateStatusBar();
            frameCount = 0;     //reset to show a new message in 60 frames
        }
    }
}

/// Uses QOpenGLDebugLogger if this is present
/// Reverts to glGetError() if not
void RenderWindow::checkForGLerrors()
{
    if(openGLDebugLogger_)
    {
        const QList<QOpenGLDebugMessage> messages = openGLDebugLogger_->loggedMessages();
        for (const QOpenGLDebugMessage &message : messages)
            qDebug() << message;
    }
    else
    {
        GLenum err = GL_NO_ERROR;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            qDebug() << "glGetError returns " << err;
        }
    }
}

void RenderWindow::SetActiveCamera(size_t cameraID)
{
    activeCameraID_ = cameraID;
    AssetManager::GetInstance()->shaderManager_->SetActiveCamera(cameras_[activeCameraID_]);
    mainWindow_->UpdateCameraSelection();
    cameras_[activeCameraID_]->projectionMatrix_.perspective(45.f, aspectratio_, 1.f, 500.f);
}

void RenderWindow::Play()
{
    movementSystem_.update_ = true;
    renderSystem_.showSelection_ = false;
    sceneManager_->componentManager_->UpdateDefaultTransforms();
    sceneManager_->ResetScene();
    mainWindow_->SetDocksHidden(true);
    showHUD_ = true;
    SetActiveCamera(1);
    gameTimer_.start();
}

void RenderWindow::Stop()
{
    Pause(true);
    renderSystem_.showSelection_ = true;
    sceneManager_->ResetScene();
    UpdateMovementSystem();
    mainWindow_->SetDocksHidden(false);
    showHUD_ = false;
    SetActiveCamera(0);
}

void RenderWindow::Pause(bool arg1)
{
    if(arg1)
        movementSystem_.update_ = false;
    else
        movementSystem_.update_ = true;
}

/// Tries to start the extended OpenGL debugger that comes with Qt
void RenderWindow::StartOpenGLDebugger()
{
    QOpenGLContext * temp = this->context();
    if (temp)
    {
        QSurfaceFormat format = temp->format();
        if (! format.testOption(QSurfaceFormat::DebugContext))
            qDebug() << "This system can not use QOpenGLDebugLogger, so we revert to glGetError()";

        if(temp->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
        {
            qDebug() << "System can log OpenGL errors!";
            openGLDebugLogger_ = new QOpenGLDebugLogger(this);
            if (openGLDebugLogger_->initialize()) // initializes in the current context
                qDebug() << "Started OpenGL debug logger!";
        }

        if(openGLDebugLogger_)
            openGLDebugLogger_->disableMessages(QOpenGLDebugMessage::APISource, QOpenGLDebugMessage::OtherType, QOpenGLDebugMessage::NotificationSeverity);
    }
}

void RenderWindow::setCameraSpeed(float value)
{
    cameraSpeed_ += value;

    //Keep within min and max values
    if(cameraSpeed_ < 0.01f)
        cameraSpeed_ = 0.01f;
    if (cameraSpeed_ > 0.3f)
        cameraSpeed_ = 0.3f;
}

void RenderWindow::HandleInput()
{
    //Camera
    cameras_[activeCameraID_]->SetSpeed(0.f);  //cancel last frame movement
    if(input_.RMB && cameras_[activeCameraID_]->movable_ && !movementSystem_.update_)
    {
        if(input_.W)
            cameras_[activeCameraID_]->SetSpeed(-cameraSpeed_);
        if(input_.S)
            cameras_[activeCameraID_]->SetSpeed(cameraSpeed_);
        if(input_.D)
            cameras_[activeCameraID_]->MoveRight(cameraSpeed_);
        if(input_.A)
            cameras_[activeCameraID_]->MoveRight(-cameraSpeed_);
        if(input_.Q)
            cameras_[activeCameraID_]->UpdateHeight(-cameraSpeed_);
        if(input_.E)
            cameras_[activeCameraID_]->UpdateHeight(cameraSpeed_);
    }
    else if (sceneManager_->activeEntityID_ < sceneManager_->componentManager_->meshComponents_.size() && sceneManager_->componentManager_->transformComponents_[sceneManager_->activeEntityID_])
    {

        gsl::Vector3D forward = cameras_[activeCameraID_]->forwardXZ_;
        gsl::Vector3D right = cameras_[activeCameraID_]->rightXZ_;
        int moveforward = 0;
        int moveright = 0;
        int moveup = 0;
        if(input_.W) // +z
            moveforward = 1;
        if(input_.S) // -z
            moveforward = -1;
        if(input_.D) // +
            moveright = 1;
        if(input_.A) // - x
            moveright = -1;
        if(input_.Q) // + y
            moveup = 1;
        if(input_.E) // - y
            moveup =-1;
        if(moveright != 0 || moveforward != 0 || moveup != 0)
        {
            if(movementSystem_.update_)
                movementSystem_.AddMovement(sceneManager_->playerEntityID_, ((right * moveright) +  (forward*moveforward)) * 0.015f * AssetManager::GetInstance()->deltaTime_);
            else
            {
                sceneManager_->componentManager_->transformComponents_[sceneManager_->activeEntityID_]->position_relative_ += ((gsl::Vector3D(1,0,0) * moveright) + (gsl::Vector3D(0,0,-1)*moveforward) + (gsl::Vector3D(0,1,0) * moveup)) * 0.015f * AssetManager::GetInstance()->deltaTime_;
                if(sceneManager_->componentManager_->transformComponents_[sceneManager_->activeEntityID_]->followLandscape_)
                    sceneManager_->componentManager_->transformComponents_[sceneManager_->activeEntityID_]->position_relative_.y =
                            movementSystem_.FindLandscapeYOnLocation(AssetManager::GetInstance()->landscape_, sceneManager_->componentManager_->transformComponents_[sceneManager_->activeEntityID_]->position_relative_);
                UpdateActiveEntityTransform();
            }
        }
    }
    if(input_.F)
    {
        if(sceneManager_->componentManager_->transformComponents_[sceneManager_->activeEntityID_])
            cameras_[activeCameraID_]->FocusOnLocation(sceneManager_->componentManager_->transformComponents_[sceneManager_->activeEntityID_]->position_world_);
        else
            qDebug() << "ERROR: nothing to focus on!";
    }
}

void RenderWindow::HandleEvents()
{
    std::vector<unsigned int> events = AssetManager::GetInstance()->events_;
    AssetManager::GetInstance()->events_.clear();
    for(unsigned int eventID : events)
    {
        qDebug() << "Event: " << eventID;
        if (eventID == 0) // death
        {
            sceneManager_->ResetScene();
            sceneManager_->roundsCleared_ = 0;
            gameTimer_.start();
        }
        // if the event is on a trophy, remove trophy and control point from each enemy
        else if (sceneManager_->componentManager_->meshComponents_[eventID]->objectType_ == TROPHY)
        {
            sceneManager_->componentManager_->meshComponents_[eventID]->objectType_ = TAKEN_TROPHY;
            sceneManager_->UpdateAIsBasedOnThropies();
            movementSystem_.caughtTrophies_.push_back(eventID);
            if(sceneManager_->componentManager_->audioComponents_[eventID])
                audioSystem_->Play(sceneManager_->componentManager_->audioComponents_[eventID]);

        }
    }
}

void RenderWindow::UpdateCameras()
{
    gsl::Matrix4x4 newprojection;
    newprojection.perspective(60.f, aspectratio_, 1.f, 500.f);
    for(size_t i = 0; i < cameras_.size(); i++)
    {
        cameras_[i]->projectionMatrix_ = newprojection;
        if(cameras_[i]->thirdPersonCamera_)
        {
            if(!sceneManager_->componentManager_->transformComponents_[sceneManager_->playerEntityID_])
                continue;
            cameras_[i]->Update(sceneManager_->componentManager_->transformComponents_[sceneManager_->playerEntityID_]);
        }
        else
            cameras_[i]->Update();
        if(i == activeCameraID_)
            continue;
        movementSystem_.UpdateTransformMatrix(cameras_[i]->CameraTransform_);
    }
}

void RenderWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) //Shuts down whole program
    {
        mainWindow_->close();
    }

    //    You get the keyboard input like this
    if(event->key() == Qt::Key_W)
    {
        input_.W = true;
    }
    if(event->key() == Qt::Key_S)
    {
        input_.S = true;
    }
    if(event->key() == Qt::Key_D)
    {
        input_.D = true;
    }
    if(event->key() == Qt::Key_A)
    {
        input_.A = true;
    }
    if(event->key() == Qt::Key_Q)
    {
        input_.Q = true;
    }
    if(event->key() == Qt::Key_E)
    {
        input_.E = true;
    }
    if(event->key() == Qt::Key_Z)
    {
        input_.Z = true;
    }
    if(event->key() == Qt::Key_X)
    {
        input_.X = true;
    }
    if(event->key() == Qt::Key_C)
    {
        input_.C = true;
    }
    if(event->key() == Qt::Key_Up)
    {
        input_.UP = true;
    }
    if(event->key() == Qt::Key_Down)
    {
        input_.DOWN = true;
    }
    if(event->key() == Qt::Key_Left)
    {
        input_.LEFT = true;
    }
    if(event->key() == Qt::Key_Right)
    {
        input_.RIGHT = true;
    }
    if(event->key() == Qt::Key_U)
    {
        glDisable(GL_CULL_FACE);
    }
    if(event->key() == Qt::Key_O)
    {
        glEnable(GL_CULL_FACE);
    }
    if(event->key() == Qt::Key_Space)
    {
        input_.SPACE = true;
    }
    if(event->key() == Qt::Key_F)
    {
        input_.F = true;
    }
}

void RenderWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W)
    {
        input_.W = false;
    }
    if(event->key() == Qt::Key_S)
    {
        input_.S = false;
    }
    if(event->key() == Qt::Key_D)
    {
        input_.D = false;
    }
    if(event->key() == Qt::Key_A)
    {
        input_.A = false;
    }
    if(event->key() == Qt::Key_Q)
    {
        input_.Q = false;
    }
    if(event->key() == Qt::Key_E)
    {
        input_.E = false;
    }
    if(event->key() == Qt::Key_Z)
    {
        input_.Z = false;
    }
    if(event->key() == Qt::Key_X)
    {
        input_.X = false;
    }
    if(event->key() == Qt::Key_C)
    {
        input_.C = false;
    }
    if(event->key() == Qt::Key_Up)
    {
        input_.UP = false;
    }
    if(event->key() == Qt::Key_Down)
    {
        input_.DOWN = false;
    }
    if(event->key() == Qt::Key_Left)
    {
        input_.LEFT = false;
    }
    if(event->key() == Qt::Key_Right)
    {
        input_.RIGHT = false;
    }
    if(event->key() == Qt::Key_U)
    {
    }
    if(event->key() == Qt::Key_O)
    {
    }
    if(event->key() == Qt::Key_Space)
    {
        input_.SPACE = false;
    }
    if(event->key() == Qt::Key_F)
    {
        input_.F = false;
    }

}

void RenderWindow::MousePicking(QMouseEvent* event)
{
    // normalize x pos, normalize and flip y pos
    float x = (2.0f * event->pos().x()) / width() - 1.0f;
    float y = 1.0f - (2.0f * event->pos().y()) / height();
    float z = 1.0f;
    gsl::Vector3D ray_nds = gsl::Vector3D(x, y, z);

    // inverse z to match the coordinate system of openGL, convert to Vec4
    gsl::Vector4D ray_clip = gsl::Vector4D(ray_nds.x, ray_nds.y, -1.0f, 1.0f);

    // workaround to get the inverse pMatrix, as the gsl has no inversed()
    gsl::Matrix4x4 inverseProjectionMatrix = cameras_[activeCameraID_]->projectionMatrix_;
    inverseProjectionMatrix.inverse();
    // get into clip space from eye space
    gsl::Vector4D ray_eye = inverseProjectionMatrix * ray_clip;
    // un-project the x,y part
    ray_eye = gsl::Vector4D(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

    gsl::Matrix4x4 inverseViewMatrix = cameras_[activeCameraID_]->viewMatrix_;
    inverseViewMatrix.inverse();

    gsl::Vector3D ray_world = (inverseViewMatrix * ray_eye).toVector3D();
    ray_world.normalize();

    for (size_t i = 0; i < sceneManager_->componentManager_->transformComponents_.size(); i++)
    {

        if(!sceneManager_->componentManager_->transformComponents_[i])
            continue;

        float radius = 1;

        // b = D . (O - C)
        float b = gsl::Vector3D::dot(ray_world, cameras_[activeCameraID_]->position_ - sceneManager_->componentManager_->transformComponents_[i]->position_world_);
        // c = (O - C) . (O - C) - r^2
        float c = gsl::Vector3D::dot(cameras_[activeCameraID_]->position_ - sceneManager_->componentManager_->transformComponents_[i]->position_world_, cameras_[activeCameraID_]->position_ - sceneManager_->componentManager_->transformComponents_[i]->position_world_) - std::sqrt(radius);

        float test = static_cast<float>(std::pow(b, 2)) - c;

        if(test > 0)
        {
            sceneManager_->activeEntityID_ = i;
            sceneManager_->entityManager_->SelectEntityInTree(sceneManager_->activeEntityID_);
            mainWindow_->renderWindowContainer_->setFocus();
            mainWindow_->LoadComponentsToList();
        }
    }
}

void RenderWindow::UpdateMovementSystem()
{
    movementSystem_.Update(sceneManager_->entityManager_,
                           sceneManager_->componentManager_->transformComponents_,
                           sceneManager_->componentManager_->meshComponents_,
                           sceneManager_->componentManager_->aiComponents_,
                           AssetManager::GetInstance()->landscape_,
                           sceneManager_->componentManager_->lightComponents_,
                           sceneManager_->playerEntityID_);
}

void RenderWindow::UpdateActiveEntityTransform()
{
    movementSystem_.UpdateTransform(sceneManager_->entityManager_,
                                    sceneManager_->componentManager_->transformComponents_,
                                    sceneManager_->activeEntityID_);
    mainWindow_->transformWidget_->UpdateTransformWidget(sceneManager_->componentManager_->transformComponents_[sceneManager_->activeEntityID_]);
}

void RenderWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        input_.RMB = true;
    if (event->button() == Qt::LeftButton)
        input_.LMB = true;
    if (event->button() == Qt::MiddleButton)
        input_.MMB = true;
    if (event->button() == Qt::LeftButton)
    {
        MousePicking(event);
    }

}

void RenderWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        input_.RMB = false;
    if (event->button() == Qt::LeftButton)
        input_.LMB = false;
    if (event->button() == Qt::MiddleButton)
        input_.MMB = false;
}

void RenderWindow::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;

    //if RMB, change the speed of the camera
    if (input_.RMB)
    {
        if (numDegrees.y() < 1)
            setCameraSpeed(0.001f);
        if (numDegrees.y() > 1)
            setCameraSpeed(-0.001f);
    }
    event->accept();
}

void RenderWindow::mouseMoveEvent(QMouseEvent *event)
{
    mouseXlast_ = event->pos().x() - mouseXlast_;
    mouseYlast_ = event->pos().y() - mouseYlast_;
    if (input_.RMB && cameras_[activeCameraID_]->movable_)
    {
        //Using mMouseXYlast as deltaXY so we don't need extra variables

        if (mouseXlast_ != 0)
            cameras_[activeCameraID_]->Yaw(cameraRotateSpeed_ * mouseXlast_);
        if (mouseYlast_ != 0)
            cameras_[activeCameraID_]->Pitch(cameraRotateSpeed_ * mouseYlast_);
    }
    if(sceneManager_->componentManager_->transformComponents_[sceneManager_->activeEntityID_])
    {
        bool changed = false;
        if(input_.Z)
        {
            sceneManager_->componentManager_->transformComponents_[sceneManager_->activeEntityID_]->rotation_relative_.z += static_cast<float>(mouseXlast_);
            changed = true;
        }
        if(input_.X)
        {
            sceneManager_->componentManager_->transformComponents_[sceneManager_->activeEntityID_]->rotation_relative_.x += static_cast<float>(mouseXlast_);
            changed = true;
        }
        if(input_.C)
        {
            sceneManager_->componentManager_->transformComponents_[sceneManager_->activeEntityID_]->rotation_relative_.y += static_cast<float>(mouseXlast_);
            changed = true;
        }
        if(changed)
            UpdateActiveEntityTransform();
    }
    mouseXlast_ = event->pos().x();
    mouseYlast_ = event->pos().y();
}
