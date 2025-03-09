//
// Created by zigameni on 3/9/25.
//

#include "mainwindow.h"
#include <QCloseEvent>
#include <QApplication>
#include <QFile>
#include <QTabWidget>
#include <QCheckBox>     // For QCheckBox
#include <QSpinBox>      // For QSpinBox
#include <QComboBox>     // For QComboBox
#include <QPushButton>    // For QPushButton
#include <QDialogButtonBox> // For QDialogButtonBox
#include <QFormLayout>   // For QFormLayout
#include <QVBoxLayout>    // For QVBoxLayout
#include <QGroupBox>     // For QGroupBox
#include <QColorDialog>  // For QColorDialog


// In mainwindow.cpp
MainWindow::MainWindow(Settings* settings, QWidget* parent)
    : QMainWindow(parent)
      , m_timer(new Timer(this))
      , m_settings(settings) // Use the passed settings object
#ifdef HAVE_QT_MULTIMEDIA
      , m_mediaPlayer(new QMediaPlayer(this))
#endif
      , m_totalTime(0)
{
    setupUi();
    setupTrayIcon();
    setupConnections();

    // Apply settings
    applySettings();


    handlePomodorosCompletedChanged(m_timer->getTotalCompletedPomodoros());

    // Set window title
    setWindowTitle("Ziga-Pomodoro");
    setWindowIcon(QIcon(":/icons/tomato.png"));

#ifdef HAVE_QT_MULTIMEDIA

    m_mediaPlayer->setVolume(100);
#endif
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    // Create central widget
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // Create main layout
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    // Create title label
    QLabel* titleLabel = new QLabel("Pomodoro Statistics", m_centralWidget);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(titleLabel);

    // Create pomodoros completed label
    m_pomodorosCompletedLabel = new QLabel("Pomodoros completed: 0", m_centralWidget);
    m_pomodorosCompletedLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(m_pomodorosCompletedLabel);

    // Add a placeholder for future statistics
    QLabel* placeholderLabel = new QLabel("More statistics will be available here", m_centralWidget);
    placeholderLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(placeholderLabel);

    // Add spacer to push content to the top
    m_mainLayout->addStretch();

    // Create button layout
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(5);

    // Create only the settings button
    m_settingsButton = new QPushButton("Settings", m_centralWidget);

    // Add button to layout
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_settingsButton);
    m_buttonLayout->addStretch();

    // Add button layout to main layout
    m_mainLayout->addLayout(m_buttonLayout);

    // Set size
    resize(400, 300);

    // Set window title
    setWindowTitle("Ziga-Pomodoro - Statistics");
    setWindowIcon(QIcon(":/icons/tomato.png"));
}


void MainWindow::setupTrayIcon()
{
    // Create tray icon
    m_trayIcon = new QSystemTrayIcon(QIcon(":/icons/tomato.png"), this);

    // Create tray menu
    m_trayMenu = new QMenu(this);

    // Create actions
    m_showAction = new QAction("Show", this);

    m_quitAction = new QAction("Quit", this);

    // Add actions to menu
    m_trayMenu->addAction(m_showAction);
    m_trayMenu->addSeparator();

    m_trayMenu->addSeparator();
    m_trayMenu->addAction(m_quitAction);

    // Set tray menu
    m_trayIcon->setContextMenu(m_trayMenu);

    // Show tray icon
    m_trayIcon->show();
}

// In mainwindow.cpp - setupConnections()
void MainWindow::setupConnections()
{
    // Connect timer signals (for statistics only)
    connect(m_timer, &Timer::pomodorosCompletedChanged, this, &MainWindow::handlePomodorosCompletedChanged);

    // Connect settings button
    connect(m_settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);

    // Connect tray icon signals
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
    connect(m_showAction, &QAction::triggered, this, &QWidget::show);
    connect(m_quitAction, &QAction::triggered, qApp, &QApplication::quit);
}


void MainWindow::handlePomodorosCompletedChanged(int count)
{
    m_pomodorosCompletedLabel->setText(QString("Pomodoros completed: %1").arg(count));
}

