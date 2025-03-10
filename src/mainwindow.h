//
// Created by zigameni on 3/9/25.
//
#ifndef ZIGA_POMODORO_MAINWINDOW_H
#define ZIGA_POMODORO_MAINWINDOW_H

// Qt Framework Headers
#include <QMainWindow>         // Base class for main window
#include <QLabel>              // Text display widget
#include <QPushButton>         // Clickable button widget
#include <QSystemTrayIcon>     // System tray icon functionality
#include <QMenu>               // Menu container
#include <QHBoxLayout>         // Horizontal layout manager
#include <QComboBox>           // Dropdown selection widget
#include <QDateEdit>           // Date selection widget

// Conditional multimedia support
#ifdef HAVE_QT_MULTIMEDIA
#include <QMediaPlayer>        // Audio/video playback
#endif
// Application-specific headers
#include "timer.h"             // Timer logic implementation
#include "settings.h"          // User settings management
#include "databasemanager.h"   // Database management
#include "pomodoroactivitymap.h" // Pomodoro activity heatmap

// Main application window class
class MainWindow : public QMainWindow
{
    Q_OBJECT // Required for Qt signals/slots and meta-object system

public:
    explicit MainWindow(Settings* settings, QWidget* parent = nullptr);
    ~MainWindow() override; // Destructor with override specifier
    friend class TimerWindow;

    void setDatabaseManager(DatabaseManager* dbManager);

protected:
    // Window management
    void closeEvent(QCloseEvent* event) override; // Handle window closing

private slots:
    void handlePomodorosCompletedChanged(int count); // Update completed counter
    void onSettingsButtonClicked(); // Open settings
    void onTimeRangeChanged(int index); // Handle time range selection
    void onCustomDateRangeChanged(); // Handle custom date range
    void onRefreshStats(); // Refresh statistics

    // System tray interactions
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason); // Tray icon clicks
    void applySettings(); // Apply user preferences
    void showSettingsDialog(); // Display settings UI

    // In mainwindow.h - private section
private:
    // Initialization methods
    void setupUi(); // Create and arrange UI components
    void setupTrayIcon(); // Configure system tray integration
    void setupConnections(); // Connect signals to slots
    void setupStatisticsTab(); // Set up the statistics UI
    void updateStatistics(); // Update statistics display

    // Notification system
    void playNotificationSound(); // Audio feedback
    void showDesktopNotification(const QString& title, const QString& message); // OS notifications

    // UI updates
    void updateWindowTitle(); // Update title bar text
    void loadStyleSheet(const QString& theme); // Apply visual styling

    // UI Components
    QWidget* m_centralWidget; // Main window central widget
    QVBoxLayout* m_mainLayout; // Primary vertical layout

    // Stats display elements
    QLabel* m_pomodorosCompletedLabel; // Completed sessions counter
    QLabel* m_totalTimeLabel; // Total work time
    QLabel* m_avgSessionLabel; // Average session length

    // Date range selection
    QComboBox* m_timeRangeCombo; // Time range dropdown
    QDateEdit* m_fromDateEdit; // Custom from date
    QDateEdit* m_toDateEdit; // Custom to date
    QPushButton* m_refreshButton; // Refresh button

    // Activity map
    PomodoroActivityMap* m_activityMap; // Pomodoro activity heatmap

    // Control buttons
    QHBoxLayout* m_buttonLayout; // Horizontal button arrangement
    QPushButton* m_settingsButton; // Open settings

    // System Tray Components
    QSystemTrayIcon* m_trayIcon; // Tray icon instance
    QMenu* m_trayMenu; // Tray context menu
    QAction* m_showAction; // Show window action
    QAction* m_quitAction; // Quit application

    // Sound Notification System
#ifdef HAVE_QT_MULTIMEDIA
    QMediaPlayer* m_mediaPlayer; // Audio playback component
#endif

    // Core Application Components
    Timer* m_timer; // Business logic controller
    Settings* m_settings; // User preferences storage
    DatabaseManager* m_dbManager; // Database manager

    // Application State
    int m_totalTime; // Current timer duration in seconds
    QDate m_fromDate; // Statistics from date
    QDate m_toDate; // Statistics to date
};

#endif // ZIGA_POMODORO_MAINWINDOW_H
