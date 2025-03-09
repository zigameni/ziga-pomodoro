//
// Created by zigameni on 3/9/25.
//

// #include "settings.h"
#include "settings.h"
#include <QJsonParseError>
#include <QDir>
#include <QStandardPaths> // For getting standard paths


// In the Settings constructor, add these initializations:
Settings::Settings(QObject* parent)
    : QObject(parent)
      , m_settings("ZigaPomodoro", "ZigaPomodoro")
      , m_workDuration(25)
      , m_shortBreakDuration(5)
      , m_longBreakDuration(15)
      , m_longBreakInterval(4)
      , m_soundEnabled(true)
      , m_soundFile(":/sounds/alarm.wav")
      , m_desktopNotificationsEnabled(true)
      , m_theme("default")
      , m_minimizeToTray(false)
      , m_startMinimized(false)
      , m_fontSize(48)
      , m_fontColor("#FFFFFF") // White
      , m_textShadowEnabled(false)
      , m_textShadowColor("#000000") // Black
      , m_textShadowBlur(3)
      , m_textShadowOffsetX(1)
      , m_textShadowOffsetY(1)
{
    loadSettings();
}

Settings::~Settings()
{
    saveSettings();
}

// Getter implementations:
int Settings::getFontSize() const
{
    return m_fontSize;
}

QString Settings::getFontColor() const
{
    return m_fontColor;
}

bool Settings::getTextShadowEnabled() const
{
    return m_textShadowEnabled;
}

QString Settings::getTextShadowColor() const
{
    return m_textShadowColor;
}

int Settings::getTextShadowBlur() const
{
    return m_textShadowBlur;
}

int Settings::getTextShadowOffsetX() const
{
    return m_textShadowOffsetX;
}

int Settings::getTextShadowOffsetY() const
{
    return m_textShadowOffsetY;
}

// Setter implementations:
void Settings::setFontSize(int size)
{
    if (m_fontSize != size)
    {
        m_fontSize = size;
        emit settingsChanged();
    }
}

// When changing settings
void Settings::setFontColor(const QString& color)
{
    if (m_fontColor != color)
    {
        m_fontColor = color;
        saveSettings(); // Save immediately
        emit settingsChanged();
    }
}

void Settings::setTextShadowEnabled(bool enabled)
{
    if (m_textShadowEnabled != enabled)
    {
        m_textShadowEnabled = enabled;
        emit settingsChanged();
    }
}

void Settings::setTextShadowColor(const QString& color)
{
    if (m_textShadowColor != color)
    {
        m_textShadowColor = color;
        emit settingsChanged();
    }
}

void Settings::setTextShadowBlur(int blur)
{
    if (m_textShadowBlur != blur)
    {
        m_textShadowBlur = blur;
        emit settingsChanged();
    }
}

void Settings::setTextShadowOffsetX(int offset)
{
    if (m_textShadowOffsetX != offset)
    {
        m_textShadowOffsetX = offset;
        emit settingsChanged();
    }
}

void Settings::setTextShadowOffsetY(int offset)
{
    if (m_textShadowOffsetY != offset)
    {
        m_textShadowOffsetY = offset;
        emit settingsChanged();
    }
}


int Settings::getWorkDuration() const
{
    return m_workDuration;
}

int Settings::getShortBreakDuration() const
{
    return m_shortBreakDuration;
}

int Settings::getLongBreakDuration() const
{
    return m_longBreakDuration;
}

int Settings::getLongBreakInterval() const
{
    return m_longBreakInterval;
}

void Settings::setWorkDuration(int minutes)
{
    if (m_workDuration != minutes)
    {
        m_workDuration = minutes;
        emit settingsChanged();
    }
}

void Settings::setShortBreakDuration(int minutes)
{
    if (m_shortBreakDuration != minutes)
    {
        m_shortBreakDuration = minutes;
        emit settingsChanged();
    }
}

void Settings::setLongBreakDuration(int minutes)
{
    if (m_longBreakDuration != minutes)
    {
        m_longBreakDuration = minutes;
        emit settingsChanged();
    }
}

void Settings::setLongBreakInterval(int count)
{
    if (m_longBreakInterval != count)
    {
        m_longBreakInterval = count;
        emit settingsChanged();
    }
}

bool Settings::getSoundEnabled() const
{
    return m_soundEnabled;
}

QString Settings::getSoundFile() const
{
    return m_soundFile;
}

bool Settings::getDesktopNotificationsEnabled() const
{
    return m_desktopNotificationsEnabled;
}

void Settings::setSoundEnabled(bool enabled)
{
    if (m_soundEnabled != enabled)
    {
        m_soundEnabled = enabled;
        emit settingsChanged();
    }
}

