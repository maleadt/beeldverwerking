//
// Configuration
//

// Headers
#include <exception>
#include <QtCore/QTextStream>
#include <QtGui/QApplication>
#include "mainwindow.h"

//
// Main
//

int main(int argc, char** argv)
{
    try
    {
        QApplication tApplication(argc, argv);
        MainWindow tWindow;
        tWindow.show();
        return tApplication.exec();
    }
    catch (std::exception iException)
    {
        QTextStream qerr(stderr);

        qerr << "---------------------------------------\n";
        qerr << "          UNTRAPPED EXCEPTION          \n";
        qerr << "---------------------------------------\n";
        qerr << "\n";
        qerr << iException.what();
    }
}

