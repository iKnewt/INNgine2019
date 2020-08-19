#include "audiowidget.h"
#include "comboboxproperty.h"
#include "spinboxproperty.h"

#include <Managers/assetmanager.h>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <Systems/audiosystem.h>

AudioWidget::AudioWidget(QWidget *parent, Qt::WindowFlags f) : BaseWidget(parent,f)
{
    InitializeWidget();
}

void AudioWidget::InitializeWidget()
{
    QGroupBox* soundSelection = new QGroupBox("Sound Selection",this);
    QVBoxLayout* soundSelectionlayout = CreateDefaultVLayout();
    sound_ = new ComboBoxProperty("Sound",1,this);
    connect(sound_,&ComboBoxProperty::AnyValueChanged,this,&AudioWidget::event_anyPropertyChanged);
    soundSelectionlayout->addWidget(sound_);

    loop_ = new QCheckBox("Loop",this);
    loop_->setCheckable(true);
    connect(loop_,&QCheckBox::clicked,this,&AudioWidget::event_anyPropertyChanged);
    soundSelectionlayout->addWidget(loop_);
    soundSelection->setLayout(soundSelectionlayout);

    QGroupBox* settings = new QGroupBox("Settings",this);
    QVBoxLayout* settingsLayout = CreateDefaultVLayout();
    gain_ = new SpinBoxProperty("Gain",1,0,500,5,this);
    connect(gain_,&ComboBoxProperty::AnyValueChanged,this,&AudioWidget::event_anyPropertyChanged);
    settingsLayout->addWidget(gain_);

    maxDistance_ = new SpinBoxProperty("Max Distance",1,0,1000000,0,this);
    connect(maxDistance_,&ComboBoxProperty::AnyValueChanged,this,&AudioWidget::event_anyPropertyChanged);
    settingsLayout->addWidget(maxDistance_);

    playSound_ = new QPushButton("Test Sound",this);
    connect(playSound_,&QPushButton::clicked,this,&AudioWidget::event_playSoundButton_Pressed);

    settings->setLayout(settingsLayout);

    QVBoxLayout* masterLayout = CreateDefaultVLayout();
    masterLayout->addWidget(soundSelection);
    masterLayout->addWidget(settings);
    masterLayout->addWidget(playSound_);
    this->setLayout(masterLayout);

}

void AudioWidget::UpdateAudioWidget(std::shared_ptr<AudioComponent> audio)
{
    UpdateSoundBox(audio);

    loop_->setChecked(audio->isLooping_);
    gain_->SetSpinBoxValue(0,audio->gain_);
    maxDistance_->SetSpinBoxValue(0,audio->maxDistance_);
}

void AudioWidget::UpdateAudioComponent(std::shared_ptr<AudioComponent> audio, std::shared_ptr<AudioSystem> audioSystem)
{
    if(sound_->GetBoxIndexValue(0) != audio->soundID_)
        AssetManager::GetInstance()->audioManager_->SetSound(sound_->GetCurrentBoxText(0),audio);
    if(loop_->isChecked() != audio->isLooping_)
        audioSystem->SetSourceIsLooping(audio,loop_->isChecked());
    if(!(gain_->GetSpinBoxValue(0)-audio->gain_ < 0.01f && gain_->GetSpinBoxValue(0)-audio->gain_ > -0.01f))
        audioSystem->SetSourceGain(audio,gain_->GetSpinBoxValue(0));
    if(int(maxDistance_->GetSpinBoxValue(0)) != int(audio->maxDistance_))
        audioSystem->SetSourceMaxDistance(audio,maxDistance_->GetSpinBoxValue(0));
}

void AudioWidget::UpdateSoundBox(std::shared_ptr<AudioComponent> audio)
{
    sound_->ClearAllBoxes();

    for(unsigned int i = 0; i < AssetManager::GetInstance()->audioManager_->sounds_.size(); i++)
    {
        sound_->AddBoxValue(0,AssetManager::GetInstance()->audioManager_->sounds_[i]->name_);
        if(AssetManager::GetInstance()->audioManager_->sounds_[i]->buffer_ == audio->currentBuffer_)
            sound_->SetBoxCurrentIndex(0,i);
    }
}


void AudioWidget::event_playSoundButton_Pressed()
{
    emit playSound();
}
