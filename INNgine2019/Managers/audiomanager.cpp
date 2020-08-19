#include "audiomanager.h"
#include <QFile>

AudioManager::AudioManager()
{
    //OpenAL
    qDebug() << "\n\nINITIALIZING AUDIO MANAGER";
    qDebug() << "Intializing OpenAL";
    ALdevice_ = alcOpenDevice(nullptr);
    if (ALdevice_)
    {
        ALcontext_ = alcCreateContext(ALdevice_,nullptr);
        alcMakeContextCurrent(ALcontext_);
    }
    if (!ALdevice_)
    {
        qDebug() << "Device not made.";
    }
    else
        qDebug() << "Intialization complete.";

    AddSound(QString(gsl::soundFilePath + "cowSound.wav"));
    AddSound(QString(gsl::soundFilePath + "laser.wav"));
    AddSound(QString(gsl::soundFilePath + "Caravan_mono.wav"));
}

void AudioManager::AddSound(QString filePath)
{
    alcMakeContextCurrent(ALcontext_);
    QString fileName = filePath.section('/', -1);
    if(!QFile::exists(gsl::soundFilePath + fileName))
        QFile::copy(filePath, gsl::soundFilePath + fileName);

    std::shared_ptr<WAV_t> wav = std::make_shared<WAV_t>();
    if (!LoadWAV(filePath.toStdString(), wav))
    {
        qDebug() << "Error loading WAV file.";
    }
    else
    {
        //putting audio into the buffer
        ALenum format{};
        switch (wav->bitsPerSample_)
        {
        case 8:
            switch (wav->channels_)
            {
            case 1: format = AL_FORMAT_MONO8;
                qDebug() << "Format: 8bit Mono";
                break;
            case 2: format = AL_FORMAT_STEREO8;
                qDebug() << "Format: 8bit Stereo";
                break;
            default: break;
            }
            break;
        case 16:
            switch (wav->channels_)
            {
            case 1: format = AL_FORMAT_MONO16;
                qDebug() << "Format: 16bit Mono";
                break;
            case 2: format = AL_FORMAT_STEREO16;
                qDebug() << "Format: 16bit Stereo";
                break;
            default: break;
            }
            break;
        default: break;
        }
        ALuint newBuffer = GetNewBuffer();
        std::shared_ptr<Sound> sound = std::make_shared<Sound>();
        sound->buffer_ = newBuffer;
        sound->name_ = fileName;
        alGenBuffers(1, &sound->buffer_);
        alBufferData(sound->buffer_, format, wav->buffer_, static_cast<ALint>(wav->dataSize_), static_cast<ALint>(wav->sampleRate_));
        sounds_.push_back(sound);
    }
}

void AudioManager::SetSound(QString soundName, std::shared_ptr<AudioComponent> component)
{
    alcMakeContextCurrent(ALcontext_);
    alSourceStop(component->source_);
    for(size_t i = 0; i < sounds_.size(); i++)
    {
        if(sounds_[i]->name_ == soundName)
        {
            qDebug() << "Setting sound " << soundName << " to component.";
            component->currentBuffer_ = sounds_[i]->buffer_;
            alSourcei(component->source_, AL_BUFFER, static_cast<ALint>(component->currentBuffer_));
            component->soundID_ = static_cast<ALuint>(i);
            return;
        }
    }
    qDebug() << "No audio found";
}

void AudioManager::SetSound(size_t index, std::shared_ptr<AudioComponent> component)
{
    alcMakeContextCurrent(ALcontext_);
    alSourceStop(component->source_);
    if(sounds_.size() < index)
        qDebug() << "ERROR: SetSound - index too high! returning...";

    qDebug() << "Setting sound " << sounds_[index]->name_ << " to component.";
    component->currentBuffer_ = sounds_[index]->buffer_;
    alSourcei(component->source_, AL_BUFFER, static_cast<ALint>(component->currentBuffer_));
    component->soundID_ = static_cast<ALuint>(index);
}

bool AudioManager::LoadWAV(std::string filePath, std::shared_ptr<WAV_t> WAVPtr)
{
    qDebug() << "Loading " + QString::fromStdString(filePath) + " from disk";
    FILE* fp = nullptr;
    fp = std::fopen(filePath.c_str(), "rb");
    if (fp == nullptr)
    {
        return EndOnError("FileHandler error: File not found.");
    }

    char type[4];

    std::fread(type, sizeof(char), 4, fp);
    if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F')
    {
        return EndOnError("FileHandler error: RIFF header missing or invalid.");
    }

    std::fread(&WAVPtr->size_, sizeof(uint32_t), 1, fp);
    std::fread(type, sizeof(char), 4, fp);
    if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E')
    {
        return EndOnError("FileHandler error: WAV header missing or invalid.");
    }

    std::fread(type, sizeof(char), 4, fp);
    if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ')
    {
        return EndOnError("FileHandler error: fmt header missing or invalid.");
    }

    std::fread(&WAVPtr->chunkSize_,       sizeof(uint32_t), 1, fp);
    std::fread(&WAVPtr->formatType_,      sizeof(short), 1, fp);
    std::fread(&WAVPtr->channels_,        sizeof(short), 1, fp);
    std::fread(&WAVPtr->sampleRate_,      sizeof(uint32_t), 1, fp);
    std::fread(&WAVPtr->avgBytesPerSec_,  sizeof(uint32_t), 1, fp);
    std::fread(&WAVPtr->bytesPerSample_,  sizeof(short), 1, fp);
    std::fread(&WAVPtr->bitsPerSample_,   sizeof(short), 1, fp);

    std::fread(type, sizeof(char), 4, fp);
    if (type[0] != 'd' || type[1] != 'a' || type[2] != 't' || type[3] != 'a')
    {
        return EndOnError("FileHandler error: data header missing or invalid.");
    }

    std::fread(&WAVPtr->dataSize_, sizeof(uint32_t), 1, fp);

    WAVPtr->buffer_ = new unsigned char[WAVPtr->dataSize_];
    uint32_t result = static_cast<uint32_t>(std::fread(WAVPtr->buffer_, sizeof(char), WAVPtr->dataSize_, fp));
    if (result != WAVPtr->dataSize_)
    {
        return EndOnError("FileHandler error: fread result mismatch.");
    }

    if (ferror(fp))
    {
        return EndOnError("FileHandler error: fstream error.");
    }

    if (WAVPtr->buffer_ == nullptr)
    {
        return EndOnError("FileHandler error: WAV Data pointer is NULL.");
    }

    std::fclose(fp);
    qDebug() << "WAV file Loading complete";

    return true;
}

void AudioManager::DeleteSound(unsigned int soundID)
{
    alcMakeContextCurrent(ALcontext_);
    alDeleteBuffers(1,&sounds_[soundID]->buffer_);
    sounds_.erase(sounds_.begin() + soundID);
}

void AudioManager::DeleteAllCustomSounds()
{
    for (size_t i = sounds_.size() - 1; i >= numberOfDefaultSounds_; i--)
    {
        DeleteSound(static_cast<unsigned int>(i));
    }
}

ALuint AudioManager::GetNewBuffer()
{
    buffers_++;
    return buffers_ - 1;
}

ALuint AudioManager::GetNewSource()
{
    sources_++;
    return sources_ - 1;
}

bool AudioManager::EndOnError(std::string errmsg)
{
    qDebug() << QString::fromStdString(errmsg);
    return false;
}
