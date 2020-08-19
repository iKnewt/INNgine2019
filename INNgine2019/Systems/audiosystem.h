#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <vector>
#ifdef _WIN32
#include <al.h>
#include <alc.h>
#endif
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif
class AudioComponent;
class Camera;
class TransformComponent;

/// Contains functionality to how to play audio in scene.
class AudioSystem
{
public:
    AudioSystem();

    void InitializeAudioComponent(std::shared_ptr<AudioComponent> audio);

    /**
     * Updates the audio sources positions and the camera's position.
     * @param camera_ To access the active camera (the listener).
     * @param transformComponents To get the position of the entity.
     * @param audioComponents To set the position of the source.
     */
    void Update(std::shared_ptr<Camera> camera_, std::vector<std::shared_ptr<TransformComponent>> transformComponents, std::vector<std::shared_ptr<AudioComponent> > audioComponents);
    /**
     * Plays a specified component's audio.
     * Needs a sound set to work.
     * @param component To get the data needed for the specified entity.
     */
    void Play(std::shared_ptr<AudioComponent> component);
    /**
     * Pauses a specified component's audio.
     * @param component To get the data needed for the specified entity.
     */
    void Pause(std::shared_ptr<AudioComponent> component);
    /**
     * Stops a specified component's audio.
     * @param component To get the data needed for the specified entity.
     */
    void Stop(std::shared_ptr<AudioComponent> component);

    /**
     * Sets the Audio Component's position.
     * @param component To get the data needed for the specified entity.
     * @param newPosition The new position for the specified Audio Component.
     */
    void SetPosition(std::shared_ptr<AudioComponent> component, gsl::Vector3D newPosition);
    /**
     * Sets the specified AudioComponent's gain. Needs to be done through this function.
     * @param component To get the data needed for the specified entity.
     * @param gain The new gain value for the specified Audio Component.
     */
    void SetSourceGain(std::shared_ptr<AudioComponent> component, ALfloat gain);
    /**
     * Sets the specified AudioComponent's IsLooping bool. Needs to be done through this function.
     * @param component To get the data needed for the specified entity.
     * @param arg The new statement for the specified Audio Component.
     */
    void SetSourceIsLooping(std::shared_ptr<AudioComponent> component, bool arg);
    /**
     * Sets the Max Distance value on the specified AudioComponent.
     * @param component To get the data needed for the specified entity.
     * @param distance The new MaxDistance value. The higher the number, the closer you have to be.
     */
    void SetSourceMaxDistance(std::shared_ptr<AudioComponent> component, ALfloat distance);

    ///The Sound Container. Contains a QString name and the WAV_t sound information. Use AddSound and SetSound to add/get any information from here.
    //    std::vector<std::pair<QString, std::shared_ptr<WAV_t>>> sounds_;
private:





};

#endif // AUDIOSYSTEM_H