void MainWindow::onSettingsButtonClicked()
{
    showSettingsDialog();
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
    {
        if (isVisible())
        {
            hide();
        }
        else
        {
            show();
            activateWindow();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    m_settings->saveSettings(); // Save settings before closing

    // Always just hide the window instead of closing the application
    hide();
    event->ignore();
}

// Not as important
void MainWindow::playNotificationSound()
{
#ifdef HAVE_QT_MULTIMEDIA
    if (m_settings->getSoundEnabled())
    {
        m_mediaPlayer->setMedia(QUrl::fromLocalFile(m_settings->getSoundFile()));
        m_mediaPlayer->play();
    }
#endif
}

void MainWindow::showDesktopNotification(const QString& title, const QString& message)
{
    if (m_settings->getDesktopNotificationsEnabled())
    {
        m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
    }
}

void MainWindow::updateWindowTitle()
{
    QString stateStr;
    switch (m_timer->getState())
    {
    case Timer::TimerState::Running:
        stateStr = "Running";
        break;
    case Timer::TimerState::Paused:
        stateStr = "Paused";
        break;
    case Timer::TimerState::Stopped:
        stateStr = "Stopped";
        break;
    }

    QString modeStr;
    switch (m_timer->getMode())
    {
    case Timer::TimerMode::Work:
        modeStr = "Work";
        break;
    case Timer::TimerMode::ShortBreak:
        modeStr = "Short Break";
        break;
    case Timer::TimerMode::LongBreak:
        modeStr = "Long Break";
        break;
    }

    // QString timeStr = m_timerLabel->text();

    // setWindowTitle(QString("Ziga-Pomodoro  %1 - %2 [%3]").arg(modeStr, timeStr, stateStr));
    setWindowTitle(QString("Ziga-Pomodoro"));

    m_trayIcon->setToolTip(windowTitle());
}

void MainWindow::applySettings()
{
    // Apply timer settings
    m_timer->setWorkDuration(m_settings->getWorkDuration());
    m_timer->setShortBreakDuration(m_settings->getShortBreakDuration());
    m_timer->setLongBreakDuration(m_settings->getLongBreakDuration());
    m_timer->setLongBreakInterval(m_settings->getLongBreakInterval());

    // Apply UI settings
    loadStyleSheet(m_settings->getTheme());
}

void MainWindow::loadStyleSheet(const QString& theme)
{
    QString stylePath = QString(":/styles/%1.qss").arg(theme);
    QFile file(stylePath);

    if (file.exists() && file.open(QFile::ReadOnly | QFile::Text))
    {
        QString styleSheet = QLatin1String(file.readAll());
        setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        setStyleSheet("");
    }
}

void MainWindow::showSettingsDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Settings");

    QTabWidget* tabWidget = new QTabWidget(&dialog);

    // Timer tab
    QWidget* timerTab = new QWidget(tabWidget);
    QFormLayout* timerLayout = new QFormLayout(timerTab);

    QSpinBox* workDurationSpinBox = new QSpinBox(timerTab);
    workDurationSpinBox->setMinimum(1);
    workDurationSpinBox->setMaximum(60);
    workDurationSpinBox->setValue(m_settings->getWorkDuration());

    QSpinBox* shortBreakDurationSpinBox = new QSpinBox(timerTab);
    shortBreakDurationSpinBox->setMinimum(1);
    shortBreakDurationSpinBox->setMaximum(30);
    shortBreakDurationSpinBox->setValue(m_settings->getShortBreakDuration());

    QSpinBox* longBreakDurationSpinBox = new QSpinBox(timerTab);
    longBreakDurationSpinBox->setMinimum(1);
    longBreakDurationSpinBox->setMaximum(60);
    longBreakDurationSpinBox->setValue(m_settings->getLongBreakDuration());

    QSpinBox* longBreakIntervalSpinBox = new QSpinBox(timerTab);
    longBreakIntervalSpinBox->setMinimum(1);
    longBreakIntervalSpinBox->setMaximum(10);
    longBreakIntervalSpinBox->setValue(m_settings->getLongBreakInterval());

    timerLayout->addRow("Work duration (minutes):", workDurationSpinBox);
    timerLayout->addRow("Short break duration (minutes):", shortBreakDurationSpinBox);
    timerLayout->addRow("Long break duration (minutes):", longBreakDurationSpinBox);
    timerLayout->addRow("Long break interval (pomodoros):", longBreakIntervalSpinBox);

    // Notifications tab
    QWidget* notificationsTab = new QWidget(tabWidget);
    QVBoxLayout* notificationsLayout = new QVBoxLayout(notificationsTab);

    QCheckBox* soundEnabledCheckBox = new QCheckBox("Enable sound notifications", notificationsTab);
    soundEnabledCheckBox->setChecked(m_settings->getSoundEnabled());

    QCheckBox* desktopNotificationsEnabledCheckBox = new QCheckBox("Enable desktop notifications", notificationsTab);
    desktopNotificationsEnabledCheckBox->setChecked(m_settings->getDesktopNotificationsEnabled());

    notificationsLayout->addWidget(soundEnabledCheckBox);
    notificationsLayout->addWidget(desktopNotificationsEnabledCheckBox);
    notificationsLayout->addStretch();

    // UI tab
    QWidget* uiTab = new QWidget(tabWidget);
    QVBoxLayout* uiLayout = new QVBoxLayout(uiTab);

    QComboBox* themeComboBox = new QComboBox(uiTab);
    themeComboBox->addItem("Default", "default");
    themeComboBox->setCurrentText("Default");

    // Minimize options
    QCheckBox* minimizeToTrayCheckBox = new QCheckBox("Minimize to tray when closed", uiTab);
    minimizeToTrayCheckBox->setChecked(m_settings->getMinimizeToTray());

    QCheckBox* startMinimizedCheckBox = new QCheckBox("Start minimized", uiTab);
    startMinimizedCheckBox->setChecked(m_settings->getStartMinimized());

    // Font size
    QSpinBox* fontSizeSpinBox = new QSpinBox(uiTab);
    fontSizeSpinBox->setMinimum(12);
    fontSizeSpinBox->setMaximum(96);
    fontSizeSpinBox->setValue(m_settings->getFontSize());

    // Font color
    QPushButton* fontColorButton = new QPushButton("Select Color", uiTab);
    fontColorButton->setAutoFillBackground(true);

    // Set button background to current font color
    QString currentColor = m_settings->getFontColor();
    fontColorButton->setStyleSheet(QString("background-color: %1;").arg(currentColor));

    // Text shadow checkbox
    QCheckBox* textShadowCheckBox = new QCheckBox("Enable text shadow", uiTab);
    textShadowCheckBox->setChecked(m_settings->getTextShadowEnabled());

    // Shadow color
    QPushButton* shadowColorButton = new QPushButton("Select Shadow Color", uiTab);
    shadowColorButton->setAutoFillBackground(true);

    // Set button background to current shadow color
    QString currentShadowColor = m_settings->getTextShadowColor();
    shadowColorButton->setStyleSheet(QString("background-color: %1;").arg(currentShadowColor));

    // Shadow blur
    QSpinBox* shadowBlurSpinBox = new QSpinBox(uiTab);
    shadowBlurSpinBox->setMinimum(0);
    shadowBlurSpinBox->setMaximum(20);
    shadowBlurSpinBox->setValue(m_settings->getTextShadowBlur());

    // Shadow offset X
    QSpinBox* shadowOffsetXSpinBox = new QSpinBox(uiTab);
    shadowOffsetXSpinBox->setMinimum(-10);
    shadowOffsetXSpinBox->setMaximum(10);
    shadowOffsetXSpinBox->setValue(m_settings->getTextShadowOffsetX());

    // Shadow offset Y
    QSpinBox* shadowOffsetYSpinBox = new QSpinBox(uiTab);
    shadowOffsetYSpinBox->setMinimum(-10);
    shadowOffsetYSpinBox->setMaximum(10);
    shadowOffsetYSpinBox->setValue(m_settings->getTextShadowOffsetY());

    // Group the shadow settings
    QGroupBox* shadowGroupBox = new QGroupBox("Text Shadow Settings", uiTab);
    QFormLayout* shadowLayout = new QFormLayout(shadowGroupBox);
    shadowLayout->addRow("Shadow Color:", shadowColorButton);
    shadowLayout->addRow("Blur Radius:", shadowBlurSpinBox);
    shadowLayout->addRow("Offset X:", shadowOffsetXSpinBox);
    shadowLayout->addRow("Offset Y:", shadowOffsetYSpinBox);

    // Only enable shadow settings if shadow is enabled
    shadowGroupBox->setEnabled(textShadowCheckBox->isChecked());

    // Connect shadow checkbox to enable/disable shadow settings
    connect(textShadowCheckBox, &QCheckBox::toggled, shadowGroupBox, &QGroupBox::setEnabled);

    // Connect color buttons to color dialogs
    connect(fontColorButton, &QPushButton::clicked, [&fontColorButton, this]()
    {
        QColor color = QColorDialog::getColor(QColor(m_settings->getFontColor()), this, "Select Font Color");
        if (color.isValid())
        {
            fontColorButton->setStyleSheet(QString("background-color: %1;").arg(color.name()));
        }
    });

    connect(shadowColorButton, &QPushButton::clicked, [&shadowColorButton, this]()
    {
        QColor color = QColorDialog::getColor(QColor(m_settings->getTextShadowColor()), this, "Select Shadow Color");
        if (color.isValid())
        {
            shadowColorButton->setStyleSheet(QString("background-color: %1;").arg(color.name()));
        }
    });

    // Layout for theme and minimize options
    QFormLayout* themeLayout = new QFormLayout();
    themeLayout->addRow("Theme:", themeComboBox);
    themeLayout->addRow("Font Size:", fontSizeSpinBox);
    themeLayout->addRow("Font Color:", fontColorButton);

    uiLayout->addLayout(themeLayout);
    uiLayout->addWidget(minimizeToTrayCheckBox);
    uiLayout->addWidget(startMinimizedCheckBox);
    uiLayout->addWidget(textShadowCheckBox);
    uiLayout->addWidget(shadowGroupBox);
    uiLayout->addStretch();

    // Add tabs
    tabWidget->addTab(timerTab, "Timer");
    tabWidget->addTab(notificationsTab, "Notifications");
    tabWidget->addTab(uiTab, "UI");

    // Button box
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);

    // Layout
    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);

    // Connect signals
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, [=]()
    {
        // Apply settings
        m_settings->setWorkDuration(workDurationSpinBox->value());
        m_settings->setShortBreakDuration(shortBreakDurationSpinBox->value());
        m_settings->setLongBreakDuration(longBreakDurationSpinBox->value());
        m_settings->setLongBreakInterval(longBreakIntervalSpinBox->value());

        m_settings->setSoundEnabled(soundEnabledCheckBox->isChecked());
        m_settings->setDesktopNotificationsEnabled(desktopNotificationsEnabledCheckBox->isChecked());

        m_settings->setTheme(themeComboBox->currentData().toString());
        m_settings->setMinimizeToTray(minimizeToTrayCheckBox->isChecked());
        m_settings->setStartMinimized(startMinimizedCheckBox->isChecked());

        // Apply new font settings
        m_settings->setFontSize(fontSizeSpinBox->value());

        // Get color from button's stylesheet - extract the color value
        QString fontColorStyle = fontColorButton->styleSheet();
        int colorStart = fontColorStyle.indexOf("background-color:") + 17;
        int colorEnd = fontColorStyle.indexOf(";", colorStart);
        QString fontColor = fontColorStyle.mid(colorStart, colorEnd - colorStart).trimmed();
        m_settings->setFontColor(fontColor);

        m_settings->setTextShadowEnabled(textShadowCheckBox->isChecked());

        // Get shadow color from button's stylesheet
        QString shadowColorStyle = shadowColorButton->styleSheet();
        colorStart = shadowColorStyle.indexOf("background-color:") + 17;
        colorEnd = shadowColorStyle.indexOf(";", colorStart);
        QString shadowColor = shadowColorStyle.mid(colorStart, colorEnd - colorStart).trimmed();
        m_settings->setTextShadowColor(shadowColor);

        m_settings->setTextShadowBlur(shadowBlurSpinBox->value());
        m_settings->setTextShadowOffsetX(shadowOffsetXSpinBox->value());
        m_settings->setTextShadowOffsetY(shadowOffsetYSpinBox->value());


        m_settings->saveSettings();
        applySettings();
    });

    // Show dialog
    if (dialog.exec() == QDialog::Accepted)
    {
        // Apply settings
        m_settings->setWorkDuration(workDurationSpinBox->value());
        m_settings->setShortBreakDuration(shortBreakDurationSpinBox->value());
        m_settings->setLongBreakDuration(longBreakDurationSpinBox->value());
        m_settings->setLongBreakInterval(longBreakIntervalSpinBox->value());

        m_settings->setSoundEnabled(soundEnabledCheckBox->isChecked());
        m_settings->setDesktopNotificationsEnabled(desktopNotificationsEnabledCheckBox->isChecked());

        m_settings->setTheme(themeComboBox->currentData().toString());
        m_settings->setMinimizeToTray(minimizeToTrayCheckBox->isChecked());
        m_settings->setStartMinimized(startMinimizedCheckBox->isChecked());

        // Apply new font settings
        m_settings->setFontSize(fontSizeSpinBox->value());

        // Get color from button's stylesheet - extract the color value
        QString fontColorStyle = fontColorButton->styleSheet();
        int colorStart = fontColorStyle.indexOf("background-color:") + 17;
        int colorEnd = fontColorStyle.indexOf(";", colorStart);
        QString fontColor = fontColorStyle.mid(colorStart, colorEnd - colorStart).trimmed();
        m_settings->setFontColor(fontColor);

        m_settings->setTextShadowEnabled(textShadowCheckBox->isChecked());

        // Get shadow color from button's stylesheet
        QString shadowColorStyle = shadowColorButton->styleSheet();
        colorStart = shadowColorStyle.indexOf("background-color:") + 17;
        colorEnd = shadowColorStyle.indexOf(";", colorStart);
        QString shadowColor = shadowColorStyle.mid(colorStart, colorEnd - colorStart).trimmed();
        m_settings->setTextShadowColor(shadowColor);

        m_settings->setTextShadowBlur(shadowBlurSpinBox->value());
        m_settings->setTextShadowOffsetX(shadowOffsetXSpinBox->value());
        m_settings->setTextShadowOffsetY(shadowOffsetYSpinBox->value());

        m_settings->saveSettings();
        applySettings();
    }
}
