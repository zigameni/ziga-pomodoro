//
// Created by zigameni on 3/9/25.
//

#include "timerwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QStyle>
#include <QGraphicsDropShadowEffect>
#include <QMediaPlayer> // Make sure these are included at the top of timerwindow.cpp
#include <QSystemTrayIcon>

TimerWindow::TimerWindow(Settings* settings, QWidget* parent)
    : QWidget(parent)
      , m_appSettings(settings)
      , m_timer(new Timer(this))
      , m_mainWindow(nullptr)
{
    m_dbManager = nullptr;
    m_hasDbManager = false;
    m_sessionStartTime = QDateTime();


    // Set window flags for a frameless, always-on-top window
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    setupUi();
    setupConnections();

    // Initialize timer with durations from settings - NOW USING m_appSettings
    m_timer->setWorkDuration(m_appSettings->getWorkDuration()); // Convert minutes to seconds
    m_timer->setShortBreakDuration(m_appSettings->getShortBreakDuration());
    m_timer->setLongBreakDuration(m_appSettings->getLongBreakDuration());
    m_timer->setLongBreakInterval(m_appSettings->getLongBreakInterval());
    // Initialize timer display
    updateTimerDisplay(m_timer->getRemainingTime());
    handleModeChanged(m_timer->getMode());
    handleStateChanged(m_timer->getState());

    m_mediaPlayer = new QMediaPlayer(this);
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon::fromTheme("ziga-pomodoro-icon", QIcon(":/icons/tomato.png")));
    // Replace with your icon
    m_trayIcon->show(); // Show tray icon (or hide it if you only want notifications)
}

TimerWindow::~TimerWindow() = default;


void TimerWindow::setupUi()
{
    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    // Create timer display label
    m_timerLabel = new QLabel("25:00", this);
    QFont timerFont = m_timerLabel->font();
    timerFont.setPointSize(m_appSettings->getFontSize());
    timerFont.setBold(true);
    m_timerLabel->setFont(timerFont);
    m_timerLabel->setAlignment(Qt::AlignCenter);

    updateTimerLabelStyle(); // New method to update the label style

    // m_timerLabel->setStyleSheet("color: white;"); // Only color, no background
    m_mainLayout->addWidget(m_timerLabel);

    // Create button layout
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(5);
    m_buttonLayout->setContentsMargins(0, 0, 0, 0);

    // Button style: fully transparent, visible on hover
    QString buttonStyle = R"(
        QPushButton {
            color: white;
            background-color: rgba(0, 0, 0, 0); /* Fully transparent */
            border-radius: 16px;
            padding: 4px;
        }
        QPushButton:hover {
            background-color: rgba(40, 40, 40, 80); /* Visible only on hover */
        }
        QPushButton:pressed {
            background-color: rgba(80, 80, 80, 150);
        }
    )";

    // Create Start/Pause Button
    m_startPauseButton = new QPushButton(this);
    m_startPauseButton->setStyleSheet(buttonStyle);
    m_startPauseButton->setIconSize(QSize(32, 32));

    // Initially transparent icon
    QPixmap pixmap = m_startPauseButton->icon().pixmap(32, 32);
    QPixmap transparentPixmap(pixmap.size());
    transparentPixmap.fill(Qt::transparent);
    QPainter painter(&transparentPixmap);
    painter.setOpacity(0.0); // Fully transparent
    painter.drawPixmap(0, 0, pixmap);
    painter.end();
    m_startPauseButton->setIcon(QIcon(transparentPixmap));

    // Create Stop Button
    m_stopButton = new QPushButton(this);
    m_stopButton->setIcon(QIcon::fromTheme("media-playback-stop",
                                           QApplication::style()->standardIcon(QStyle::SP_MediaStop)));
    m_stopButton->setToolTip("Stop");
    m_stopButton->setIconSize(QSize(32, 32));
    m_stopButton->setFlat(true);
    m_stopButton->setEnabled(false);
    m_stopButton->setStyleSheet(buttonStyle);


    // Create Skip Button (after creating the other buttons)
    m_skipButton = new QPushButton(this);
    m_skipButton->setIcon(QIcon::fromTheme("media-skip-forward",
                                           QApplication::style()->standardIcon(QStyle::SP_MediaSkipForward)));
    m_skipButton->setToolTip("Skip Break");
    m_skipButton->setIconSize(QSize(32, 32));
    m_skipButton->setFlat(true);
    m_skipButton->setEnabled(false); // Initially disabled
    m_skipButton->setStyleSheet(buttonStyle);

    // Add to button layout (place it between stop and settings button)


    // Create Settings Button
    m_settingsButton = new QPushButton(this);
    m_settingsButton->setIcon(QIcon::fromTheme("preferences-system",
                                               QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView)));
    m_settingsButton->setToolTip("Settings");
    m_settingsButton->setIconSize(QSize(32, 32));
    m_settingsButton->setFlat(true);
    m_settingsButton->setStyleSheet(buttonStyle);

    // Add buttons to layout
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_startPauseButton);
    m_buttonLayout->addWidget(m_stopButton);
    m_buttonLayout->addWidget(m_skipButton);
    m_buttonLayout->addWidget(m_settingsButton);
    m_buttonLayout->addStretch();

    m_startPauseButton->hide(); // Initially hide buttons
    m_stopButton->hide();
    m_settingsButton->hide();

    m_mainLayout->addLayout(m_buttonLayout);

    // Create Close Button
    m_closeButton = new QPushButton("×", this);
    m_closeButton->setFixedSize(24, 24);
    m_closeButton->setToolTip("Close");
    m_closeButton->setFlat(true);

    // Close button style (matching hover effect)
    m_closeButton->setStyleSheet(R"(
        QPushButton {
            color: white;
            background-color: rgba(255, 255, 255, 0); /* Fully transparent */
            border-radius: 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 30);
        }
        QPushButton:pressed {
            background-color: rgba(255, 255, 255, 60);
        }
    )");

    // Position close button in top-right corner
    m_closeButton->move(this->width() - 30, 5);

    // Set size
    resize(300, 150);
}

