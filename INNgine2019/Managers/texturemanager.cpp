#include "texturemanager.h"
#include <QDir>

TextureManager::TextureManager()
{
    qDebug() << "\n\nINITIALIZING TEXTURE MANAGER";
    AddTexture(gsl::textureFilePath + "white.bmp"); // 0
    AddTexture(gsl::textureFilePath + "white.bmp"); // 1
}

void TextureManager::AddTexture(const QString& filePath, GLint wrap, GLint filter)
{
    if(filePath.length() < 0) // if no path given
        return;

    QString fileName = filePath.section('/', -1);
    if(!QFile::exists(gsl::textureFilePath + fileName))
        QFile::copy(filePath, gsl::textureFilePath + fileName);

    initializeOpenGLFunctions();
    textures_.push_back(std::make_shared<Texture>(fileName, wrap, filter));
    ReadBitmapFile(filePath);
    setTexture();
}

void TextureManager::DeleteTexture(unsigned int textureID)
{
    qDebug() << "Texture deleted:" << textures_[textureID]->name_;
    glDeleteTextures(1, &textures_[textureID]->glName_);
    textures_.erase(textures_.begin() + textureID);
}

void TextureManager::DeleteAllCustomTextures()
{
    for (int i = textures_.size() - 1; i >= numberOfDefaultTextures_; i--)
    {
        glDeleteTextures(1, &textures_[i]->glName_);
        textures_.erase(textures_.begin() + i);
    }
}

void TextureManager::setTexture()
{
    glGenTextures(1, &textures_.back()->glName_);

    // activate the texture unit first before binding texture
    // somehow it wasnt activating first texture properly
    if(textures_.back()->glName_ == 0)
        glActiveTexture(GL_TEXTURE0);
    else
        glActiveTexture(GL_TEXTURE0 + textures_.back()->glName_);

    glBindTexture(GL_TEXTURE_2D, textures_.back()->glName_);

    qDebug() << "Texture ID: " << textures_.size()-1 << " OpenGL ID: " << textures_.back()->glName_;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textures_.back()->wrapS_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textures_.back()->wrapT_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textures_.back()->magFilter_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textures_.back()->minFilter_);
    glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGB,
                textures_.back()->columns_,
                textures_.back()->rows_,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                textures_.back()->bitmap_);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void TextureManager::ReadBitmapFile(const QString& filePath)
{
    OBITMAPFILEHEADER bmFileHeader;
    OBITMAPINFOHEADER bmInfoHeader;

    std::ifstream file;

    file.open (filePath.toStdString().c_str(), std::ifstream::in | std::ifstream::binary);
    if (file.is_open())
    {
        //        file.read((char *) &bmFileHeader, 14);
        file.read(reinterpret_cast<char*>(&bmFileHeader), 14);
        //        file.read((char *) &bmInfoHeader, sizeof(OBITMAPINFOHEADER));
        file.read(reinterpret_cast<char*>(&bmInfoHeader), sizeof(OBITMAPINFOHEADER));

        textures_.back()->columns_ = bmInfoHeader.biWidth;
        textures_.back()->rows_ = bmInfoHeader.biHeight;
        textures_.back()->nByte_ = bmInfoHeader.biBitCount / 8;

        textures_.back()->bitmap_ = new unsigned char[static_cast<size_t>(textures_.back()->columns_ * textures_.back()->rows_ * textures_.back()->nByte_)];
        file.read(reinterpret_cast<char*>(textures_.back()->bitmap_), textures_.back()->columns_ * textures_.back()->rows_ * textures_.back()->nByte_);
        file.close();
    }
    else
    {
        qDebug() << "Can not read " << filePath;
    }
    unsigned char tmp;
    // switch red and blue
    for (int k = 0; k < (textures_.back()->columns_ * textures_.back()->rows_ * textures_.back()->nByte_); k += textures_.back()->nByte_) {
        tmp = textures_.back()->bitmap_[k];
        textures_.back()->bitmap_[k] = textures_.back()->bitmap_[k + 2];
        textures_.back()->bitmap_[k + 2] = tmp;
    }
    qDebug() << "Texture read: " << filePath;
}
