//
// Created by zigameni on 3/9/25.
//
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
#include <QAction>             // Menu item abstraction
#include <QProgressBar>        // Progress visualization
#include <QDialog>             // Dialog window base
#include <QVBoxLayout>         // Vertical layout manager
#include <QHBoxLayout>         // Horizontal layout manager
#include <QSpinBox>            // Numerical input widget
#include <QCheckBox>           // Checkbox widget
#include <QComboBox>           // Dropdown list widget
#include <QGroupBox>           // Group box container
#include <QFormLayout>         // Form-style layout manager

// Conditional multimedia support
#ifdef HAVE_QT_MULTIMEDIA
#include <QMediaPlayer>        // Audio/video playback
#include <QAudioOutput>        // Audio output control
#endif
// Application-specific headers
#include "timer.h"             // Timer logic implementation
#include "settings.h"          // User settings management

// Main application window class
class MainWindow : public QMainWindow {
    Q_OBJECT  // Required for Qt signals/slots and meta-object system

public:
    explicit MainWindow(QWidget *parent = nullptr);  // Constructor
    ~MainWindow() override;  // Destructor with override specifier
    friend class TimerWindow;

protected:
    // Window management
    void closeEvent(QCloseEvent *event) override;  // Handle window closing

private slots:
    // Timer-related updates
    // void updateTimerDisplay(int remainingSeconds);  // Update time display
    // void handleTimerCompleted(Timer::TimerMode mode);  // Handle finished timer
    // void handleModeChanged(Timer::TimerMode mode);  // React to mode changes
    // void handleStateChanged(Timer::TimerState state);  // Update UI for state changes
    void handlePomodorosCompletedChanged(int count);  // Update completed counter

    // Button click handlers
    // void onStartButtonClicked();    // Start timer
    // void onPauseButtonClicked();    // Pause timer
    // void onResetButtonClicked();    // Reset timer
    // void onSkipButtonClicked();     // Skip current phase
    void onSettingsButtonClicked(); // Open settings

    // System tray interactions
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason); // Tray icon clicks
    void applySettings();            // Apply user preferences
    void showSettingsDialog();       // Display settings UI

    // In mainwindow.h - private section
private:
    // Initialization methods
    void setupUi();          // Create and arrange UI components
    void setupTrayIcon();    // Configure system tray integration
    void setupConnections(); // Connect signals to slots

    // Notification system
    void playNotificationSound();  // Audio feedback
    void showDesktopNotification(const QString &title, const QString &message);  // OS notifications

    // UI updates
    void updateWindowTitle();  // Update title bar text
    void loadStyleSheet(const QString &theme);  // Apply visual styling

    // UI Components
    QWidget *m_centralWidget;     // Main window central widget
    QVBoxLayout *m_mainLayout;    // Primary vertical layout

    // Stats display elements
    QLabel *m_pomodorosCompletedLabel;  // Completed sessions counter

    // Control buttons
    QHBoxLayout *m_buttonLayout;    // Horizontal button arrangement
    QPushButton *m_settingsButton;  // Open settings

    // System Tray Components
    QSystemTrayIcon *m_trayIcon;  // Tray icon instance
    QMenu *m_trayMenu;            // Tray context menu
    QAction *m_showAction;        // Show window action
    QAction *m_quitAction;        // Quit application

    // Sound Notification System
#ifdef HAVE_QT_MULTIMEDIA
    QMediaPlayer *m_mediaPlayer;  // Audio playback component
#endif

    // Core Application Components
    Timer *m_timer;        // Business logic controller
    Settings *m_settings;  // User preferences storage

    // Application State
    int m_totalTime;       // Current timer duration in seconds
};

#endif // ZIGA_POMODORO_MAINWINDOW_H