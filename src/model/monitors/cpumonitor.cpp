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
            coreStat.user = coreStat[1].toLongLong();
            coreStat.nice = coreStat[2].toLongLong();
            coreStat.system = coreStat[3].toLongLong();
            coreStat.idle = coreStat[4].toLongLong();
            coreStat.iowait = coreStat[5].toLongLong();
            coreStat.irq = coreStat[6].toLongLong();
            coreStat.softirq = coreStat[7].toLongLong();
            coreStat.steal = (coreStat.size() > 8) ? coreStat[8].toLongLong() : 0;
        }
    }
}
