#include <QtQuick/QQuickWindow>
#include <QtWidgets/QApplication>

#include "QUAVApplication.h"

//-----------------------------------------------------------------------------
/**
 * @brief Starts the application
 *
 * @param argc Number of commandline arguments
 * @param argv Commandline arguments
 * @return exit code, 0 for normal exit and !=0 for error cases
 */

int main(int argc, char *argv[])
{
    bool runUnitTests = false;
    bool simpleBootTest = false;

    QUAVApplication app(argc, argv, runUnitTests, simpleBootTest);

    app.init();

    int exitCode = 0;
    {
        if (!simpleBootTest) {
            exitCode = app.exec();
        }
    }

    app.shutdown();

    qDebug() << "Exiting main 111";

    return exitCode;
}
