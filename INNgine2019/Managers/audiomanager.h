#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "componentmanager.h"
struct WAV_t
{
    /// Size of the WAV file.
    uint32_t size_;
    /// Size of the fmt chunk.
    uint32_t chunkSize_;
    /// Sample rate of the audio.
    uint32_t sampleRate_;
    /// Average byte rate.
    uint32_t avgBytesPerSec_;
    /// Size of the data chunk.
    uint32_t dataSize_;
    /// Audio format.
    short formatType_;
    /// Number of audio channels.
    short channels_;
    /// Number of bytes per sample.
    short bytesPerSample_;
    /// Number of bits per sample.
    short bitsPerSample_;
    /// Buffer to hold audio data.
    unsigned char* buffer_;
    /// Structure Constructor. Initialises data buffer to NULL.
    WAV_t() : buffer_(nullptr) {}
};

/// Contains sound data.
struct Sound{
    QString name_;
    ALuint buffer_{0};
};

/// Keeps all the data and logic connected to Audio.
class AudioManager
{
public:
    AudioManager();

    /// Number of defaults, used to make sure default are not deleted or edited.
    const size_t numberOfDefaultSounds_{1};
    /// All sounds
    std::vector<std::shared_ptr<Sound>> sounds_;
    /**
     * Adds a sound from the Assets/Sounds Filepath.
     * @param fileName the specified audio in the Assets/Sounds Folder.
     */
    void AddSound(QString filePath);
    /**
     * Sets a sound of a specified Audio Component that is already loaded with the AddSound function.
     * @param soundName The specified audio file name (with the .wav extension).
     * @param component The specified audio component where the sound is added.
     */
    void SetSound(QString soundName, std::shared_ptr<AudioComponent> component);
    void SetSound(size_t index, std::shared_ptr<AudioComponent> component);

    /**
     * Deletes sound
     * @param soundID ID of sound to remove
     */
    void DeleteSound(unsigned int soundID);
    /**
     * Delets all sounds expect the defaults
     */
    void DeleteAllCustomSounds();

    /// Pointer to the ALC Device. Gets set automatically.
    ALCdevice* ALdevice_;
    /// Pointer to the ALC Context. Gets set automatically.
    ALCcontext* ALcontext_;
    ALuint GetNewBuffer();
    ALuint GetNewSource();
    ALuint buffers_{0};
    ALuint sources_{0};

private:
    /**
     * Error handling function.
     * @param errmsg Error message to output.
     * @return Always returns false, outputs argumen string to the command line.
     */
    static bool EndOnError(std::string errmsg);

    /**
     * Loads a given WAV file, fills WAV_t structure with WAV data.
     * @param filePath File path relative to execution directory.
     * @param WAVPtr Pointer to a WAV_t structure to contain the WAV data.
     * @return
     */
    static bool LoadWAV(std::string filePath, std::shared_ptr<WAV_t> WAVPtr);

};

#endif // AUDIOMANAGER_H
