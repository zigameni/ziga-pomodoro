//
// Created by zigameni on 3/9/25.
//

#ifndef ZIGA_POMODORO_TIMERWINDOW_H
#define ZIGA_POMODORO_TIMERWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMediaPlayer> // For sound
#include <QSystemTrayIcon> // For notifications

#include "timer.h"
#include "mainwindow.h"
#include "settings.h"
#include "databasemanager.h" // Add include for DatabaseManager

class TimerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TimerWindow(Settings* settings, QWidget* parent = nullptr);
    ~TimerWindow() override;

    // Access to internal timer for MainWindow
    Timer* getTimer() const { return m_timer; }

    // Set the database manager
    void setDatabaseManager(DatabaseManager* dbManager);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void updateTimerDisplay(int remainingSeconds);
    void handleModeChanged(Timer::TimerMode mode);
    void handleStateChanged(Timer::TimerState state);
    void handleTimerCompleted(Timer::TimerMode completedMode); // New slot for timer completion
    void onStartPauseButtonClicked();
    void onStopButtonClicked();
    void onSettingsButtonClicked();
    void resizeEvent(QResizeEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void onSettingsChanged();
    void playNotificationSound();
    void showDesktopNotification(const QString& title, const QString& message);

private:
    void setupUi();
    void updateTimerLabelStyle();
    void setupConnections();
    void updateStartPauseButton();

    QMediaPlayer* m_mediaPlayer;
    QSystemTrayIcon* m_trayIcon;

    // UI Components
    QLabel* m_timerLabel;
    QPushButton* m_startPauseButton;
    QPushButton* m_stopButton;
    QPushButton* m_settingsButton;
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_buttonLayout;
    QPushButton* m_closeButton;
    QPushButton* m_skipButton;

    // Core components
    Timer* m_timer;
    MainWindow* m_mainWindow;
    DatabaseManager* m_dbManager; // Add database manager member

    // Session tracking
    QDateTime m_sessionStartTime; // Track when the current session started
    bool m_hasDbManager; // Flag to check if DB manager is set and initialized

    // For window dragging
    QPoint m_dragPosition;

    bool isRunning = false; // Track if timer is running
    void updateStartPauseIcon();
    void onSkipButtonClicked();
    void updateWindowSize();

    Settings* m_appSettings;
};

#endif // ZIGA_POMODORO_TIMERWINDOW_H
