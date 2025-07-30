/**
 * @file alertmanager.h
 * @brief Alert and threshold management system
 * @author TungNHS
 * @version 1.0.0
 */

#ifndef ALERTMANAGER_H
#define ALERTMANAGER_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include <QMutex>
#include "core/types.h"

/**
 * @brief Central alert managment and threshold monitoring
 * Handles alert creation, acknowledgement and cleanup
 */

class AlertManager : public QObject
{
    Q_OBJECT
public:
    explicit AlertManager(QObject *parent = nullptr);

    // Alert managment
    void addAlert(const AlertData& alert);
    void acknowledgeAlert(int alertId);
    void clearAllAlerts();
    void clearAcknowledgedAlerts();

    // Data access
    QVector<AlertData> getActiveAlerts() const;
    QVector<AlertData> getAllAlerts() const;
    int getUnacknowledgedCount() const;

    // Configuration
    void setMaxAlertsHistory(int maxCount);
    void setAlertCleanupInterval(int intervalMs);

    // Threshold monitoring (connect to monitor signals)
    void checkCPUThresholds(const CPUData& data);
    void checkMemoryThresholds(const MemoryData& data);

signals:
    void alertAdded(const AlertData& alert);
    void alertAcknowledged(int alertId);
    void criticalAlert(const AlertData& alert);
    void alertCountChanged(int totalCount, int unacknowledgedCount);

private slots:
    void cleanupOldAlerts();

private:
    // Alert creation helpers
    AlertData createCPUAlert(AlertSeverity severity, const QString& message, double value, const QString& unit = "%");
    AlertData createMemoryAlert(AlertSeverity severity, const QString& message, double value, const QString& unit = "%");
    AlertData createTemperatureAlert(AlertSeverity severity, double temperature);

    // State tracking to prevent spam
    bool shouldCreateCPUAlert(AlertSeverity severity) const;
    bool shouldCreateMemoryAlert(AlertSeverity severity) const;
    bool shouldCreateTempAlert(AlertSeverity severity) const;

    // Data members
    QVector<AlertData> m_alerts;
    mutable QMutex m_alertsMutex;
    QTimer* m_cleanupTimer;
    int m_maxAlertsHistory;
    int m_nextAlertId;

    // State tracking (prevent duplicate alerts)
    bool m_cpuWarningActive;
    bool m_cpuCriticalActive;
    bool m_memoryWarningActive;
    bool m_memoryCriticalActive;
    bool m_tempWarningActive;
    bool m_tempCriticalActive;

    QDateTime m_lastCPUAlert;
    QDateTime m_lastMemoryAlert;
    QDateTime m_lastTempAlert;

    // Minimum time between similar alerts (ms)
    static const int ALERT_COOLDOWN_MS = 30000; // 30 seconds
};

#endif // ALERTMANAGER_H
