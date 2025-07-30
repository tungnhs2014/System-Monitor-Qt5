/**
 * @file alertmanager.cpp
 * @brief Alert management implementation
 */

#include "alertmanager.h"
#include "core/constants.h"
#include <QDebug>

AlertManager::AlertManager(QObject *parent)
    : QObject(parent)
    , m_cleanupTimer(new QTimer(this))
    , m_maxAlertsHistory(MAX_ALERTS_HISTORY)
    , m_nextAlertId(1)
    , m_cpuWarningActive(false)
    , m_cpuCriticalActive(false)
    , m_memoryWarningActive(false)
    , m_memoryCriticalActive(false)
    , m_tempWarningActive(false)
    , m_tempCriticalActive(false)
{
    connect(m_cleanupTimer, &QTimer::timeout, this, &AlertManager::cleanupOldAlerts);
    m_cleanupTimer->start(ALERT_CLEANUP_INTERVAL);
}

void AlertManager::addAlert(const AlertData &alert)
{
    QMutexLocker locker(&m_alertsMutex);

    AlertData newAlert = alert;
    newAlert.timestamp = QDateTime::currentDateTime();

    m_alerts.append(newAlert);

    // Emit signals
    emit alertAdded(newAlert);
    if (newAlert.severity == AlertSeverity::Critical || newAlert.severity == AlertSeverity::Emergency) {
        emit criticalAlert(newAlert);
    }

    emit alertCountChanged(m_alerts.size(), getUnacknowledgedCount());

    // Cleanup if too many alerts
    if (m_alerts.size() > m_maxAlertsHistory) {
        m_alerts.removeFirst();
    }
}

void AlertManager::acknowledgeAlert(int alertId)
{
    QMutexLocker locker(&m_alertsMutex);

    for (auto& alert : m_alerts) {
        if (alert.source.contains(QString::number(alertId))) {
            alert.acknowledged = true;
            emit alertAcknowledged(alertId);
            emit alertCountChanged(m_alerts.size(), getUnacknowledgedCount());
            break;
        }
    }
}

void AlertManager::clearAllAlerts()
{
    QMutexLocker locker(&m_alertsMutex);
    m_alerts.clear();
    emit alertCountChanged(0, 0);
}

void AlertManager::clearAcknowledgedAlerts()
{
    QMutexLocker locker(&m_alertsMutex);

    m_alerts.erase(std::remove_if(m_alerts.begin(), m_alerts.end(),
                                  [](const AlertData& alert) {
                                      return alert.acknowledged;
                                  }), m_alerts.end());

    emit alertCountChanged(m_alerts.size(), getUnacknowledgedCount());
}

QVector<AlertData> AlertManager::getActiveAlerts() const
{
    QMutexLocker locker(&m_alertsMutex);

    QVector<AlertData> activeAlerts;
    for (const auto& alert: m_alerts) {
        if (!alert.acknowledged) {
            activeAlerts.append(alert);
        }
    }

    return activeAlerts;
}

QVector<AlertData> AlertManager::getAllAlerts() const
{
    QMutexLocker locker(&m_alertsMutex);
    return m_alerts;
}

int AlertManager::getUnacknowledgedCount() const
{
    int count = 0;
    for (const auto& alert : m_alerts) {
        if (!alert.acknowledged) {
            count++;
        }
    }

    return count;
}

void AlertManager::setMaxAlertsHistory(int maxCount)
{
    m_maxAlertsHistory = qBound(50, maxCount, 1000);
}

void AlertManager::setAlertCleanupInterval(int intervalMs)
{
    m_cleanupTimer->setInterval(qMax(60000, intervalMs));     // Min 1 minute
}

void AlertManager::checkCPUThresholds(const CPUData &data)
{
    // Check CPU usage
    if (data.totalUsage >= CPU_CRITICAL_THRESHOLD && shouldCreateCPUAlert(AlertSeverity::Critical)) {
        AlertData alert = createCPUAlert(AlertSeverity::Critical,
                                         QString("CPU usage exceed critical threshold"),
                                         data.totalUsage);
        addAlert(alert);
        m_cpuCriticalActive = true;
        m_lastCPUAlert = QDateTime::currentDateTime();
    }
    else if (data.totalUsage >= CPU_WARNING_THRESHOLD && shouldCreateCPUAlert(AlertSeverity::Warning)) {
        AlertData alert = createCPUAlert(AlertSeverity::Warning,
                                         QString("CPU usage high"),
                                         data.totalUsage);
        addAlert(alert);
        m_cpuWarningActive = true;
        m_lastCPUAlert = QDateTime::currentDateTime();
    }
    else {
        m_cpuCriticalActive = false;
        m_cpuWarningActive = false;
    }

    // Check temperature
    if (data.temperature >= TEMP_CRITICAL_THRESHOLD && shouldCreateTempAlert(AlertSeverity::Critical)) {
        AlertData alert = createTemperatureAlert(AlertSeverity::Critical, data.temperature);
        addAlert(alert);
        m_tempCriticalActive = true;
        m_lastTempAlert = QDateTime::currentDateTime();
    }
    else if (data.temperature >= TEMP_WARNING_THRESHOLD && shouldCreateTempAlert(AlertSeverity::Warning)) {
        AlertData alert = createTemperatureAlert(AlertSeverity::Warning, data.temperature);
        addAlert(alert);
        m_tempWarningActive = true;
        m_lastTempAlert = QDateTime::currentDateTime();
    }
    else {
        m_tempCriticalActive = false;
        m_tempWarningActive = false;
    }
}

