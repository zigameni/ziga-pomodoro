//
// Created by zigameni on 3/9/25.
//

#ifndef ZIGA_POMODORO_SETTINGS_H
#define ZIGA_POMODORO_SETTINGS_H

#include <QObject>
#include <QSettings>

class Settings : public QObject {
    Q_OBJECT

public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings() override;

    // Timer durations
    int getWorkDuration() const;
    int getShortBreakDuration() const;
    int getLongBreakDuration() const;
    int getLongBreakInterval() const;

    void setWorkDuration(int minutes);
    void setShortBreakDuration(int minutes);
    void setLongBreakDuration(int minutes);
    void setLongBreakInterval(int count);

    // Notification settings
    bool getSoundEnabled() const;
    QString getSoundFile() const;
    bool getDesktopNotificationsEnabled() const;

    void setSoundEnabled(bool enabled);
    void setSoundFile(const QString &filePath);
    void setDesktopNotificationsEnabled(bool enabled);

    // UI settings
    QString getTheme() const;
    bool getMinimizeToTray() const;
    bool getStartMinimized() const;

    void setTheme(const QString &theme);
    void setMinimizeToTray(bool enabled);
    void setStartMinimized(bool enabled);

    // Load and save settings
    void loadSettings();
    void saveSettings();
    void resetToDefaults();

    signals:
        void settingsChanged();

private:
    QSettings m_settings;

    // Timer settings
    int m_workDuration;
    int m_shortBreakDuration;
    int m_longBreakDuration;
    int m_longBreakInterval;

    // Notification settings
    bool m_soundEnabled;
    QString m_soundFile;
    bool m_desktopNotificationsEnabled;

    // UI settings
    QString m_theme;
    bool m_minimizeToTray;
    bool m_startMinimized;
};

#endif // ZIGA_POMODORO_SETTINGS_H