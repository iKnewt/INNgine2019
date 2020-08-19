#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

//Quick fix to get rid of windows.h which contains
//BITMAPINFOHEADER and BITMAPFILEHEADER.
typedef unsigned short int OWORD;    //should be 16 bit
typedef unsigned int ODWORD;         //should be 32 bit
typedef int OLONG;                   //should be 32 bit

/// Defines what the header data in a bitmap
struct OBITMAPINFOHEADER {
    ODWORD biSize;
    OLONG  biWidth;
    OLONG  biHeight;
    OWORD  biPlanes;
    OWORD  biBitCount;
    ODWORD biCompression;
    ODWORD biSizeImage;
    OLONG  biXPelsPerMeter;
    OLONG  biYPelsPerMeter;
    ODWORD biClrUsed;
    ODWORD biClrImportant;
};

struct OBITMAPFILEHEADER {
    OWORD  bfType;
    ODWORD bfSize;
    OWORD  bfReserved1;
    OWORD  bfReserved2;
    ODWORD bfOffBits;
};

/// Contains data connected to textures.
struct Texture{

    Texture(QString name, GLint wrap = GL_REPEAT, GLint filter = GL_LINEAR) : name_(name), wrapS_(wrap), wrapT_(wrap), magFilter_(filter), minFilter_(filter){}
    GLuint glName_{0};
    /// Name of texture, used to identify texture in GUI and debug.
    QString name_;

    GLubyte pixels_[16];
    /// Bitmap data.
    unsigned char* bitmap_;
    /// Bitmap width in pixels.
    int columns_;
    /// Bitmap height in pixels.
    int rows_;
    /// Number of bytes per pixel, which is the color depth of the image.
    int nByte_;

    /// Texture wrap in S/U/X direction.
    GLint wrapS_;
    /// Texture wrap in T/V/Y direction.
    GLint wrapT_;
    /// Used when the pixel being textured maps to an area less than or equal to one texture element.
    GLint magFilter_;
    /// Used whenever the pixel being textured maps to an area greater than one texture element.
    GLint minFilter_;
};

/// Keeps all the data and logic connected to Textures.
class TextureManager : public QOpenGLFunctions_4_1_Core
{
public:
    TextureManager();

    /// Number of defaults, used to make sure default are not deleted or edited.
    const size_t numberOfDefaultTextures_{2};
    /// Vector containing all textures.
    std::vector<std::shared_ptr<Texture>> textures_;
    /**
     * Adds new texture to textures_.
     * @param filename Filename of texture to import, must be .bmp and placed in Textures/ folder in predefined gsl::assetFilePath.
     * @param wrap How to wrap the texture.
     * @param filter How to filter the texture.
     */
    void AddTexture(const QString& filePath, GLint wrap = GL_REPEAT, GLint filter = GL_LINEAR);
    /**
     *
     * @param textureID
     */
    void DeleteTexture(unsigned int textureID);
    /**
     * Deletes all textures after the
     */
    void DeleteAllCustomTextures();
private:
    /**
     * sets up texture and mipmap settings in open gl for the last element in textures_.
     */
    void setTexture();
    /**
     * Reads BMP image file into the bitmap_ of the last element in textures_.
     * @param filename Filename of BMP to use, must be in Textures/ folder in predefined gsl::assetFilePath.
     */
    void ReadBitmapFile(const QString& filePath);

};

#endif // TEXTUREMANAGER_H
