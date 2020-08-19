#ifndef AUDIOWIDGET_H
#define AUDIOWIDGET_H

#include "basewidget.h"

class QCheckBox;
class SpinBoxProperty;
class ComboBoxProperty;
class AudioSystem;
class QPushButton;

///The AudioWidget shows all information from the AudioComponent that the user can modify in runtime.
class AudioWidget : public BaseWidget
{
public:
    Q_OBJECT
public:
    AudioWidget(QWidget *parent, Qt::WindowFlags f = Qt::WindowFlags());

    /**
     *Initializes the widget and creates all subwidgets needed.
     */
    void InitializeWidget() override;


    ///ComboBoxProperty containing all sounds loaded into the game.
    ComboBoxProperty* sound_{nullptr};
    ///Checkbox to specify the active AudioComponent's looping setting.
    QCheckBox* loop_{nullptr};
    ///SpinBoxProperty to specify the active AudioComponent's gain.
    SpinBoxProperty* gain_{nullptr};
    ///SpinBoxProperty to specify the active AudioComponent's max distance.
    SpinBoxProperty* maxDistance_{nullptr};
    ///A button to test the sound of the active AudioComponent.
    QPushButton* playSound_{nullptr};

    /**
     * Updates the values from the AudioComponent to the Widget.
     * @param audio the Active Entity's AudioComponent.
     * @param audioSystem the Audio System.
     */
    void UpdateAudioWidget(std::shared_ptr<AudioComponent> audio);
    /**
     * Updates the values from the Widget to the AudioComponent.
     * @param audio the Active Entity's AudioComponent.
     * @param audioSystem the Audio System.
     */
    void UpdateAudioComponent(std::shared_ptr<AudioComponent> audio, std::shared_ptr<AudioSystem> audioSystem);
    /**
     * Updates all sounds loaded into the system and creates a menu item in the sounds_ ComboBoxProperty.
     * @param audioSystem the Audio System.
     */
    void UpdateSoundBox(std::shared_ptr<AudioComponent> audio);


public slots:
    /**
     * Runs whenever playSound_ PushButton is pressed.
     */
    void event_playSoundButton_Pressed();
signals:
    /**
     *Sends a signal to MainWindow indicating that the playSound_ PushButton has been pressed.
     */
    void playSound();
};

#endif // AUDIOWIDGET_H
