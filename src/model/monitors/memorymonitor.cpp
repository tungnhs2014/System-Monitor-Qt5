/**
 * @file memorymonitor.cpp
 * @brief Memory monitoring implementation for Pi 3B+
 */

#include "memorymonitor.h"
#include "core/systemutils.h"
#include "core/constants.h"
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

MemoryMonitor::MemoryMonitor(QObject *parent) : BaseMonitor(parent), m_maxHistorySize(MAX_HISTORY_SIZE)
{
    // Initialize with basic memry info
    m_currentData.totalRAM = SystemUtils::getTotalMemory();
}

MemoryData MemoryMonitor::getCurrentData() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_currentData;
}

QVector<MemoryData> MemoryMonitor::getHistory() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_history;
}

void MemoryMonitor::setHistorySize(int size)
{
    m_maxHistorySize = qBound(10, size, 1000);
}

double MemoryMonitor::getMemoryEfficiency() const
{
    QMutexLocker locker(&m_dataMutex);
    if (m_currentData.totalRAM <= 0) return 0.0;

    return static_cast<double>(m_currentData.availableRAM) / m_currentData.totalRAM * 100.0;
}

qint64 MemoryMonitor::getMemoryPressure() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_currentData.usedRAM - m_currentData.buffers - m_currentData.cached;
}

bool MemoryMonitor::isSwapping() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_currentData.swapUsed > 0;
}

void MemoryMonitor::collectData()
{
    collectMemoryInfo();
    collectSwapInfo();
}

void MemoryMonitor::processData()
{
    m_currentData.usagePercentage = calculateUsagePercent();
    m_currentData.swapPercentage = calculateSwapPercent();
    m_currentData.status = determineStatus();
    m_currentData.timestamp = QDateTime::currentDateTime();
}

void MemoryMonitor::validateData()
{
    // Validate percentages
    if (!SystemUtils::isValidPercentage(m_currentData.usagePercentage)) {
        m_currentData.usagePercentage = 0.0;
    }
    if (!SystemUtils::isValidPercentage(m_currentData.swapPercentage)) {
        m_currentData.swapPercentage = 0.0;
    }

    // Validate memory values (should be positive)
    if (m_currentData.totalRAM < 0) m_currentData.totalRAM = 0;
    if (m_currentData.usedRAM < 0) m_currentData.usedRAM = 0;
    if (m_currentData.availableRAM < 0) m_currentData.availableRAM = 0;
}

void MemoryMonitor::emitSignal()
{
    emit memoryDataUpdated(m_currentData);

    // Emit threshold warnings
    if (m_currentData.usagePercentage >= RAM_CRITICAL_THRESHOLD) {
        emit memoryCritical(m_currentData.usagePercentage);
    } else if (m_currentData.usagePercentage >= RAM_WARNING_THRESHOLD) {
        emit memoryWarning(m_currentData.usagePercentage);
    }

    // Emit swap warnings
    if (m_currentData.swapPercentage > 50.0) {
        emit swapWarning(m_currentData.swapPercentage);
    }

    // Emit low memory alert
    if (m_currentData.availableRAM < LOW_MEMORY_THRESHOLD) {
        emit lowMemoryAlert(m_currentData.availableRAM);
    }

    // Update history
    m_history.append(m_currentData);
    if (m_history.size() > m_maxHistorySize) {
        m_history.removeFirst();
    }
}

void MemoryMonitor::collectMemoryInfo()
{
    m_currentData.totalRAM = SystemUtils::getTotalMemory();
    m_currentData.freeRAM = SystemUtils::getFreeMemory();
    m_currentData.availableRAM = SystemUtils::getAvailableMemory();
    m_currentData.buffers = SystemUtils::getBufferMemory();
    m_currentData.cached = SystemUtils::getCacheMemory();
}

void MemoryMonitor::collectSwapInfo()
{
    // Read swap info from /proc/meminfo
    QString swapTotalStr = SystemUtils::extractValueFromProcFile(PROC_MEMINFO, "SwapTotal");
    QString swapFreeStr = SystemUtils::extractValueFromProcFile(PROC_MEMINFO, "SwapFree");

    // Parse swap values (format: "123456 kB")
    QRegularExpression regex("(\\d+)\\s*kB");

    QRegularExpressionMatch totalMatch = regex.match(swapTotalStr);
    if (totalMatch.hasMatch()) {
        m_currentData.swapTotal = totalMatch.captured(1).toLongLong() * 1024;
    }

    QRegularExpressionMatch freeMatch = regex.match(swapFreeStr);
    if (freeMatch.hasMatch()) {
        qint64 swapFree = freeMatch.captured(1).toLongLong() * 1024;
        m_currentData.swapUsed = m_currentData.swapTotal - swapFree;
    }
}

double MemoryMonitor::calculateUsagePercent() const
{
    if (m_currentData.totalRAM <= 0) return 0.0;

    double usage = static_cast<double>(m_currentData.usedRAM) / m_currentData.totalRAM * 100.0;
    return qBound(0.0, usage, 100.0);
}

double MemoryMonitor::calculateSwapPercent() const
{
    if (m_currentData.swapTotal <= 0) return 0.0;

    double usage = static_cast<double>(m_currentData.swapUsed) / m_currentData.swapTotal * 100.0;
    return qBound(0.0, usage, 100.0);
}

MetricStatus MemoryMonitor::determineStatus() const
{
    // Check RAM usage thresholds
    if (m_currentData.usagePercentage >= RAM_CRITICAL_THRESHOLD) {
        return MetricStatus::Critical;
    }
    if (m_currentData.usagePercentage >= RAM_WARNING_THRESHOLD) {
        return MetricStatus::Warning;
    }

    // Check low memory condition
    if (m_currentData.availableRAM < LOW_MEMORY_THRESHOLD) {
        return MetricStatus::Warning;
    }

    return MetricStatus::Normal;
}
