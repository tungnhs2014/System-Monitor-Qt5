#ifndef CPUMONITOR_H
#define CPUMONITOR_H

#include "model/base/basemonitor.h"
#include "core/types.h"

class CPUMonitor : public BaseMonitor
{
    Q_OBJECT
public:
    explicit CPUMonitor(QObject *parent = nullptr);

    // Data access
    CPUData getCurrentData() const;
    QVector<CPUData> getHistory() const;
    void setHistorySize(int size);

signals:
    void cpuDataUpdated(const CPUData& data);
    void temperatureWarning(double temp);
    void temperatureCritical(double temp);
    void usageWarning(double usage);
    void usageCritical(double usage);

protected:
    // Temperature Method implementation
    void collectData() override;
    void processData() override;
    void validateData() override;
    void emitSignal() override;

private:
    // Data collection
    void collectCPUStats();
    void collectTemperature();
    void collectFrequency();
    void collectCoreData();

    // Calculations
    double calculateUsagePercent();
    MetricStatus determineStatus() const;

    // CPU statistics structure
    struct CPUStat {
        qint64 user, nice, system, idle, iowait, irq, softirq, steal;

        qint64 total() const {
            return user + nice + system + idle + iowait + irq + softirq + steal;
        }

        qint64 active() const {
            return total() - idle- iowait;
        }

        CPUStat() : user(0), nice(0), system(0), idle(0), iowait(0), irq(0), softirq(0), steal(0) {}
    };

    // Data members
    CPUData m_currentData;
    CPUData m_previousData;
    QVector<CPUData> m_history;
    int m_maxHistorySize;

    // Raw CPU stats for delta calculation
    CPUStat m_currentStats;
    CPUStat m_previousStats;
    QVector<CPUStat> m_coreStats;
    QVector<CPUStat> m_previousCoreStats;
};

#endif // CPUMONITOR_H
