//
// Created by zigameni on 3/9/25.
//

#include "timerwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QStyle>

TimerWindow::TimerWindow(QWidget* parent)
    : QWidget(parent)
      , m_timer(new Timer(this))
      , m_mainWindow(nullptr)
{
    // Set window flags for a frameless, always-on-top window
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    setupUi();
    setupConnections();

    // Initialize timer display
    updateTimerDisplay(m_timer->getRemainingTime());
    handleModeChanged(m_timer->getMode());
    handleStateChanged(m_timer->getState());
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
    timerFont.setPointSize(48);
    timerFont.setBold(true);
    m_timerLabel->setFont(timerFont);
    m_timerLabel->setAlignment(Qt::AlignCenter);
    m_timerLabel->setStyleSheet("color: white;"); // Only color, no background
    m_mainLayout->addWidget(m_timerLabel);

    // Create button layout
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(10);
    m_buttonLayout->setContentsMargins(0, 0, 0, 0);

    // Update the button styles to remove background or make it very transparent
    // QString buttonStyle =
    //     "QPushButton { color: white; background-color: rgba(0, 0, 0, 30); border-radius: 16px; padding: 8px; } "
    //     "QPushButton:hover { background-color: rgba(40, 40, 40, 80); }";
    QString buttonStyle =
        "QPushButton { color: white; background-color: rgba(0, 0, 0, 25); border-radius: 16px; padding: 8px; opacity: 0.1; } "
        "QPushButton:hover { background-color: rgba(40, 40, 40, 80); opacity: 1.0; }";
    // Create control buttons with icons
    m_startPauseButton = new QPushButton(this);

    // Then set the icons with lower opacity initially
    m_startPauseButton->setIcon(QIcon::fromTheme("media-playback-start",
                                                 QApplication::style()->standardIcon(QStyle::SP_MediaPlay)));
    m_startPauseButton->setIconSize(QSize(32, 32));
    m_startPauseButton->setStyleSheet(buttonStyle);

    // Set the icon opacity (0.1 = 10% opacity)
    QPixmap pixmap = m_startPauseButton->icon().pixmap(32, 32);
    QPixmap transparentPixmap(pixmap.size());
    transparentPixmap.fill(Qt::transparent);
    QPainter painter(&transparentPixmap);
    painter.setOpacity(0.1); // 10% opacity
    painter.drawPixmap(0, 0, pixmap);
    painter.end();
    m_startPauseButton->setIcon(QIcon(transparentPixmap));

    // m_startPauseButton->setIcon(QIcon::fromTheme("media-playback-start",
    //                                              QApplication::style()->standardIcon(QStyle::SP_MediaPlay)));
    // m_startPauseButton->setToolTip("Start");
    // m_startPauseButton->setIconSize(QSize(32, 32));
    // m_startPauseButton->setFlat(true);
    // m_startPauseButton->setStyleSheet(buttonStyle);

    m_stopButton = new QPushButton(this);
    m_stopButton->setIcon(QIcon::fromTheme("media-playback-stop",
                                           QApplication::style()->standardIcon(QStyle::SP_MediaStop)));
    m_stopButton->setToolTip("Stop");
    m_stopButton->setIconSize(QSize(32, 32));
    m_stopButton->setFlat(true);
    m_stopButton->setEnabled(false);
    m_stopButton->setStyleSheet(buttonStyle);

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
    m_buttonLayout->addWidget(m_settingsButton);
    m_buttonLayout->addStretch();

    m_mainLayout->addLayout(m_buttonLayout);

    // Create close button
    m_closeButton = new QPushButton("×", this);
    m_closeButton->setFixedSize(24, 24);
    m_closeButton->setToolTip("Close");
    m_closeButton->setFlat(true);


    m_closeButton->setStyleSheet(
        "QPushButton { color: white; background-color: rgba(255, 255, 255, 10); border-radius: 12px; font-weight: bold; } "
        "QPushButton:hover { background-color: rgba(255, 255, 255, 30); }");


    // Position close button in top-right corner
    m_closeButton->move(this->width() - 30, 5);

    // Set size
    resize(300, 150);
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

    connect(m_closeButton, &QPushButton::clicked, this, &QWidget::close);
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
    QString colorStyle;

    switch (mode)
    {
    case Timer::TimerMode::Work:
        colorStyle = "color: white; background-color: rgba(231, 76, 60, 100);"; // Red
        break;

    case Timer::TimerMode::ShortBreak:
        colorStyle = "color: white; background-color: rgba(46, 204, 113, 100);"; // Green
        break;

    case Timer::TimerMode::LongBreak:
        colorStyle = "color: white; background-color: rgba(52, 152, 219, 100);"; // Blue
        break;
    }

    m_timerLabel->setStyleSheet(colorStyle);
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
    m_timer->reset();
}

void TimerWindow::onSettingsButtonClicked()
{
    if (!m_mainWindow)
    {
        m_mainWindow = new MainWindow(nullptr); // Use nullptr instead of this to avoid parent-child relationship

        // Share the timer instance with MainWindow
        if (m_mainWindow->m_timer)
        {
            delete m_mainWindow->m_timer;
            m_mainWindow->m_timer = m_timer;

            // Reconnect signals in MainWindow
            m_mainWindow->setupConnections();
        }
    }

    m_mainWindow->show();
    m_mainWindow->activateWindow();
}

void TimerWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // // Draw rounded rectangle background with semi-transparency
    // QColor backgroundColor(30, 30, 30, 180); // Semi-transparent dark background
    // painter.setBrush(backgroundColor);
    // painter.setPen(Qt::NoPen);
    //
    // QRect roundedRect = rect().adjusted(0, 0, 0, 0);
    // painter.drawRoundedRect(roundedRect, 15, 15);
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
