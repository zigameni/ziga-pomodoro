//
// Created by zigameni on 3/9/25.
//

#include "pomodoroactivitymap.h"
#include "databasemanager.h"
#include <QPainter>
#include <QToolTip>
#include <QDateTime>

PomodoroActivityMap::PomodoroActivityMap(QWidget* parent)
    : QWidget(parent)
      , m_dbManager(nullptr)
      , m_cellSize(18)
      , m_cellSpacing(3)
      , m_maxPomodoros(0)
{
    setMouseTracking(true);
    setMinimumHeight(150);

    // Default to the last 3 months
    m_startDate = QDate::currentDate().addMonths(-3);
    m_endDate = QDate::currentDate();
}

PomodoroActivityMap::~PomodoroActivityMap() = default;

void PomodoroActivityMap::setDatabaseManager(DatabaseManager* dbManager)
{
    m_dbManager = dbManager;
    refreshData();
}

void PomodoroActivityMap::setDateRange(const QDate& startDate, const QDate& endDate)
{
    m_startDate = startDate;
    m_endDate = endDate;
    refreshData();
}

void PomodoroActivityMap::refreshData()
{
    if (!m_dbManager)
    {
        return;
    }

    // Clear previous data
    m_pomodorosByDate.clear();
    m_minutesByDate.clear();
    m_maxPomodoros = 0;

    // Fetch data from database
    QList<QPair<QDate, int>> dailyStats = m_dbManager->getDailyPomodoroStats(m_startDate, m_endDate);

    for (const auto& stat : dailyStats)
    {
        m_pomodorosByDate[stat.first] = stat.second;
        m_maxPomodoros = qMax(m_maxPomodoros, stat.second);

        // Get total minutes for this day
        int minutes = m_dbManager->getTotalWorkMinutes(stat.first);
        m_minutesByDate[stat.first] = minutes;
    }

    // Make sure we have a non-zero max for color scaling
    if (m_maxPomodoros == 0)
    {
        m_maxPomodoros = 1;
    }

    calculateCellPositions();
    update();
}

void PomodoroActivityMap::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw month labels
    QFont monthFont = font();
    monthFont.setPointSize(9);
    painter.setFont(monthFont);

    QDate date = m_startDate;
    int currentMonth = -1;
    int monthLabelY = 15;

    while (date <= m_endDate)
    {
        if (date.month() != currentMonth)
        {
            currentMonth = date.month();
            int dayOfWeek = date.dayOfWeek();
            int x = 50 + ((date.daysTo(m_startDate) * -1) / 7) * (m_cellSize + m_cellSpacing);

            QString monthName = QLocale().monthName(date.month(), QLocale::ShortFormat);
            painter.drawText(x, monthLabelY, monthName);
        }
        date = date.addDays(1);
    }

    // Draw weekday labels
    QFont dayFont = font();
    dayFont.setPointSize(8);
    painter.setFont(dayFont);

    QStringList dayNames = {"Mon", "Wed", "Fri"};
    for (int i = 0; i < dayNames.size(); i++)
    {
        int dayIndex = i * 2; // 0, 2, 4 for Mon, Wed, Fri
        int y = 35 + dayIndex * (m_cellSize + m_cellSpacing) + m_cellSize / 2;
        painter.drawText(15, y + 4, dayNames[i]);
    }

    // Draw cells
    for (const auto& cell : m_cells)
    {
        QColor cellColor = getColorForCount(cell.count);

        // Highlight the cell being hovered
        if (cell.date == m_currentHoverDate)
        {
            painter.setPen(QPen(Qt::black, 1));
        }
        else
        {
            painter.setPen(QPen(cellColor.darker(110), 1));
        }

        painter.setBrush(cellColor);
        painter.drawRoundedRect(cell.rect, 2, 2);
    }

    // Draw the legend
    drawLegend(painter);
}

void PomodoroActivityMap::mouseMoveEvent(QMouseEvent* event)
{
    CellInfo* cell = getCellAt(event->pos());

    if (cell)
    {
        m_currentHoverDate = cell->date;
        update();

        QString toolTipText = QString("%1\n%2 pomodoros\n%3 minutes")
                              .arg(cell->date.toString("MMM d, yyyy"))
                              .arg(cell->count)
                              .arg(cell->minutes);
        QToolTip::showText(event->globalPos(), toolTipText, this, cell->rect);
    }
    else if (!m_legendRect.contains(event->pos()))
    {
        m_currentHoverDate = QDate();
        update();
        QToolTip::hideText();
    }

    QWidget::mouseMoveEvent(event);
}

