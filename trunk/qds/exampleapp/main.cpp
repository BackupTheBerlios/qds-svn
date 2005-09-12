// Qt includes
#include <qapplication.h>

// QDS includes
#include "qds/qds.h"

// local includes
#include "mainwindow.h"

int main(int argc, char** argv)
{
    QApplication* app = QDS::createApplication(argc, argv);

    MainWindow* mainWindow = new MainWindow();
    mainWindow->show();

    app->setMainWidget(mainWindow);
    
    int ret = app->exec();

    delete app;
    return ret;
}

// End of File