void AlertManager::checkMemoryThresholds(const MemoryData &data)
{
    if (data.usagePercentage >= RAM_CRITICAL_THRESHOLD && shouldCreateMemoryAlert(AlertSeverity::Critical)) {
        AlertData alert = createMemoryAlert(AlertSeverity::Critical,
                                            QString("Memory usage critical"),
                                            data.usagePercentage);
        addAlert(alert);
        m_memoryCriticalActive = true;
        m_lastMemoryAlert = QDateTime::currentDateTime();
    }
    else if (data.usagePercentage >= RAM_WARNING_THRESHOLD && shouldCreateMemoryAlert(AlertSeverity::Warning)) {
        AlertData alert = createMemoryAlert(AlertSeverity::Warning,
                                            QString("Memory usage high"),
                                            data.usagePercentage);
        addAlert(alert);
        m_memoryWarningActive = true;
        m_lastMemoryAlert = QDateTime::currentDateTime();
    }
    else {
        m_memoryWarningActive = false;
        m_memoryCriticalActive = false;
    }
}

void AlertManager::cleanupOldAlerts()
{
    QMutexLocker locker(&m_alertsMutex);

    QDateTime cutoffTime = QDateTime::currentDateTime().addDays(-1); // Kepp 1 day

    int oldSize = m_alerts.size();
    m_alerts.erase(std::remove_if(m_alerts.begin(), m_alerts.end(),
                                  [cutoffTime](const AlertData& alert) {
                                      return alert.acknowledged && alert.timestamp < cutoffTime;
                                }), m_alerts.end());
    if (m_alerts.size() != oldSize) {
        emit alertCountChanged(m_alerts.size(), getUnacknowledgedCount());
    }
}

AlertData AlertManager::createCPUAlert(AlertSeverity severity, const QString &message, double value, const QString &unit)
{
    AlertData alert;
    alert.severity = severity;
    alert.title = (severity == AlertSeverity::Critical) ? "CPU Critical" : "CPU Warning";
    alert.message = QString("%1: %2%3").arg(message).arg(value, 0, 'f', 1).arg(unit);
    alert.source = "CPU";
    alert.acknowledged = false;

    return alert;

}

AlertData AlertManager::createMemoryAlert(AlertSeverity severity, const QString &message, double value, const QString &unit)
{
    AlertData alert;
    alert.severity = severity;
    alert.title = (severity == AlertSeverity::Critical) ? "Memory Critical" : "Memory Warning";
    alert.message = QString("%1: %2%3").arg(message).arg(value, 0, 'f', 1).arg(unit);
    alert.source = "Memory";
    alert.acknowledged = false;

    return alert;
}

AlertData AlertManager::createTemperatureAlert(AlertSeverity severity, double temperature)
{
    AlertData alert;
    alert.severity = severity;
    alert.title = (severity == AlertSeverity::Critical) ? "Temperature Critical" : "Temperature Warning";
    alert.message = QString("CPU temperature: %1°C").arg(temperature, 0, 'f', 1);
    alert.source = "Temperature";
    alert.acknowledged = false;

    return alert;
}

bool AlertManager::shouldCreateCPUAlert(AlertSeverity severity) const
{
    bool isActive = (severity == AlertSeverity::Critical) ? m_cpuCriticalActive : m_cpuWarningActive;
    bool cooldownExpired = m_lastCPUAlert.msecsTo(QDateTime::currentDateTime()) > ALERT_COOLDOWN_MS;

    return !isActive || cooldownExpired;
}

bool AlertManager::shouldCreateMemoryAlert(AlertSeverity severity) const
{
    bool isActive = (severity == AlertSeverity::Critical) ? m_memoryCriticalActive : m_memoryWarningActive;
    bool cooldownExpired = m_lastMemoryAlert.msecsTo(QDateTime::currentDateTime()) > ALERT_COOLDOWN_MS;

    return !isActive || cooldownExpired;
}

bool AlertManager::shouldCreateTempAlert(AlertSeverity severity) const
{
    bool isActive = (severity == AlertSeverity::Critical) ? m_tempCriticalActive : m_tempWarningActive;
    bool cooldownExpired = m_lastTempAlert.msecsTo(QDateTime::currentDateTime()) > ALERT_COOLDOWN_MS;

    return !isActive || cooldownExpired;
}
