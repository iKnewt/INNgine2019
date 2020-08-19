#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <chrono>
#include <QFileInfo>
#include <QtGui>
#include <QTreeWidget>
#include <QListWidget>

#include "Legacy/camera.h"
#include "Legacy/input.h"

#include "Managers/assetmanager.h"
#include "Managers/scenemanager.h"

#include "Systems/rendersystem.h"
#include "Systems/audiosystem.h"
#include "Systems/movementsystem.h"

class QOpenGLContext;
class MainWindow;

/// Contains information on what to render in HUD.
struct HUDelement
{
    /// Color to draw text in.
    QPen pen_{Qt::white};
    /// Image to display.
    QImage image_{gsl::assetFilePath + "Icons/Alberto_Icon.png"};
    /// Font to use.
    QFont font_{"Sans Serif", 35};
    /// Text to use before remaining trophy counter.
    QString textBeforeTrophieCount_{"Cows left: "};
    /// Text to use when getting all trophies.
    QString textWhenWinning_{"You got all the cows!"};
};

/// This inherits from QWindow to get access to the Qt functionality and
/// OpenGL surface.
/// We also inherit from QOpenGLFunctions, to get access to the OpenGL functions
/// This is the same as using glad and glw from general OpenGL tutorials
class RenderWindow : public QOpenGLWindow,
        protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT

public:
    RenderWindow(QWindow* parent = nullptr, MainWindow* mainWindow = nullptr) :
        QOpenGLWindow(PartialUpdateBlit, parent), mainWindow_(mainWindow) {}
    ~RenderWindow() override;

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void toggleTwofacedCulling(bool TwoFacedCullingOn);
    void toggleBaryc(bool arg1);

    std::vector<std::shared_ptr<Camera>> cameras_;
    size_t activeCameraID_{0};
    void SetActiveCamera(size_t cameraID);

    /// Contains the scene with entities and components.
    std::shared_ptr<SceneManager> sceneManager_;

    // SYSTEMS
    /// Used to render scene.
    RenderSystem renderSystem_;
    /// Used to update movement and collision in scene.
    MovementSystem movementSystem_;
    /// Used to play audio in scene.
    std::shared_ptr<AudioSystem> audioSystem_{nullptr};

    /// Whether two faced culling is used.
    bool twoFacedCulling_{false};

    /**
    * Starts game.
    */
    void Play();
    /**
    *
    /// Pauses and unpauses game.
    * @param arg1 true to pause, false to unpause.
    */
    void Pause(bool arg1);
    /**
    * Stops game and resets scene.
    */
    void Stop();
    /**
     * Udates movement system one tick.
     * Often used to update transforms after a change has been made in editor.
     */
    void UpdateMovementSystem();
    /**
     * Updates only the transform of the active entity.
     * Usually onlt the active entity is being moved around the scene.
     */
    void UpdateActiveEntityTransform();

    /// Information on what to render in HUD.
    HUDelement* HUDelement_;
private:
    /// Whether to show the HUD on screen.
    bool showHUD_{false};
    /**
     * Paints the HUD on screen using QPainter.
     */
    void paintHUD();

    /**
     * Detects what entity the user is clicking with the mouse.
     * @param event Used to get the mouse cursors position on screen.
     */
    void MousePicking(QMouseEvent* event);
    void checkForGLerrors();

    float aspectratio_{1.f};

    MainWindow* mainWindow_{nullptr};    //points back to MainWindow to be able to put info in StatusBar
    QTimer* renderTimer_{nullptr};  //timer that drives the gameloop
    class QOpenGLDebugLogger* openGLDebugLogger_{nullptr};

    void setCameraSpeed(float value);
    void CalculateFramerate();
    void StartOpenGLDebugger();
    void HandleInput();
    void HandleEvents();
    void UpdateCameras();

    /// Timer used to track elapsed time during playing.
    QTime gameTimer_;
    /// Time elapsed while game is playing, used to display time in HUD.
    int elapsedTime_{0};

    Input input_;
    float cameraSpeed_{0.1f};
    float cameraRotateSpeed_{0.1f};
    int mouseXlast_{0};
    int mouseYlast_{0};

protected:
    //The QWindow that we inherit from has these functions to capture
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};

#endif // RENDERWINDOW_H
