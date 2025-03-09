//
// Created by zigameni on 3/9/25.
//

#ifndef ZIGA_POMODORO_TIMER_H
#define ZIGA_POMODORO_TIMER_H

#include <QObject>
#include <QTimer>

class Timer : public QObject
{
    Q_OBJECT

public:
    enum class TimerState
    {
        Stopped,
        Running,
        Paused
    };

    enum class TimerMode
    {
        Work,
        ShortBreak,
        LongBreak
    };

    explicit Timer(QObject* parent = nullptr);
    ~Timer() override;

    void start();
    void pause();
    void reset();
    void skipBreak();
    void skipToNext();

    TimerState getState() const;
    TimerMode getMode() const;
    int getRemainingTime() const;
    int getElapsedTime() const;
    int getTotalCompletedPomodoros() const;

    void setWorkDuration(int minutes);
    void setShortBreakDuration(int minutes);
    void setLongBreakDuration(int minutes);
    void setLongBreakInterval(int count);

signals:
    void timerTick(int remainingSeconds);
    void timerCompleted(TimerMode completedMode);
    void modeChanged(TimerMode newMode);
    void stateChanged(TimerState newState);
    void pomodorosCompletedChanged(int count);

private slots:
    void onTimeout();

private:
    QTimer* m_timer;
    TimerState m_state;
    TimerMode m_mode;
    int m_remainingSeconds;
    int m_elapsedSeconds;
    int m_workDuration;
    int m_shortBreakDuration;
    int m_longBreakDuration;
    int m_longBreakInterval;
    int m_pomodorosCompleted;

    void switchToNextMode();
    void resetTimerForCurrentMode();
};

#endif // ZIGA_POMODORO_TIMER_H