void TimerWindow::updateTimerLabelStyle()
{
    // Get font size
    int fontSize = m_appSettings->getFontSize();

    // Set font color
    QString styleSheet = QString("color: %1;").arg(m_appSettings->getFontColor());

    // Apply background color for break modes
    if (m_timer->getMode() == Timer::TimerMode::ShortBreak ||
        m_timer->getMode() == Timer::TimerMode::LongBreak)
    {
        styleSheet += " background-color: rgba(46, 204, 113, 150);"; // Green background
    }

    m_timerLabel->setStyleSheet(styleSheet);

    // Apply text shadow if enabled
    if (m_appSettings->getTextShadowEnabled())
    {
        auto shadowEffect = new QGraphicsDropShadowEffect();
        shadowEffect->setOffset(m_appSettings->getTextShadowOffsetX(),
                                m_appSettings->getTextShadowOffsetY());
        shadowEffect->setBlurRadius(m_appSettings->getTextShadowBlur());
        shadowEffect->setColor(QColor(m_appSettings->getTextShadowColor()));

        m_timerLabel->setGraphicsEffect(shadowEffect);
    }
    else
    {
        m_timerLabel->setGraphicsEffect(nullptr); // Remove effect if disabled
    }

    // Adjust the window size based on font size and layout
    updateWindowSize();
    qDebug() << "Applied style:" << styleSheet;
}


void TimerWindow::setupConnections()
{
    // Connect timer signals
    connect(m_timer, &Timer::timerTick, this, &TimerWindow::updateTimerDisplay);
    connect(m_timer, &Timer::modeChanged, this, &TimerWindow::handleModeChanged);
    connect(m_timer, &Timer::stateChanged, this, &TimerWindow::handleStateChanged);

    // Connect button signals
    connect(m_startPauseButton, &QPushButton::clicked, this, &TimerWindow::onStartPauseButtonClicked);
    connect(m_stopButton, &QPushButton::clicked, this, &TimerWindow::onStopButtonClicked);
    connect(m_settingsButton, &QPushButton::clicked, this, &TimerWindow::onSettingsButtonClicked);
    connect(m_skipButton, &QPushButton::clicked, this, &TimerWindow::onSkipButtonClicked);


    // connect(m_closeButton, &QPushButton::clicked, this, &QWidget::close);
    // connect(m_closeButton, &QPushButton::clicked, qApp, &QApplication::quit);

    // Connect Settings::settingsChanged signal to TimerWindow::onSettingsChanged slot
    connect(m_appSettings, &Settings::settingsChanged, this, &TimerWindow::onSettingsChanged);
    // connect(m_closeButton, &QPushButton::clicked, this, &QWidget::close);
    connect(m_closeButton, &QPushButton::clicked, qApp, &QApplication::quit);

    connect(m_timer, &Timer::timerCompleted, this, &TimerWindow::handleTimerCompleted);
}

void TimerWindow::updateTimerDisplay(int remainingSeconds)
{
    int minutes = remainingSeconds / 60;
    int seconds = remainingSeconds % 60;
    m_timerLabel->setText(QString("%1:%2")
                          .arg(minutes, 2, 10, QChar('0'))
                          .arg(seconds, 2, 10, QChar('0')));
}

