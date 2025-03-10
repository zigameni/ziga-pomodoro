//
// Created by zigameni on 3/9/25.
//

#include "databasemanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject* parent)
    : QObject(parent)
      , m_initialized(false)
{
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen())
    {
        m_db.close();
    }
}

bool DatabaseManager::initialize()
{
    if (m_initialized)
    {
        return true;
    }

    // Set up database connection
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(getDatabasePath());

    if (!m_db.open())
    {
        emit databaseError("Failed to open database: " + m_db.lastError().text());
        return false;
    }

    // Check if we need to create tables or upgrade schema
    int currentVersion = getCurrentSchemaVersion();
    if (currentVersion == 0)
    {
        if (!createTables())
        {
            m_db.close();
            return false;
        }
    }
    else if (currentVersion < 1)
    {
        // For future upgrades
        if (!upgradeSchema(currentVersion, 1))
        {
            m_db.close();
            return false;
        }
    }

    m_initialized = true;
    return true;
}

bool DatabaseManager::isInitialized() const
{
    return m_initialized;
}

bool DatabaseManager::recordPomodoroSession(const QDateTime& startTime, int durationSeconds, bool completed)
{
    if (!m_initialized)
    {
        emit databaseError("Database not initialized");
        return false;
    }

    QString queryStr = "INSERT INTO pomodoro_sessions (start_time, duration_seconds, completed) "
        "VALUES (:start_time, :duration, :completed)";

    QMap<QString, QVariant> bindValues;
    bindValues[":start_time"] = startTime;
    bindValues[":duration"] = durationSeconds;
    bindValues[":completed"] = completed ? 1 : 0;

    return executeSqlQuery(queryStr, bindValues);
}

bool DatabaseManager::recordBreakSession(const QDateTime& startTime, int durationSeconds, bool isLongBreak)
{
    if (!m_initialized)
    {
        emit databaseError("Database not initialized");
        return false;
    }

    QString queryStr = "INSERT INTO break_sessions (start_time, duration_seconds, is_long_break) "
        "VALUES (:start_time, :duration, :is_long_break)";

    QMap<QString, QVariant> bindValues;
    bindValues[":start_time"] = startTime;
    bindValues[":duration"] = durationSeconds;
    bindValues[":is_long_break"] = isLongBreak ? 1 : 0;

    return executeSqlQuery(queryStr, bindValues);
}

int DatabaseManager::getTotalCompletedPomodoros(const QDate& date)
{
    if (!m_initialized)
    {
        emit databaseError("Database not initialized");
        return 0;
    }

    QString queryStr = "SELECT COUNT(*) FROM pomodoro_sessions "
        "WHERE completed = 1 AND date(start_time) = date(:date)";

    QSqlQuery query(m_db);
    query.prepare(queryStr);
    query.bindValue(":date", date.toString(Qt::ISODate));

    if (!query.exec() || !query.next())
    {
        emit databaseError("Failed to get total completed pomodoros: " + query.lastError().text());
        return 0;
    }

    return query.value(0).toInt();
}

int DatabaseManager::getTotalWorkMinutes(const QDate& date)
{
    if (!m_initialized)
    {
        emit databaseError("Database not initialized");
        return 0;
    }

    QString queryStr = "SELECT SUM(duration_seconds) / 60 FROM pomodoro_sessions "
        "WHERE date(start_time) = date(:date)";

    QSqlQuery query(m_db);
    query.prepare(queryStr);
    query.bindValue(":date", date.toString(Qt::ISODate));

    if (!query.exec() || !query.next())
    {
        emit databaseError("Failed to get total work minutes: " + query.lastError().text());
        return 0;
    }

    return query.value(0).toInt();
}

double DatabaseManager::getAverageSessionLength(const QDate& from, const QDate& to)
{
    if (!m_initialized)
    {
        emit databaseError("Database not initialized");
        return 0.0;
    }

    QString queryStr = "SELECT AVG(duration_seconds) / 60.0 FROM pomodoro_sessions "
        "WHERE date(start_time) BETWEEN date(:from) AND date(:to) AND completed = 1";

    QSqlQuery query(m_db);
    query.prepare(queryStr);
    query.bindValue(":from", from.toString(Qt::ISODate));
    query.bindValue(":to", to.toString(Qt::ISODate));

    if (!query.exec() || !query.next())
    {
        emit databaseError("Failed to get average session length: " + query.lastError().text());
        return 0.0;
    }

    return query.value(0).toDouble();
}

QList<QPair<QDate, int>> DatabaseManager::getDailyPomodoroStats(const QDate& from, const QDate& to)
{
    QList<QPair<QDate, int>> results;

    if (!m_initialized)
    {
        emit databaseError("Database not initialized");
        return results;
    }

    QString queryStr = "SELECT date(start_time) as day, COUNT(*) as count "
        "FROM pomodoro_sessions "
        "WHERE date(start_time) BETWEEN date(:from) AND date(:to) AND completed = 1 "
        "GROUP BY day ORDER BY day";

    QSqlQuery query(m_db);
    query.prepare(queryStr);
    query.bindValue(":from", from.toString(Qt::ISODate));
    query.bindValue(":to", to.toString(Qt::ISODate));

    if (!query.exec())
    {
        emit databaseError("Failed to get daily pomodoro stats: " + query.lastError().text());
        return results;
    }

    while (query.next())
    {
        QDate date = QDate::fromString(query.value(0).toString(), Qt::ISODate);
        int count = query.value(1).toInt();
        results.append(qMakePair(date, count));
    }

    return results;
}

