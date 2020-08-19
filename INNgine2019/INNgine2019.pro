QT          += core gui widgets
QT       += core qml
QT += quick widgets

TEMPLATE    = app
CONFIG      += c++17

TARGET      = INNgine2019

PRECOMPILED_HEADER = Legacy/innpch.h

INCLUDEPATH +=  ./GSL
INCLUDEPATH +=  ./include

RESOURCES +=
RESOURCES += Assets/Styles/qdarkstyle/style.qrc
mac {
    LIBS += -framework OpenAL
}

win32 {
    INCLUDEPATH += $(OPENAL_HOME)\\include\\AL
    LIBS *= $(OPENAL_HOME)\\libs\\Win64\\libOpenAL32.dll.a
}

HEADERS += \
#
    GSL/bsplinecurve.h \
    GSL/matrix2x2.h \
    GSL/matrix3x3.h \
    GSL/matrix4x4.h \
    GSL/quaternion.h \
    GSL/vector2d.h \
    GSL/vector3d.h \
    GSL/vector4d.h \
    GSL/gsl_math.h \
    GSL/math_constants.h \
#
    Legacy/constants.h \
    Legacy/vertex.h \
    Legacy/camera.h \
    Legacy/gltypes.h \
    Legacy/input.h \
#    innpch.h \
#
    Managers/assetmanager.h \
    Managers/audiomanager.h \
    Managers/componentmanager.h \
    Managers/components.h \
    Managers/entitymanager.h \
    Managers/materialmanager.h \
    Managers/meshmanager.h \
    Managers/scenemanager.h \
    Managers/shadermanager.h \
    Managers/texturemanager.h \
    Systems/audiosystem.h \
    Systems/movementsystem.h \
    Systems/rendersystem.h \
#
    UI/assetmanagerwidget.h \
    UI/assetviewer.h \
    UI/audiowidget.h \
    UI/baseproperty.h \
    UI/basewidget.h \
    UI/comboboxproperty.h \
    UI/landscapeeditor.h \
    UI/lightwidget.h \
    UI/lineboxproperty.h \
    UI/materialcreator.h \
    UI/meshwidget.h \
    UI/spinboxproperty.h \
    UI/transformwidget.h \
    UI/valuebox.h \
    renderwindow.h \
    mainwindow.h \
    script.h \


SOURCES += main.cpp \
#
    GSL/bsplinecurve.cpp \
    GSL/matrix2x2.cpp \
    GSL/matrix3x3.cpp \
    GSL/matrix4x4.cpp \
    GSL/quaternion.cpp \
    GSL/vector2d.cpp \
    GSL/vector3d.cpp \
    GSL/vector4d.cpp \
    GSL/gsl_math.cpp \
#
    Legacy/vertex.cpp \
    Legacy/camera.cpp \
#
    Managers/assetmanager.cpp \
    Managers/audiomanager.cpp \
    Managers/componentmanager.cpp \
    Managers/components.cpp \
    Managers/entitymanager.cpp \
    Managers/materialmanager.cpp \
    Managers/meshmanager.cpp \
    Managers/scenemanager.cpp \
    Managers/shadermanager.cpp \
    Managers/texturemanager.cpp \
    Systems/audiosystem.cpp \
    Systems/movementsystem.cpp \
    Systems/rendersystem.cpp \
#
    UI/assetmanagerwidget.cpp \
    UI/assetviewer.cpp \
    UI/audiowidget.cpp \
    UI/baseproperty.cpp \
    UI/basewidget.cpp \
    UI/comboboxproperty.cpp \
    UI/landscapeeditor.cpp \
    UI/lightwidget.cpp \
    UI/lineboxproperty.cpp \
    UI/materialcreator.cpp \
    UI/meshwidget.cpp \
    UI/spinboxproperty.cpp \
    UI/transformwidget.cpp \
    UI/valuebox.cpp \
    renderwindow.cpp \
    mainwindow.cpp \
    script.cpp \

FORMS += \
    UI/landscapeeditor.ui \
    UI/materialcreator.ui \
    mainwindow.ui

DISTFILES += \
    Scripts/TestScript.js \
    Shaders/MonoColorShader.frag \
    Shaders/MonoColorShader.vert \
    Shaders/lightfragment.frag \
    Shaders/lightshader.frag \
    Shaders/lightshader.vert \
    Shaders/lightvertex.vert \
    Shaders/plainshader.frag \
    Shaders/plainshader.vert \
    Shaders/textureshader.frag \
    GSL/README.md \
    README.md \
    Shaders/textureshader.vert \
