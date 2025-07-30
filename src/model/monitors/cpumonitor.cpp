/**
 * @file cpumonitor.cpp
 * @brief CPU monitoring implementation for Pi 3B+
 * @author TungNHS
 * version 1.0.0
 */

#include "cpumonitor.h"
#include "core/systemutils.h"
#include "core/constants.h"
#include <QStringList>
#include <QDebug>

CPUMonitor::CPUMonitor(QObject *parent) : BaseMonitor(parent), m_maxHistorySize(MAX_HISTORY_SIZE)
{
    int coreCount = SystemUtils::getCPUCoreCount();
    m_currentData.coreCount = coreCount;
    m_currentData.cores.resize(coreCount);
    m_coreStats.resize(coreCount);
    m_previousCoreStats.resize(coreCount);

    m_currentData.model = SystemUtils::getCPUModel();
}

CPUData CPUMonitor::getCurrentData() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_currentData;
}

QVector<CPUData> CPUMonitor::getHistory() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_history;
}

void CPUMonitor::setHistorySize(int size)
{
    m_maxHistorySize = qBound(10, size, 1000);
}

void CPUMonitor::collectData()
{
    // Save previous state for delta calculation
    m_previousStats = m_currentStats;
    m_previousCoreStats = m_coreStats;
    m_previousData = m_currentData;

    collectCPUStats();
    collectTemperature();
    collectFrequency();
    collectCoreData();
}

void CPUMonitor::processData()
{
    m_currentData.totalUsage = calculateUsagePercent();
    m_currentData.status = determineStatus();
    m_currentData.timestamp = QDateTime::currentDateTime();
}

void CPUMonitor::validateData()
{
    if (!SystemUtils::isValidPercentage(m_currentData.totalUsage)) {
        m_currentData.totalUsage = 0.0;
    }

    if (!SystemUtils::isValidTemperature(m_currentData.temperature)) {
        m_currentData.temperature = 0.0;
    }

    for (auto& core : m_currentData.cores) {
        if (!SystemUtils::isValidPercentage(core.usage)) {
            core.usage = 0.0;
        }
    }
}

void CPUMonitor::emitSignal()
{
    emit cpuDataUpdated(m_currentData);

    // Emit threshold warnings
    if (m_currentData.temperature >= TEMP_CRITICAL_THRESHOLD) {
        emit temperatureCritical(m_currentData.temperature);
    }
    else if (m_currentData.temperature >= TEMP_WARNING_THRESHOLD) {
        emit temperatureWarning(m_currentData.temperature);
    }

    if (m_currentData.totalUsage >= CPU_CRITICAL_THRESHOLD) {
        emit usageCritical(m_currentData.totalUsage);
    }
    else if (m_currentData.totalUsage >= CPU_WARNING_THRESHOLD) {
        emit usageWarning(m_currentData.totalUsage);
    }

    // Update history
    m_history.append(m_currentData);
    if (m_history.size() > m_maxHistorySize) {
        m_history.removeFirst();
    }
}

void CPUMonitor::collectCPUStats()
{
    QString statContent = SystemUtils::readFile(PROC_STAT);
    if (statContent.isEmpty()) return;

    QStringList lines = statContent.split('\n', Qt::SkipEmptyParts);
    if (lines.isEmpty()) return;

    // Parse overall CPU line: "cpu, user, nice, system, idle..."
    QString cpuLine = lines[0];
    QStringList parts = cpuLine.split(' ', Qt::SkipEmptyParts);

    if (parts.size() >= 8 && parts[0] == "cpu") {
        m_currentStats.user = parts[1].toLongLong();
        m_currentStats.nice = parts[2].toLongLong();
        m_currentStats.system = parts[3].toLongLong();
        m_currentStats.idle = parts[4].toLongLong();
        m_currentStats.iowait = parts[5].toLongLong();
        m_currentStats.irq = parts[6].toLongLong();
        m_currentStats.softirq = parts[7].toLongLong();
        m_currentStats.steal = (parts.size() > 8) ? parts[8].toLongLong() : 0;
    }

    // Parse per-core stats: "cpu0", "cpu1", etc
    for (int i = 0; i < m_currentData.coreCount && i + 1 < lines.size(); ++i) {
        QString coreLine = lines[i + 1];
        QStringList coreParts = coreLine.split(' ', Qt::SkipEmptyParts);

        if (coreParts.size() >= 8 && coreParts[0] == QString("cpu%1").arg(i)) {
            CPUStat& coreStat = m_coreStats[i];
            coreStat.user = coreParts[1].toLongLong();
            coreStat.nice = coreParts[2].toLongLong();
            coreStat.system = coreParts[3].toLongLong();
            coreStat.idle = coreParts[4].toLongLong();
            coreStat.iowait = coreParts[5].toLongLong();
            coreStat.irq = coreParts[6].toLongLong();
            coreStat.softirq = coreParts[7].toLongLong();
            coreStat.steal = (coreParts.size() > 8) ? coreParts[8].toLongLong() : 0;
        }
    }
}

void CPUMonitor::collectTemperature()
{
    m_currentData.temperature = SystemUtils::getCPUTemperature();
}

void CPUMonitor::collectFrequency()
{
    m_currentData.averageFrequency = SystemUtils::getCPUFrequency();
}

void CPUMonitor::collectCoreData()
{
    // Calculate per-core usage
    for (int i = 0; i < m_currentData.coreCount; ++i) {
        CPUCoreData& coreData = m_currentData.cores[i];
        coreData.coreID = i;
        coreData.frequency = m_currentData.averageFrequency;
        coreData.temperature = m_currentData.temperature;

        if (i < m_coreStats.size() && i < m_previousCoreStats.size()) {
            const CPUStat& current = m_coreStats[i];
            const CPUStat& previous = m_previousCoreStats[i];

            qint64 totalDiff = current.total() - previous.total();
            qint64 idleDiff = current.idle - previous.idle;

            if (totalDiff > 0) {
                double usage = (1.0 - static_cast<double>(idleDiff) / totalDiff) * 100.0;
                coreData.usage = qBound(0.0, usage, 100.0);
            }
        }
    }
}

double CPUMonitor::calculateUsagePercent()
{
    qint64 totalDiff = m_currentStats.total() - m_previousStats.total();
    qint64 idleDiff = m_currentStats.idle - m_previousStats.idle;

    if (totalDiff <= 0) {
        return 0.0;
    }

    double usage = (1.0 - static_cast<double>(idleDiff) / totalDiff) * 100.0;
    return qBound(0.0, usage, 100.0);
}

MetricStatus CPUMonitor::determineStatus() const
{
    // Temperature has priority
    if (m_currentData.temperature >= TEMP_CRITICAL_THRESHOLD) {
        return MetricStatus::Critical;
    }
    if (m_currentData.temperature >= TEMP_WARNING_THRESHOLD) {
        return MetricStatus::Warning;
    }

    // Then check CPU usage
    if (m_currentData.totalUsage >= CPU_CRITICAL_THRESHOLD) {
        return MetricStatus::Critical;
    }
    if (m_currentData.totalUsage >= CPU_WARNING_THRESHOLD) {
        return MetricStatus::Warning;
    }

    return MetricStatus::Normal;
}
