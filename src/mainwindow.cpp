//
// Created by zigameni on 3/9/25.
//

#include "mainwindow.h"
#include <QCloseEvent>
#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QTabWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_timer(new Timer(this))
    , m_settings(new Settings(this))
#ifdef HAVE_QT_MULTIMEDIA
    , m_mediaPlayer(new QMediaPlayer(this))
//    , m_audioOutput(new QAudioOutput(this))
#endif
    , m_totalTime(0)
{
    setupUi();
    setupTrayIcon();
    setupConnections();

    // Apply settings
    applySettings();

    // Initialize timer display
    // updateTimerDisplay(m_timer->getRemainingTime());
    // handleModeChanged(m_timer->getMode());
    // handleStateChanged(m_timer->getState());
    handlePomodorosCompletedChanged(m_timer->getTotalCompletedPomodoros());

    // Set window title
    setWindowTitle("Ziga-Pomodoro");
    setWindowIcon(QIcon(":/icons/tomato.png"));

#ifdef HAVE_QT_MULTIMEDIA
    // Configure media player
//    m_mediaPlayer->setAudioOutput(m_audioOutput);
        m_mediaPlayer->setVolume(100);
#endif
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
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


void MainWindow::setupTrayIcon() {
    // Create tray icon
    m_trayIcon = new QSystemTrayIcon(QIcon(":/icons/tomato.png"), this);

    // Create tray menu
    m_trayMenu = new QMenu(this);

    // Create actions
    m_showAction = new QAction("Show", this);
    // m_startAction = new QAction("Start", this);
    // m_pauseAction = new QAction("Pause", this);
    // m_resetAction = new QAction("Reset", this);
    m_quitAction = new QAction("Quit", this);

    // Add actions to menu
    m_trayMenu->addAction(m_showAction);
    m_trayMenu->addSeparator();
    // m_trayMenu->addAction(m_startAction);
    // m_trayMenu->addAction(m_pauseAction);
    // m_trayMenu->addAction(m_resetAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(m_quitAction);

    // Set tray menu
    m_trayIcon->setContextMenu(m_trayMenu);

    // Show tray icon
    m_trayIcon->show();
}

// In mainwindow.cpp - setupConnections()
void MainWindow::setupConnections() {
    // Connect timer signals (for statistics only)
    connect(m_timer, &Timer::pomodorosCompletedChanged, this, &MainWindow::handlePomodorosCompletedChanged);

    // Connect settings button
    connect(m_settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);

    // Connect tray icon signals
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
    connect(m_showAction, &QAction::triggered, this, &QWidget::show);
    connect(m_quitAction, &QAction::triggered, qApp, &QApplication::quit);
}

// void MainWindow::updateTimerDisplay(int remainingSeconds) {
//     int minutes = remainingSeconds / 60;
//     int seconds = remainingSeconds % 60;
//     m_timerLabel->setText(QString("%1:%2")
//                              .arg(minutes, 2, 10, QChar('0'))
//                              .arg(seconds, 2, 10, QChar('0')));
//
//     // Update progress bar
//     if (m_totalTime > 0) {
//         int progress = (static_cast<float>(remainingSeconds) / m_totalTime) * 100;
//         m_progressBar->setValue(progress);
//     }
//
//     // Update tray tooltip
//     updateWindowTitle();
// }

// void MainWindow::handleTimerCompleted(Timer::TimerMode mode) {
//     // Play notification sound
//     playNotificationSound();
//
//     // Show desktop notification
//     QString title;
//     QString message;
//
//     switch (mode) {
//         case Timer::TimerMode::Work:
//             title = "Work Session Completed";
//             message = "Time for a break!";
//             break;
//
//         case Timer::TimerMode::ShortBreak:
//             title = "Break Completed";
//             message = "Time to get back to work!";
//             break;
//
//         case Timer::TimerMode::LongBreak:
//             title = "Long Break Completed";
//             message = "Time to start a new work session!";
//             break;
//     }
//
//     showDesktopNotification(title, message);
// }

// void MainWindow::handleModeChanged(Timer::TimerMode mode) {
//     QString modeText;
//     QString modeStyle;
//
//     switch (mode) {
//         case Timer::TimerMode::Work:
//             modeText = "Work";
//             modeStyle = "color: #E74C3C;"; // Red
//             m_totalTime = m_settings->getWorkDuration() * 60;
//             break;
//
//         case Timer::TimerMode::ShortBreak:
//             modeText = "Short Break";
//             modeStyle = "color: #2ECC71;"; // Green
//             m_totalTime = m_settings->getShortBreakDuration() * 60;
//             break;
//
//         case Timer::TimerMode::LongBreak:
//             modeText = "Long Break";
//             modeStyle = "color: #3498DB;"; // Blue
//             m_totalTime = m_settings->getLongBreakDuration() * 60;
//             break;
//     }
//
//     m_modeLabel->setText(modeText);
//     m_modeLabel->setStyleSheet(modeStyle);
//
//     updateWindowTitle();
// }

// void MainWindow::handleStateChanged(Timer::TimerState state) {
//     switch (state) {
//         case Timer::TimerState::Running:
//             m_startButton->setEnabled(false);
//             m_pauseButton->setEnabled(true);
//             m_resetButton->setEnabled(true);
//             m_skipButton->setEnabled(true);
//             m_startAction->setEnabled(false);
//             m_pauseAction->setEnabled(true);
//             m_resetAction->setEnabled(true);
//             break;
//
//         case Timer::TimerState::Paused:
//             m_startButton->setEnabled(true);
//             m_pauseButton->setEnabled(false);
//             m_resetButton->setEnabled(true);
//             m_skipButton->setEnabled(true);
//             m_startAction->setEnabled(true);
//             m_pauseAction->setEnabled(false);
//             m_resetAction->setEnabled(true);
//             break;
//
//         case Timer::TimerState::Stopped:
//             m_startButton->setEnabled(true);
//             m_pauseButton->setEnabled(false);
//             m_resetButton->setEnabled(false);
//             m_skipButton->setEnabled(true);
//             m_startAction->setEnabled(true);
//             m_pauseAction->setEnabled(false);
//             m_resetAction->setEnabled(false);
//             break;
//     }
//
//     updateWindowTitle();
// }

void MainWindow::handlePomodorosCompletedChanged(int count) {
    m_pomodorosCompletedLabel->setText(QString("Pomodoros completed: %1").arg(count));
}

// void MainWindow::onStartButtonClicked() {
//     m_timer->start();
// }

// void MainWindow::onPauseButtonClicked() {
//     m_timer->pause();
// }
//
// void MainWindow::onResetButtonClicked() {
//     m_timer->reset();
// }
//
// void MainWindow::onSkipButtonClicked() {
//     m_timer->skipToNext();
// }

void MainWindow::onSettingsButtonClicked() {
    showSettingsDialog();
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        if (isVisible()) {
            hide();
        } else {
            show();
            activateWindow();
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Always just hide the window instead of closing the application
    hide();
    event->ignore();
}

void MainWindow::playNotificationSound() {
#ifdef HAVE_QT_MULTIMEDIA
    if (m_settings->getSoundEnabled()) {
        m_mediaPlayer->setMedia(QUrl::fromLocalFile(m_settings->getSoundFile()));
        m_mediaPlayer->play();
    }
#endif
}

void MainWindow::showDesktopNotification(const QString &title, const QString &message) {
    if (m_settings->getDesktopNotificationsEnabled()) {
        m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
    }
}

void MainWindow::updateWindowTitle() {
    QString stateStr;
    switch (m_timer->getState()) {
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
    switch (m_timer->getMode()) {
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

void MainWindow::applySettings() {
    // Apply timer settings
    m_timer->setWorkDuration(m_settings->getWorkDuration());
    m_timer->setShortBreakDuration(m_settings->getShortBreakDuration());
    m_timer->setLongBreakDuration(m_settings->getLongBreakDuration());
    m_timer->setLongBreakInterval(m_settings->getLongBreakInterval());

    // Apply UI settings
    loadStyleSheet(m_settings->getTheme());

    // Update display
    // handleModeChanged(m_timer->getMode());
    // updateTimerDisplay(m_timer->getRemainingTime());
}

void MainWindow::loadStyleSheet(const QString &theme) {
    QString stylePath = QString(":/styles/%1.qss").arg(theme);
    QFile file(stylePath);

    if (file.exists() && file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(file.readAll());
        setStyleSheet(styleSheet);
        file.close();
    } else {
        setStyleSheet("");
    }
}

void MainWindow::showSettingsDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Settings");

    QTabWidget *tabWidget = new QTabWidget(&dialog);

    // Timer tab
    QWidget *timerTab = new QWidget(tabWidget);
    QFormLayout *timerLayout = new QFormLayout(timerTab);

    QSpinBox *workDurationSpinBox = new QSpinBox(timerTab);
    workDurationSpinBox->setMinimum(1);
    workDurationSpinBox->setMaximum(60);
    workDurationSpinBox->setValue(m_settings->getWorkDuration());

    QSpinBox *shortBreakDurationSpinBox = new QSpinBox(timerTab);
    shortBreakDurationSpinBox->setMinimum(1);
    shortBreakDurationSpinBox->setMaximum(30);
    shortBreakDurationSpinBox->setValue(m_settings->getShortBreakDuration());

    QSpinBox *longBreakDurationSpinBox = new QSpinBox(timerTab);
    longBreakDurationSpinBox->setMinimum(1);
    longBreakDurationSpinBox->setMaximum(60);
    longBreakDurationSpinBox->setValue(m_settings->getLongBreakDuration());

    QSpinBox *longBreakIntervalSpinBox = new QSpinBox(timerTab);
    longBreakIntervalSpinBox->setMinimum(1);
    longBreakIntervalSpinBox->setMaximum(10);
    longBreakIntervalSpinBox->setValue(m_settings->getLongBreakInterval());

    timerLayout->addRow("Work duration (minutes):", workDurationSpinBox);
    timerLayout->addRow("Short break duration (minutes):", shortBreakDurationSpinBox);
    timerLayout->addRow("Long break duration (minutes):", longBreakDurationSpinBox);
    timerLayout->addRow("Long break interval (pomodoros):", longBreakIntervalSpinBox);

    // Notifications tab
    QWidget *notificationsTab = new QWidget(tabWidget);
    QVBoxLayout *notificationsLayout = new QVBoxLayout(notificationsTab);

    QCheckBox *soundEnabledCheckBox = new QCheckBox("Enable sound notifications", notificationsTab);
    soundEnabledCheckBox->setChecked(m_settings->getSoundEnabled());

    QCheckBox *desktopNotificationsEnabledCheckBox = new QCheckBox("Enable desktop notifications", notificationsTab);
    desktopNotificationsEnabledCheckBox->setChecked(m_settings->getDesktopNotificationsEnabled());

    notificationsLayout->addWidget(soundEnabledCheckBox);
    notificationsLayout->addWidget(desktopNotificationsEnabledCheckBox);
    notificationsLayout->addStretch();

    // UI tab
    QWidget *uiTab = new QWidget(tabWidget);
    QVBoxLayout *uiLayout = new QVBoxLayout(uiTab);

    QComboBox *themeComboBox = new QComboBox(uiTab);
    themeComboBox->addItem("Default", "default");
    themeComboBox->setCurrentText("Default");

    QCheckBox *minimizeToTrayCheckBox = new QCheckBox("Minimize to tray when closed", uiTab);
    minimizeToTrayCheckBox->setChecked(m_settings->getMinimizeToTray());

    QCheckBox *startMinimizedCheckBox = new QCheckBox("Start minimized", uiTab);
    startMinimizedCheckBox->setChecked(m_settings->getStartMinimized());

    QFormLayout *themeLayout = new QFormLayout();
    themeLayout->addRow("Theme:", themeComboBox);

    uiLayout->addLayout(themeLayout);
    uiLayout->addWidget(minimizeToTrayCheckBox);
    uiLayout->addWidget(startMinimizedCheckBox);
    uiLayout->addStretch();

    // Add tabs
    tabWidget->addTab(timerTab, "Timer");
    tabWidget->addTab(notificationsTab, "Notifications");
    tabWidget->addTab(uiTab, "UI");

    // Button box
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);

    // Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);

    // Connect signals
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, [=]() {
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

        m_settings->saveSettings();
        applySettings();
    });

    // Show dialog
    if (dialog.exec() == QDialog::Accepted) {
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

        m_settings->saveSettings();
        applySettings();
    }
}