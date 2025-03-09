//
// Created by zigameni on 3/9/25.
//

#include "timer.h"

Timer::Timer(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_state(TimerState::Stopped)
    , m_mode(TimerMode::Work)
    , m_remainingSeconds(25 * 60)  // Default: 25 minutes
    , m_elapsedSeconds(0)
    , m_workDuration(25 * 60)      // Default: 25 minutes
    , m_shortBreakDuration(5 * 60) // Default: 5 minutes
    , m_longBreakDuration(15 * 60) // Default: 15 minutes
    , m_longBreakInterval(4)       // Default: Every 4 pomodoros
    , m_pomodorosCompleted(0)
{
    connect(m_timer, &QTimer::timeout, this, &Timer::onTimeout);
    m_timer->setInterval(1000);  // 1 second interval
}

Timer::~Timer() {
    m_timer->stop();
}

void Timer::start() {
    if (m_state != TimerState::Running) {
        m_timer->start();
        m_state = TimerState::Running;
        emit stateChanged(m_state);
    }
}

void Timer::pause() {
    if (m_state == TimerState::Running) {
        m_timer->stop();
        m_state = TimerState::Paused;
        emit stateChanged(m_state);
    }
}

void Timer::reset() {
    m_timer->stop();
    m_state = TimerState::Stopped;
    resetTimerForCurrentMode();
    m_elapsedSeconds = 0;
    emit stateChanged(m_state);
    emit timerTick(m_remainingSeconds);
}

void Timer::skipToNext() {
    m_timer->stop();
    m_state = TimerState::Stopped;

    // Complete current timer
    if (m_mode == TimerMode::Work) {
        m_pomodorosCompleted++;
        emit pomodorosCompletedChanged(m_pomodorosCompleted);
    }

    switchToNextMode();
    emit stateChanged(m_state);
}

Timer::TimerState Timer::getState() const {
    return m_state;
}

Timer::TimerMode Timer::getMode() const {
    return m_mode;
}

int Timer::getRemainingTime() const {
    return m_remainingSeconds;
}

int Timer::getElapsedTime() const {
    return m_elapsedSeconds;
}

int Timer::getTotalCompletedPomodoros() const {
    return m_pomodorosCompleted;
}

void Timer::setWorkDuration(int minutes) {
    m_workDuration = minutes * 60;
    if (m_mode == TimerMode::Work && m_state == TimerState::Stopped) {
        m_remainingSeconds = m_workDuration;
        emit timerTick(m_remainingSeconds);
    }
}

void Timer::setShortBreakDuration(int minutes) {
    m_shortBreakDuration = minutes * 60;
    if (m_mode == TimerMode::ShortBreak && m_state == TimerState::Stopped) {
        m_remainingSeconds = m_shortBreakDuration;
        emit timerTick(m_remainingSeconds);
    }
}

void Timer::setLongBreakDuration(int minutes) {
    m_longBreakDuration = minutes * 60;
    if (m_mode == TimerMode::LongBreak && m_state == TimerState::Stopped) {
        m_remainingSeconds = m_longBreakDuration;
        emit timerTick(m_remainingSeconds);
    }
}

void Timer::setLongBreakInterval(int count) {
    m_longBreakInterval = count;
}

void Timer::onTimeout() {
    m_remainingSeconds--;
    m_elapsedSeconds++;

    emit timerTick(m_remainingSeconds);

    if (m_remainingSeconds <= 0) {
        m_timer->stop();
        m_state = TimerState::Stopped;

        // Emit signal for timer completion
        emit timerCompleted(m_mode);

        // If work timer completed, increment counter
        if (m_mode == TimerMode::Work) {
            m_pomodorosCompleted++;
            emit pomodorosCompletedChanged(m_pomodorosCompleted);
        }

        switchToNextMode();
        emit stateChanged(m_state);
    }
}

void Timer::switchToNextMode() {
    TimerMode previousMode = m_mode;

    switch (m_mode) {
        case TimerMode::Work:
            // Check if it's time for a long break
            if (m_pomodorosCompleted % m_longBreakInterval == 0) {
                m_mode = TimerMode::LongBreak;
            } else {
                m_mode = TimerMode::ShortBreak;
            }
            break;

        case TimerMode::ShortBreak:
        case TimerMode::LongBreak:
            m_mode = TimerMode::Work;
            break;
    }

    resetTimerForCurrentMode();

    if (previousMode != m_mode) {
        emit modeChanged(m_mode);
    }
}

void Timer::resetTimerForCurrentMode() {
    switch (m_mode) {
        case TimerMode::Work:
            m_remainingSeconds = m_workDuration;
            break;

        case TimerMode::ShortBreak:
            m_remainingSeconds = m_shortBreakDuration;
            break;

        case TimerMode::LongBreak:
            m_remainingSeconds = m_longBreakDuration;
            break;
    }

    m_elapsedSeconds = 0;
    emit timerTick(m_remainingSeconds);
}