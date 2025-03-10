//
// Created by zigameni on 3/9/25.
//

#ifndef ZIGA_POMODORO_POMODOROACTIVITYMAP_H
#define ZIGA_POMODORO_POMODOROACTIVITYMAP_H

#include <QWidget>
#include <QMap>
#include <QDate>
#include <QPair>
#include <QToolTip>
#include <QPainter>
#include <QMouseEvent>
#include <QLabel>

// Forward declaration
class DatabaseManager;

class PomodoroActivityMap : public QWidget
{
    Q_OBJECT

public:
    explicit PomodoroActivityMap(QWidget* parent = nullptr);
    ~PomodoroActivityMap() override;

    void setDatabaseManager(DatabaseManager* dbManager);
    void setDateRange(const QDate& startDate, const QDate& endDate);
    void refreshData();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    DatabaseManager* m_dbManager;
    QDate m_startDate;
    QDate m_endDate;
    QMap<QDate, int> m_pomodorosByDate;
    QMap<QDate, int> m_minutesByDate;

    QDate m_currentHoverDate;
    QRect m_legendRect;

    int m_cellSize;
    int m_cellSpacing;
    int m_maxPomodoros;

    struct CellInfo {
        QRect rect;
        QDate date;
        int count;
        int minutes;
    };
    QList<CellInfo> m_cells;

    void calculateCellPositions();
    QColor getColorForCount(int count) const;
    void drawLegend(QPainter& painter);
    CellInfo* getCellAt(const QPoint& pos);
};

#endif // ZIGA_POMODORO_POMODOROACTIVITYMAP_H