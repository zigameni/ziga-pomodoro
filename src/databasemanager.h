//
// Created by zigameni on 3/9/25.
//

#ifndef ZIGA_POMODORO_DATABASEMANAGER_H
#define ZIGA_POMODORO_DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager() override;

    // Database initialization
    bool initialize();
    bool isInitialized() const;

    // Pomodoro session tracking
    bool recordPomodoroSession(const QDateTime& startTime, int durationSeconds, bool completed);
    bool recordBreakSession(const QDateTime& startTime, int durationSeconds, bool isLongBreak);

    // Statistics retrieval
    int getTotalCompletedPomodoros(const QDate& date = QDate::currentDate());
    int getTotalWorkMinutes(const QDate& date = QDate::currentDate());
    double getAverageSessionLength(const QDate& from = QDate::currentDate().addDays(-30),
                                 const QDate& to = QDate::currentDate());

    // Time tracking
    QList<QPair<QDate, int>> getDailyPomodoroStats(const QDate& from = QDate::currentDate().addDays(-7),
                                                 const QDate& to = QDate::currentDate());

    // Database maintenance
    bool clearOldData(const QDate& olderThan = QDate::currentDate().addMonths(-3));
    bool exportData(const QString& filePath);
    bool importData(const QString& filePath);

signals:
    void databaseError(const QString& errorMessage);

private:
    QSqlDatabase m_db;
    bool m_initialized;

    // Database setup methods
    bool createTables();
    bool upgradeSchema(int fromVersion, int toVersion);
    int getCurrentSchemaVersion();
    bool setSchemaVersion(int version);

    // Helper methods
    QString getDatabasePath() const;
    bool executeSqlQuery(const QString& queryStr, const QMap<QString, QVariant>& bindValues = QMap<QString, QVariant>());
};

#endif // ZIGA_POMODORO_DATABASEMANAGER_H