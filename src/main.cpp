//
// Created by zigameni on 3/9/25.
//

#include "timerwindow.h"
#include "settings.h"
#include <QApplication>
#include <QDir>

int main(int argc, char* argv[])
{
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

    // Instantiate Settings object HERE
    Settings* appSettings = new Settings(); // Create as pointer

    // Initialize and display the timer window, PASSING the settings object
    TimerWindow timerWindow(appSettings);
    // <----- MODIFIED LINE: Pass the appSettings object to the TimerWindow constructor

    // Initialize and display the timer window instead of MainWindow
    // TimerWindow timerWindow;
    timerWindow.show();
    // Make sure appSettings outlives the application
    QObject::connect(&app, &QApplication::aboutToQuit, [appSettings]()
    {
        appSettings->saveSettings();
        delete appSettings;
    });

    return app.exec();
}
