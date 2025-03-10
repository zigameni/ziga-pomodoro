//
// Created by zigameni on 3/9/25.
//

#include "timerwindow.h"
#include "settings.h"
#include "databasemanager.h" // Add include for DatabaseManager
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

    // Instantiate Settings object
    Settings* appSettings = new Settings(); // Create as pointer

    // Create and initialize database manager
    DatabaseManager* dbManager = new DatabaseManager();
    if (!dbManager->initialize())
    {
        qWarning() << "Failed to initialize database, continuing without persistence";
    }

    // Initialize and display the timer window, passing the settings object
    TimerWindow timerWindow(appSettings);
    timerWindow.setDatabaseManager(dbManager); // Add this method to TimerWindow
    timerWindow.show();

    // Make sure resources outlive the application
    QObject::connect(&app, &QApplication::aboutToQuit, [appSettings, dbManager]()
    {
        appSettings->saveSettings();
        delete appSettings;
        delete dbManager;
    });

    return app.exec();
}
