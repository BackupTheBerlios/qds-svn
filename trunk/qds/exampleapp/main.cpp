// Qt includes
#include <qapplication.h>

// QDS includes
#include "qds/qds.h"

// local includes
#include "mainwindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QDS::init(app.argc(), app.argv());

    MainWindow* mainWindow = new MainWindow();
    mainWindow->show();

    app.setMainWidget(mainWindow);

    return app.exec();
}

// End of File
