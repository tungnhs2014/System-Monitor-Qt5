/**
 * @file datamanager.h
 * @brief Central data coordination and management system
 * @author TungNHS
 * @version 1.0.0
 */

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include <memory>
#include "core/types.h"

// Forward declarations
class CPUMonitor;
class MemoryMonitor;
class AlertManager;

/**
 * @brief System overview data structure
 * Aggregates all monitoring data into single snapshot
 */
struct SystemOverview {
    CPUData cpu;
    MemoryData memory;
    QDateTime timestamp;

    bool isValid() const {
        return cpu.isValid() && memory.isValid();
    }

    SystemOverview() {
        timestamp = QDateTime::currentDateTime();
    }
};

Q_DECLARE_METATYPE(SystemOverview)

/**
 * @brief Central data coordination and management
 * Manages all monitors and provides unified data access
 */

class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = nullptr);
    ~DataManager();

    // Lifecycle management
    void initialize();
    void start();
    void stop();
    void pause();
    void resume();

    // Data access
    SystemOverview getCurrentSystemData() const;
    CPUData getCurrentCPUData() const;
    MemoryData getCurrentMemoryData() const;

    // Status
    bool isRunning() const { return m_isRunning; }
    bool isPaused() const { return m_isPaused; }
    bool isInitialized() const { return m_isInitialized; }

    // Configuration
    void setUpdateInterval(int intervalMs);
    void setGlobalPaused(bool paused);

    // Monitor access
    CPUMonitor* getCPUMonitor() const { return m_cpuMonitor.get(); }
    MemoryMonitor* getMemoryMonitor() const { return m_memoryMonitor.get(); }
    AlertManager* getAlertManager() const { return m_alertManager.get(); }

signals:
    void systemDataUpdated(const SystemOverview& overview);
    void monitoringStateChanged(bool isActive);
    void initializationComplete();
    void errorOccurred(const QString& error);

private slots:
    void onCPUDataUpdated(const CPUData& data);
    void onMemoryDataUpdated(const MemoryData& data);
    void aggregateSystemData();

private:
    void connectMonitorSignals();
    void updateSystemOverview();

    // Monitor instances
    std::unique_ptr<CPUMonitor> m_cpuMonitor;
    std::unique_ptr<MemoryMonitor> m_memoryMonitor;
    std::unique_ptr<AlertManager> m_alertManager;

    // Data synchronization
    SystemOverview m_currentOverview;
    mutable QMutex m_dataMutex;
    QTimer* m_aggregationTimer;

    // State
    bool m_isInitialized;
    bool m_isRunning;
    bool m_isPaused;
    int m_updateInterval;
};

#endif // DATAMANAGER_H
