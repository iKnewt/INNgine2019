#include "audiosystem.h"
#include "Legacy/camera.h"
#include "GSL/matrix4x4.h"
#include "Managers/componentmanager.h"

#include <Managers/assetmanager.h>
AudioSystem::AudioSystem()
{

}

void AudioSystem::InitializeAudioComponent(std::shared_ptr<AudioComponent> audio)
{
    alcMakeContextCurrent(AssetManager::GetInstance()->audioManager_->ALcontext_);
    audio->source_ = AssetManager::GetInstance()->audioManager_->GetNewSource();
    alGenSources(1,&audio->source_);
    alSourcef(audio->source_, AL_PITCH, 1.0f);
    alSourcef(audio->source_, AL_GAIN, 1);
    ALfloat temp[3] = {0,0,0};
    alSourcefv(audio->source_, AL_POSITION, temp);
    ALfloat temp2[3] = {0,0,0};
    alSourcefv(audio->source_, AL_VELOCITY, temp2);
    alSourcef(audio->source_, AL_MAX_DISTANCE, audio->maxDistance_);
    alSourcei(audio->source_, AL_LOOPING, audio->isLooping_);
    alSourcei(audio->source_, AL_BUFFER, static_cast<ALint>(AssetManager::GetInstance()->audioManager_->sounds_[audio->soundID_]->buffer_));
    audio->initialized_ = true;
}
void AudioSystem::Update(std::shared_ptr<Camera> camera_, std::vector<std::shared_ptr<TransformComponent>> transformComponents, std::vector<std::shared_ptr<AudioComponent>> audioComponents)
{

    alcMakeContextCurrent(AssetManager::GetInstance()->audioManager_->ALcontext_);
    ALfloat posVec[3];
    ALfloat velVec[3];
    ALfloat headVec[6];

    gsl::Matrix4x4 temp = camera_->viewMatrix_;
    gsl::Vector3D pos = camera_->position();
    gsl::Vector3D up(temp.getFloat(4),temp.getFloat(5),temp.getFloat(6));
    gsl::Vector3D front(-temp.getFloat(8),-temp.getFloat(9),-temp.getFloat(10));

    posVec[0] = pos.x;
    posVec[1] = pos.y;
    posVec[2] = pos.z;
    velVec[0] = 0;
    velVec[1] = 0;
    velVec[2] = 0;
    headVec[0] = front.x;
    headVec[1] = front.y;
    headVec[2] = front.z;
    headVec[3] = up.x;
    headVec[4] = up.y;
    headVec[5] = up.z;
    alListenerfv(AL_POSITION, posVec);
    alListenerfv(AL_VELOCITY, velVec);
    alListenerfv(AL_ORIENTATION, headVec);

    for(size_t i = 0; i < audioComponents.size(); i++)
    {
        if(!audioComponents[i])
            continue;
        if(!audioComponents[i]->initialized_)
            InitializeAudioComponent(audioComponents[i]);
        if(transformComponents[i])
            SetPosition(audioComponents[i],transformComponents[i]->position_world_);
        else
            SetPosition(audioComponents[i],pos);
    }
}

void AudioSystem::Play(std::shared_ptr<AudioComponent> component)
{
    alcMakeContextCurrent(AssetManager::GetInstance()->audioManager_->ALcontext_);
    alSourcePlay(component->source_);
}

void AudioSystem::Pause(std::shared_ptr<AudioComponent> component)
{
    alcMakeContextCurrent(AssetManager::GetInstance()->audioManager_->ALcontext_);
    alSourcePause(component->source_);
}

void AudioSystem::Stop(std::shared_ptr<AudioComponent> component)
{
    alcMakeContextCurrent(AssetManager::GetInstance()->audioManager_->ALcontext_);
    alSourceStop(component->source_);
}

void AudioSystem::SetPosition(std::shared_ptr<AudioComponent> component, gsl::Vector3D newPosition)
{
    alcMakeContextCurrent(AssetManager::GetInstance()->audioManager_->ALcontext_);
    component->position_ = newPosition;
    ALfloat temp[3] = {component->position_.x, component->position_.y, component->position_.z};
    alSourcefv(component->source_, AL_POSITION, temp);
}

void AudioSystem::SetSourceGain(std::shared_ptr<AudioComponent> component, ALfloat gain)
{
    alcMakeContextCurrent(AssetManager::GetInstance()->audioManager_->ALcontext_);
    component->gain_ = gain;
    alSourcef(component->source_, AL_GAIN, gain);
}

void AudioSystem::SetSourceIsLooping(std::shared_ptr<AudioComponent> component, bool arg)
{
    alcMakeContextCurrent(AssetManager::GetInstance()->audioManager_->ALcontext_);
    component->isLooping_ = arg;
    alSourcei(component->source_,AL_LOOPING,arg);
}

void AudioSystem::SetSourceMaxDistance(std::shared_ptr<AudioComponent> component, ALfloat distance)
{
    alcMakeContextCurrent(AssetManager::GetInstance()->audioManager_->ALcontext_);
    component->maxDistance_ = distance;
    alSourcef(component->source_, AL_MAX_DISTANCE, component->maxDistance_);
}