void TimerWindow::handleModeChanged(Timer::TimerMode mode)
{
    // Update the style with the current mode
    updateTimerLabelStyle();

    // Update the skip button state
    switch (mode)
    {
    case Timer::TimerMode::Work:
        m_skipButton->setEnabled(false); // Disable skip during work
        break;
    case Timer::TimerMode::ShortBreak:
    case Timer::TimerMode::LongBreak:
        m_skipButton->setEnabled(true); // Enable skip during breaks
        break;
    }
}

void TimerWindow::handleStateChanged(Timer::TimerState state)
{
    switch (state)
    {
    case Timer::TimerState::Running:
        m_stopButton->setEnabled(true);
        break;

    case Timer::TimerState::Paused:
        m_stopButton->setEnabled(true);
        break;

    case Timer::TimerState::Stopped:
        m_stopButton->setEnabled(false);
        break;
    }

    // Add this for database tracking
    if (state == Timer::TimerState::Running && m_timer->getElapsedTime() == 0)
    {
        // Just started
        m_sessionStartTime = QDateTime::currentDateTime();
    }
    updateStartPauseButton();
}

void TimerWindow::updateStartPauseButton()
{
    if (m_timer->getState() == Timer::TimerState::Running)
    {
        m_startPauseButton->setIcon(QIcon::fromTheme("media-playback-pause",
                                                     QApplication::style()->standardIcon(QStyle::SP_MediaPause)));
        m_startPauseButton->setToolTip("Pause");
    }
    else
    {
        m_startPauseButton->setIcon(QIcon::fromTheme("media-playback-start",
                                                     QApplication::style()->standardIcon(QStyle::SP_MediaPlay)));
        m_startPauseButton->setToolTip("Start");
    }
}

void TimerWindow::updateStartPauseIcon()
{
    if (isRunning)
    {
        m_startPauseButton->setIcon(QIcon::fromTheme("media-playback-pause",
                                                     QApplication::style()->standardIcon(QStyle::SP_MediaPause)));
        m_startPauseButton->setToolTip("Pause");
    }
    else
    {
        m_startPauseButton->setIcon(QIcon::fromTheme("media-playback-start",
                                                     QApplication::style()->standardIcon(QStyle::SP_MediaPlay)));
        m_startPauseButton->setToolTip("Start");
    }
}

void TimerWindow::onStartPauseButtonClicked()
{
    if (m_timer->getState() == Timer::TimerState::Running)
    {
        m_timer->pause();
    }
    else
    {
        m_timer->start();
    }
}

void TimerWindow::onStopButtonClicked()
{
    // Add this for database tracking of interrupted sessions
    if (m_hasDbManager && m_dbManager && !m_sessionStartTime.isNull() &&
        m_timer->getState() != Timer::TimerState::Stopped)
    {
        QDateTime endTime = QDateTime::currentDateTime();
        int duration = m_sessionStartTime.secsTo(endTime);

        if (m_timer->getMode() == Timer::TimerMode::Work)
        {
            m_dbManager->recordPomodoroSession(m_sessionStartTime, duration, false); // incomplete
        }
    }
    m_timer->reset();
}

// void TimerWindow::setDatabaseManager(DatabaseManager* dbManager)
// {
//     m_dbManager = dbManager;
//     m_hasDbManager = (dbManager != nullptr && dbManager->isInitialized());
// }
// This should go in the setDatabaseManager method in TimerWindow
void TimerWindow::setDatabaseManager(DatabaseManager* dbManager)
{
    m_dbManager = dbManager;
    m_hasDbManager = (dbManager != nullptr && dbManager->isInitialized());

    // If MainWindow is already created, pass the database manager to it
    if (m_mainWindow)
    {
        m_mainWindow->setDatabaseManager(dbManager);
    }
}

// Modify the onSettingsButtonClicked method to include the database manager
void TimerWindow::onSettingsButtonClicked()
{
    if (!m_mainWindow)
    {
        m_mainWindow = new MainWindow(m_appSettings, nullptr);

        // If we have a database manager, pass it to the MainWindow
        if (m_hasDbManager && m_dbManager)
        {
            m_mainWindow->setDatabaseManager(m_dbManager);
        }
    }

    m_mainWindow->show();
    m_mainWindow->raise();
    m_mainWindow->activateWindow();
}

void TimerWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
}

void TimerWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void TimerWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void TimerWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    // Keep close button in top-right corner
    if (m_closeButton)
    {
        m_closeButton->move(this->width() - 30, 5);
    }
}

void TimerWindow::enterEvent(QEvent* event)
{
    m_startPauseButton->show();
    m_stopButton->show();
    m_skipButton->show(); // Show skip button
    m_settingsButton->show();
    updateStartPauseButton();
    QWidget::enterEvent(event);
}