bool DatabaseManager::clearOldData(const QDate& olderThan)
{
    if (!m_initialized)
    {
        emit databaseError("Database not initialized");
        return false;
    }

    QSqlQuery query(m_db);
    m_db.transaction();

    // Delete old pomodoro sessions
    query.prepare("DELETE FROM pomodoro_sessions WHERE date(start_time) < date(:cutoff_date)");
    query.bindValue(":cutoff_date", olderThan.toString(Qt::ISODate));
    if (!query.exec())
    {
        m_db.rollback();
        emit databaseError("Failed to clear old pomodoro data: " + query.lastError().text());
        return false;
    }

    // Delete old break sessions
    query.prepare("DELETE FROM break_sessions WHERE date(start_time) < date(:cutoff_date)");
    query.bindValue(":cutoff_date", olderThan.toString(Qt::ISODate));
    if (!query.exec())
    {
        m_db.rollback();
        emit databaseError("Failed to clear old break data: " + query.lastError().text());
        return false;
    }

    m_db.commit();
    return true;
}

bool DatabaseManager::exportData(const QString& filePath)
{
    if (!m_initialized)
    {
        emit databaseError("Database not initialized");
        return false;
    }

    // This is a simplified export. For a real implementation, you might want to
    // use SQLite's backup API or generate JSON/CSV data
    if (QFile::exists(filePath))
    {
        QFile::remove(filePath);
    }

    return QFile::copy(getDatabasePath(), filePath);
}

bool DatabaseManager::importData(const QString& filePath)
{
    if (!QFile::exists(filePath))
    {
        emit databaseError("Import file does not exist");
        return false;
    }

    // Close the current database connection
    if (m_db.isOpen())
    {
        m_db.close();
    }

    // Backup current database
    QString currentDbPath = getDatabasePath();
    QString backupPath = currentDbPath + ".backup";
    if (QFile::exists(backupPath))
    {
        QFile::remove(backupPath);
    }
    QFile::copy(currentDbPath, backupPath);

    // Replace with imported file
    if (QFile::exists(currentDbPath))
    {
        QFile::remove(currentDbPath);
    }

    if (!QFile::copy(filePath, currentDbPath))
    {
        // Restore from backup
        QFile::copy(backupPath, currentDbPath);
        emit databaseError("Failed to copy import file");
        return false;
    }

    // Re-initialize the database connection
    return initialize();
}

bool DatabaseManager::createTables()
{
    m_db.transaction();

    // Create schema_version table
    if (!executeSqlQuery("CREATE TABLE IF NOT EXISTS schema_version ("
        "version INTEGER NOT NULL)"))
    {
        m_db.rollback();
        return false;
    }

    // Create pomodoro_sessions table
    if (!executeSqlQuery("CREATE TABLE IF NOT EXISTS pomodoro_sessions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "start_time DATETIME NOT NULL, "
        "duration_seconds INTEGER NOT NULL, "
        "completed BOOLEAN NOT NULL DEFAULT 0)"))
    {
        m_db.rollback();
        return false;
    }

    // Create break_sessions table
    if (!executeSqlQuery("CREATE TABLE IF NOT EXISTS break_sessions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "start_time DATETIME NOT NULL, "
        "duration_seconds INTEGER NOT NULL, "
        "is_long_break BOOLEAN NOT NULL DEFAULT 0)"))
    {
        m_db.rollback();
        return false;
    }

    // Add indexes for faster querying
    if (!executeSqlQuery("CREATE INDEX IF NOT EXISTS idx_pomodoro_start_time ON pomodoro_sessions(start_time)"))
    {
        m_db.rollback();
        return false;
    }

    if (!executeSqlQuery("CREATE INDEX IF NOT EXISTS idx_break_start_time ON break_sessions(start_time)"))
    {
        m_db.rollback();
        return false;
    }

    // Set initial schema version
    if (!setSchemaVersion(1))
    {
        m_db.rollback();
        return false;
    }

    m_db.commit();
    return true;
}

bool DatabaseManager::upgradeSchema(int fromVersion, int toVersion)
{
    // This method will be used for future schema upgrades
    // For now, we just return true since we don't need to upgrade anything yet
    Q_UNUSED(fromVersion);

    setSchemaVersion(toVersion);
    return true;
}

int DatabaseManager::getCurrentSchemaVersion()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT version FROM schema_version ORDER BY version DESC LIMIT 1");

    if (!query.exec() || !query.next())
    {
        // If the query fails or returns no results, we assume version 0 (no schema yet)
        return 0;
    }

    return query.value(0).toInt();
}

bool DatabaseManager::setSchemaVersion(int version)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO schema_version (version) VALUES (:version)");
    query.bindValue(":version", version);

    if (!query.exec())
    {
        emit databaseError("Failed to set schema version: " + query.lastError().text());
        return false;
    }

    return true;
}

QString DatabaseManager::getDatabasePath() const
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);

    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    return dir.filePath("ziga_pomodoro.db");
}

bool DatabaseManager::executeSqlQuery(const QString& queryStr, const QMap<QString, QVariant>& bindValues)
{
    QSqlQuery query(m_db);
    query.prepare(queryStr);

    // Bind values if any
    QMapIterator<QString, QVariant> i(bindValues);
    while (i.hasNext())
    {
        i.next();
        query.bindValue(i.key(), i.value());
    }

    if (!query.exec())
    {
        emit databaseError("SQL error: " + query.lastError().text() + " for query: " + queryStr);
        return false;
    }

    return true;
}
