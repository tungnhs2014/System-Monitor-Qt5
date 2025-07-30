/**
 * @file memorymonitor.h
 * @brief Memory monitoring for Pi 3B+ (1GB RAM)
 * @author TungNHS
 * @version 1.0.0
 */

#ifndef MEMORYMONITOR_H
#define MEMORYMONITOR_H

#include "model/base/basemonitor.h"
#include "core/types.h"
#include <QVector>

class MemoryMonitor : public BaseMonitor
{
    Q_OBJECT
public:
    explicit MemoryMonitor(QObject *parent = nullptr);

    // Data access
    MemoryData getCurrentData() const;
    QVector<MemoryData> getHistory() const;
    void setHistorySize(int size);

    // Memory analysis
    double getMemoryEfficiency() const;    // Available/Total * 100
    qint64 getMemoryPressure() const;      // Used - Buffers - Cache
    bool isSwapping() const;               // Swap usage > 0

signals:
    void memoryDataUpdated(const MemoryData& data);
    void memoryWarning(double usagePercent);
    void memoryCritical(double usagePercent);
    void swapWarning(double swapPercent);
    void lowMemoryAlert(qint64 availableBytes);

protected:
    // Template Method implementation
    void collectData() override;
    void processData() override;
    void validateData() override;
    void emitSignal() override;

private:
    // Data collection
    void collectMemoryInfo();
    void collectSwapInfo();

    // Calculations
    double calculateUsagePercent() const;
    double calculateSwapPercent() const;
    MetricStatus determineStatus() const;

    // Data members
    MemoryData m_currentData;
    QVector<MemoryData> m_history;
    int m_maxHistorySize;

    // Low memory threshold for Pi 3B+ (50MB)
    static const qint64 LOW_MEMORY_THRESHOLD = 50 * 1024 * 1024;
};

#endif // MEMORYMONITOR_H
