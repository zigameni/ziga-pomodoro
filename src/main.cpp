//
// Created by zigameni on 3/9/25.
//

#include "timerwindow.h"
#include <QApplication>
#include <QDir>

int main(int argc, char *argv[]) {
    // Initialize Qt application framework with command-line arguments
    QApplication app(argc, argv);

    // Set application metadata for organization and identification
    QApplication::setApplicationName("Ziga-Pomodoro");
    QApplication::setApplicationVersion("0.1");
    QApplication::setOrganizationName("ZigaPomodoro");
    QApplication::setOrganizationDomain("zigapomodoro.com");

    // Create resource directory structure if not exists
    QDir().mkpath(QDir::currentPath() + "/resources/icons");
    QDir().mkpath(QDir::currentPath() + "/resources/sounds");
    QDir().mkpath(QDir::currentPath() + "/resources/styles");

    // Initialize and display the timer window instead of MainWindow
    TimerWindow timerWindow;
    timerWindow.show();

    // Start the Qt event loop
    return app.exec();
}