/**
 * @file basemonitor.h
 * @brief Abstract base class for system monitors
 * @author TungNHS
 * @version 1.0.0
 */

#ifndef BASEMONITOR_H
#define BASEMONITOR_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QDateTime>
#include "../../core/constants.h"
#include "../../core/types.h"

/**
 * @brief Abstract base monitor using Template method pattern
 * Thread-safe monitoring lifecylce management
 */

class BaseMonitor : public QObject
{
    Q_OBJECT
public:
    explicit BaseMonitor(QObject *parent = nullptr);
    virtual ~BaseMonitor() = default;

    // Control interface
    virtual void startMonitoring();
    virtual void stopMonitoring();
    virtual void pauseMonitoring();
    virtual void resumeMonitoring();

    // Configuration
    void setUpdateInterval(int intervalMs);
    int getUpdateInterval() const { return m_updateInterval; }

    // Staus queries
    bool isMonitoring() const { return m_isMonitoring; }
    bool isPaused() const { return m_isPaused; }
    QDateTime getLastUpdateTime() const { return m_lastUpdateTime; }

protected:
    // Template Method - implement in concrete classes
    virtual void collectData() = 0;     // Collect raw data
    virtual void processData() = 0;     // Calculate metrics
    virtual void validateData() = 0;    // Validate results
    virtual void emitSignal() = 0;      // Send Qt signals

    // Utility methods
    void updateTimestamp();
    bool isDataState(int maxAgeMs = 5000) const;

    // Thread safety
    mutable QMutex m_dataMutex;

protected slots:
    void onTimerTick();

signals:
    void monitoringStarted();
    void monitoringStopped();
    void dataUpdated();
    void errorOccurred(const QString& error);

private:
    QTimer* m_updateTimer;
    bool m_isMonitoring;
    bool m_isPaused;
    int m_updateInterval;
    QDateTime m_lastUpdateTime;
};

#endif // BASEMONITOR_H
