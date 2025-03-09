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

#include "timer.h"
#include "mainwindow.h"

class TimerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TimerWindow(QWidget* parent = nullptr);
    ~TimerWindow() override;

    // Access to internal timer for MainWindow
    Timer* getTimer() const { return m_timer; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void updateTimerDisplay(int remainingSeconds);
    void handleModeChanged(Timer::TimerMode mode);
    void handleStateChanged(Timer::TimerState state);
    void onStartPauseButtonClicked();
    void onStopButtonClicked();
    void onSettingsButtonClicked();
    void resizeEvent(QResizeEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    // enterEvent(QEvent* event)

private:
    void setupUi();
    void setupConnections();
    void updateStartPauseButton();

    // UI Components
    QLabel* m_timerLabel;
    QPushButton* m_startPauseButton;
    QPushButton* m_stopButton;
    QPushButton* m_settingsButton;
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_buttonLayout;
    QPushButton* m_closeButton;

    // Core components
    Timer* m_timer;
    MainWindow* m_mainWindow;

    // For window dragging
    QPoint m_dragPosition;

    bool isRunning = false; // Track if timer is running
    void updateStartPauseIcon();
};

#endif // ZIGA_POMODORO_TIMERWINDOW_H