void TimerWindow::leaveEvent(QEvent* event)
{
    m_startPauseButton->hide();
    m_stopButton->hide();
    m_skipButton->hide(); // Hide skip button
    m_settingsButton->hide();
    QWidget::leaveEvent(event);
}

void TimerWindow::onSettingsChanged() // <----- IMPLEMENT onSettingsChanged SLOT
{
    // Timer settings
    m_timer->setWorkDuration(m_appSettings->getWorkDuration());
    m_timer->setShortBreakDuration(m_appSettings->getShortBreakDuration());
    m_timer->setLongBreakDuration(m_appSettings->getLongBreakDuration());
    m_timer->setLongBreakInterval(m_appSettings->getLongBreakInterval());

    // Font settings
    QFont timerFont = m_timerLabel->font();
    timerFont.setPointSize(m_appSettings->getFontSize());
    m_timerLabel->setFont(timerFont);

    // Update label style
    updateTimerLabelStyle();

    // Force an immediate update of the timer display
    updateTimerDisplay(m_timer->getRemainingTime());

    // Save settings to ensure they persist
    m_appSettings->saveSettings();

    // Force UI update
    update();;
}

void TimerWindow::onSkipButtonClicked()
{
    // Skip current break and move to next work session
    if (m_timer->getMode() == Timer::TimerMode::ShortBreak ||
        m_timer->getMode() == Timer::TimerMode::LongBreak)
    {
        m_timer->skipBreak(); // We need to add this method to the Timer class
    }
}

void TimerWindow::updateWindowSize()
{
    int fontSize = m_appSettings->getFontSize();

    // Calculate text size based on font size
    int textWidth = fontSize * 3.5; // Adjusted width for "XX:XX"
    int textHeight = fontSize * 2; // Adjusted height for compact view

    // Add small padding
    int padding = 10;

    // Resize the window to tightly fit the timer text
    resize(textWidth + padding, textHeight + padding);

    qDebug() << "Window resized to:" << width() << "x" << height();
}

// Add this method to your TimerWindow implementation

// Implement the new slot for timer completion
void TimerWindow::handleTimerCompleted(Timer::TimerMode completedMode)
{
    if (!m_hasDbManager || !m_dbManager)
    {
        return; // Skip database recording if no DB manager
    }

    QDateTime endTime = QDateTime::currentDateTime();
    int duration = m_sessionStartTime.secsTo(endTime);

    if (completedMode == Timer::TimerMode::Work)
    {
        m_dbManager->recordPomodoroSession(m_sessionStartTime, duration, true);
    }
    else if (completedMode == Timer::TimerMode::ShortBreak)
    {
        m_dbManager->recordBreakSession(m_sessionStartTime, duration, false);
    }
    else if (completedMode == Timer::TimerMode::LongBreak)
    {
        m_dbManager->recordBreakSession(m_sessionStartTime, duration, true);
    }

    QString title;
    QString message;

    if (completedMode == Timer::TimerMode::Work)
    {
        title = "Work Session Complete!";
        message = "Time for a break.";
    }
    else if (completedMode == Timer::TimerMode::ShortBreak || completedMode == Timer::TimerMode::LongBreak)
    {
        title = "Break Over!";
        message = "Time to get back to work.";
    }

    // Show notification - using existing setting name
    if (m_appSettings->getDesktopNotificationsEnabled())
    {
        showDesktopNotification(title, message);
    }

    // Play sound - using existing setting name
    if (m_appSettings->getSoundEnabled())
    {
        playNotificationSound();
    }
}

// void TimerWindow::playNotificationSound()
// {
// #ifdef HAVE_QT_MULTIMEDIA // Keep the conditional compilation if it's important for your build
//     if (m_appSettings->getSoundEnabled()) // Use m_appSettings here
//     {
//         m_mediaPlayer->setMedia(QUrl::fromLocalFile(m_appSettings->getSoundFile())); // Use m_appSettings here
//         m_mediaPlayer->play();
//     }
// #endif
// }
//

void TimerWindow::playNotificationSound()
{
#ifdef HAVE_QT_MULTIMEDIA
    if (m_appSettings->getSoundEnabled()) // Use m_appSettings
    {
        m_mediaPlayer->setMedia(QUrl::fromLocalFile(m_appSettings->getSoundFile())); // Use m_appSettings
        m_mediaPlayer->play();
    }
#endif
}

void TimerWindow::showDesktopNotification(const QString& title, const QString& message)
{
    if (m_appSettings->getDesktopNotificationsEnabled()) // Use m_appSettings, keep existing name
    {
        m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
    }
}
