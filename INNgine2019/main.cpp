#include <QApplication>
#include "mainwindow.h"
#include "Legacy/constants.h"


void SetupDarkStyle()
{
    QFile f(gsl::assetFilePath + "/Styles/qdarkstyle/style.qss");
    if (!f.exists())
    {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
}

int main(int argc, char *argv[])
{
    //Forces the usage of desktop OpenGL
    //Attribute must be set before Q(Gui)Application is constructed:
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    //Makes an Qt application
    QApplication app(argc, argv);

    QPixmap pixmap(gsl::assetFilePath + "Icons/Alberto_Splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    //Makes the Qt MainWindow and shows it.
    MainWindow window;
    window.setAnimated(false);
    window.show();

    SetupDarkStyle();

    splash.finish(&window);

    return app.exec();
}