void PomodoroActivityMap::leaveEvent(QEvent* event)
{
    m_currentHoverDate = QDate();
    update();
    QToolTip::hideText();

    QWidget::leaveEvent(event);
}

void PomodoroActivityMap::calculateCellPositions()
{
    m_cells.clear();

    // Determine the number of weeks to display
    int totalDays = m_startDate.daysTo(m_endDate) + 1;
    int totalWeeks = (totalDays + 6) / 7; // Ceiling division

    // Adjust the widget width based on cell size and number of weeks
    int mapWidth = 50 + (totalWeeks * (m_cellSize + m_cellSpacing));
    setMinimumWidth(mapWidth + 100); // Add space for legend

    // Create cells for each date in the range
    QDate date = m_startDate;
    while (date <= m_endDate)
    {
        int dayOfWeek = date.dayOfWeek() % 7; // 0-6 (Sunday = 0, Monday = 1, etc.)
        if (dayOfWeek == 0) dayOfWeek = 7; // Adjust to make Monday = 1, Sunday = 7

        int weekIndex = m_startDate.daysTo(date) / 7;

        int x = 50 + weekIndex * (m_cellSize + m_cellSpacing);
        int y = 35 + (dayOfWeek - 1) * (m_cellSize + m_cellSpacing);

        QRect cellRect(x, y, m_cellSize, m_cellSize);

        CellInfo cell;
        cell.rect = cellRect;
        cell.date = date;
        cell.count = m_pomodorosByDate.value(date, 0);
        cell.minutes = m_minutesByDate.value(date, 0);

        m_cells.append(cell);

        date = date.addDays(1);
    }

    // Position the legend
    m_legendRect = QRect(width() - 240, height() - 40, 200, 30);
}

QColor PomodoroActivityMap::getColorForCount(int count) const
{
    if (count == 0)
    {
        return QColor(238, 238, 238); // Light gray for zero
    }

    // Set a minimum intensity level for non-zero values
    // This ensures even small counts have a visible color
    int minIntensity = 50; // Adjust this value as needed (0-255)

    // Calculate intensity with a minimum threshold
    int intensity = minIntensity;
    if (m_maxPomodoros > 1)
    {
        // Scale the remaining intensity range (minIntensity to 255)
        int scaledIntensity = (count * (255 - minIntensity)) / m_maxPomodoros;
        intensity += scaledIntensity;
    }

    // Ensure intensity is within bounds
    intensity = qMin(255, qMax(minIntensity, intensity));

    // GitHub-like green scale with minimum visibility
    return QColor(
        235 - (intensity * 0.8), // Decrease red as count increases
        235 - (intensity * 0.3) + 20, // Keep green higher
        235 - (intensity * 0.8) // Decrease blue as count increases
    );
}

void PomodoroActivityMap::drawLegend(QPainter& painter)
{
    int legendX = width() - 240;
    int legendY = height() - 40;
    int legendItemWidth = 15;
    int legendSpacing = 5;

    painter.drawText(legendX, legendY + 12, "Less");

    legendX += 40;

    // Draw 5 sample boxes representing activity levels
    for (int i = 0; i < 5; i++)
    {
        int value = i * m_maxPomodoros / 4;
        QColor color = getColorForCount(value);

        QRect rect(legendX + i * (legendItemWidth + legendSpacing),
                   legendY, legendItemWidth, legendItemWidth);

        painter.setBrush(color);
        painter.setPen(QPen(color.darker(110), 1));
        painter.drawRoundedRect(rect, 2, 2);
    }

    legendX += 5 * (legendItemWidth + legendSpacing) + 5;
    painter.drawText(legendX, legendY + 12, "More");
}

PomodoroActivityMap::CellInfo* PomodoroActivityMap::getCellAt(const QPoint& pos)
{
    for (auto& cell : m_cells)
    {
        if (cell.rect.contains(pos))
        {
            return &cell;
        }
    }
    return nullptr;
}