void Settings::setSoundFile(const QString& filePath)
{
    if (m_soundFile != filePath)
    {
        m_soundFile = filePath;
        emit settingsChanged();
    }
}

void Settings::setDesktopNotificationsEnabled(bool enabled)
{
    if (m_desktopNotificationsEnabled != enabled)
    {
        m_desktopNotificationsEnabled = enabled;
        emit settingsChanged();
    }
}

QString Settings::getTheme() const
{
    return m_theme;
}

bool Settings::getMinimizeToTray() const
{
    return m_minimizeToTray;
}

bool Settings::getStartMinimized() const
{
    return m_startMinimized;
}

void Settings::setTheme(const QString& theme)
{
    if (m_theme != theme)
    {
        m_theme = theme;
        emit settingsChanged();
    }
}

void Settings::setMinimizeToTray(bool enabled)
{
    if (m_minimizeToTray != enabled)
    {
        m_minimizeToTray = enabled;
        emit settingsChanged();
    }
}

void Settings::setStartMinimized(bool enabled)
{
    if (m_startMinimized != enabled)
    {
        m_startMinimized = enabled;
        emit settingsChanged();
    }
}

void Settings::loadSettings()
{
    m_workDuration = m_settings.value("timer/workDuration", 25).toInt();
    m_shortBreakDuration = m_settings.value("timer/shortBreakDuration", 5).toInt();
    m_longBreakDuration = m_settings.value("timer/longBreakDuration", 15).toInt();
    m_longBreakInterval = m_settings.value("timer/longBreakInterval", 4).toInt();

    m_soundEnabled = m_settings.value("notifications/soundEnabled", true).toBool();
    m_soundFile = m_settings.value("notifications/soundFile", ":/sounds/alarm.wav").toString();
    m_desktopNotificationsEnabled = m_settings.value("notifications/desktopEnabled", true).toBool();

    m_theme = m_settings.value("ui/theme", "default").toString();
    m_minimizeToTray = m_settings.value("ui/minimizeToTray", false).toBool();
    m_startMinimized = m_settings.value("ui/startMinimized", false).toBool();


    m_fontSize = m_settings.value("ui/fontSize", 48).toInt();
    m_fontColor = m_settings.value("ui/fontColor", "#FFFFFF").toString();
    m_textShadowEnabled = m_settings.value("ui/textShadowEnabled", false).toBool();
    m_textShadowColor = m_settings.value("ui/textShadowColor", "#000000").toString();
    m_textShadowBlur = m_settings.value("ui/textShadowBlur", 3).toInt();
    m_textShadowOffsetX = m_settings.value("ui/textShadowOffsetX", 1).toInt();
    m_textShadowOffsetY = m_settings.value("ui/textShadowOffsetY", 1).toInt();
}


void Settings::saveSettings()
{
    m_settings.setValue("timer/workDuration", m_workDuration);
    m_settings.setValue("timer/shortBreakDuration", m_shortBreakDuration);
    m_settings.setValue("timer/longBreakDuration", m_longBreakDuration);
    m_settings.setValue("timer/longBreakInterval", m_longBreakInterval);

    m_settings.setValue("notifications/soundEnabled", m_soundEnabled);
    m_settings.setValue("notifications/soundFile", m_soundFile);
    m_settings.setValue("notifications/desktopEnabled", m_desktopNotificationsEnabled);

    m_settings.setValue("ui/theme", m_theme);
    m_settings.setValue("ui/minimizeToTray", m_minimizeToTray);
    m_settings.setValue("ui/startMinimized", m_startMinimized);

    m_settings.setValue("ui/fontSize", m_fontSize);
    m_settings.setValue("ui/fontColor", m_fontColor);
    m_settings.setValue("ui/textShadowEnabled", m_textShadowEnabled);
    m_settings.setValue("ui/textShadowColor", m_textShadowColor);
    m_settings.setValue("ui/textShadowBlur", m_textShadowBlur);
    m_settings.setValue("ui/textShadowOffsetX", m_textShadowOffsetX);
    m_settings.setValue("ui/textShadowOffsetY", m_textShadowOffsetY);


    m_settings.sync();
}

void Settings::resetToDefaults()
{
    m_workDuration = 25;
    m_shortBreakDuration = 5;
    m_longBreakDuration = 15;
    m_longBreakInterval = 4;

    m_soundEnabled = true;
    m_soundFile = ":/sounds/alarm.wav";
    m_desktopNotificationsEnabled = true;

    m_theme = "default";
    m_minimizeToTray = false;
    m_startMinimized = false;

    m_fontSize = 48;
    m_fontColor = "#FFFFFF";
    m_textShadowEnabled = false;
    m_textShadowColor = "#000000";
    m_textShadowBlur = 3;
    m_textShadowOffsetX = 1;
    m_textShadowOffsetY = 1;

    emit settingsChanged();
}
