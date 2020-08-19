#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QMessageBox>
#include <string>
#include "vector3d.h"

namespace gsl  //Game School Lib
{
const QString projectFolderName{"../INNgine2019/"};
const QString assetFilePath{projectFolderName + "Assets/"};
const QString soundFilePath{assetFilePath + "Sounds/"};
const QString shaderFilePath{projectFolderName + "Shaders/"};
const QString scriptFilePath{projectFolderName + "Scripts/"};
const QString meshFilePath{assetFilePath + "Meshes/"};
const QString textureFilePath{assetFilePath + "Textures/"};

const int MAX_NUMBER_OF_LIGHTS{30};
const int MAX_COLORS{255};

const float LODLEVEL1{7.5f};
const float LODLEVEL2{15.f};

const size_t INVALID_SIZE{static_cast<size_t>(-1)};

struct Sphere
{
    float radius;
    gsl::Vector3D center;
};

struct Plane
{
    float distanceToOrigo;
    gsl::Vector3D normal;
};


}
static void CreateMessageBox(QString messageboxTitle, QString text, QMessageBox::StandardButton buttonType = QMessageBox::StandardButton::Ok){
    QMessageBox* x  = new QMessageBox(QMessageBox::Information,
                                      messageboxTitle,
                                      text,
                                      buttonType);
    x->setStyleSheet("QPushButton{min-width: 100px;}");
    x->open();
    x->setAttribute(Qt::WA_DeleteOnClose);
}
#endif // CONSTANTS_H
