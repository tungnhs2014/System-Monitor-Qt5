/**
 * @file systemutils.cpp
 * @brief System utilities imlementation for PI 3B+ monitoring
 * @author TungNHS
 * @version 1.0.0
 */

#include "systemutils.h"
#include "constants.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QHostInfo>
#include <QStorageInfo>
#include <QDebug>
#include <QDir>
#include <QProcess>

// ===================================================================
// FILE I/O OPERATIONS
// ===================================================================

QString SystemUtils::readFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot read file: " << filePath << "Error: " << file.errorString();
        return QString();
    }

    QString content = file.readAll();
    file.close();

    return content.trimmed();
}

QStringList SystemUtils::readFileLines(const QString &filePath)
{
    QString content = readFile(filePath);
    if (content.isEmpty()) {
        return QStringList();
    }

    return content.split('\n', Qt::SkipEmptyParts);
}

bool SystemUtils::fileExists(const QString &filePath)
{
    QFile file(filePath);
    return file.exists() && file.open(QIODevice::ReadOnly);
}

// ===================================================================
// DATA PARSING UTILITIES
// ===================================================================

double SystemUtils::parseDouble(const QString &str, bool *ok)
{
    bool conversionOk = false;
    double result = str.trimmed().toDouble(&conversionOk);

    if (ok) {
        *ok = conversionOk;
    }

    if (!conversionOk) {
        qWarning() << "Failed to parse double: " << str;
        return 0.0;
    }

    return result;
}

qint64 SystemUtils::parseInt64(const QString &str, bool *ok)
{
    bool conversionOk = false;
    qint64 result = str.trimmed().toLongLong(&conversionOk);

    if (ok) {
        *ok = conversionOk;
    }

    if (!conversionOk) {
        qWarning() << "Failed to parse int64: " << str;
        return 0;
    }

    return result;
}

// ===================================================================
// SYSTEM INFORMATION
// ===================================================================

QString SystemUtils::getHostname()
{
    // Try readng from /proc/sys/kernel/host/first
    QString hostname = readFile("/proc/sys/kernel/hostname");

    if (hostname.isEmpty()) {
        // Fallback to QHostInfo
        hostname = QHostInfo::localHostName();
    }

    return hostname.isEmpty() ? "Unknown" : hostname;
}

QString SystemUtils::getKernelVersion()
{
    QString version = readFile(PROC_VERSION);
    if (version.isEmpty()) {
        return "Unknown";
    }

    // Parse kernel version from /proc/version
    // Format: "Linux version 5.10.x-xxx...""
    QRegularExpression regex("Linux version ([\\d\\.\\-\\w]+)");
    QRegularExpressionMatch match = regex.match(version);

    if (match.hasMatch()) {
        return match.captured(1);
    }

    return "Unknown";
}

QString SystemUtils::getUptime()
{
    QString uptimeStr = readFile(PROC_UPTIME);
    if (uptimeStr.isEmpty()) {
        return "Unknown";
    }

    // /proc/uptime format: "12345.67 8901.23"
    // First number is total uptime inn seconds
    QStringList parts = uptimeStr.split(' ');
    if (parts.isEmpty()) {
        return "Unknown";
    }

    bool ok;
    double uptimeSeconds = parseDouble(parts[0], &ok);
    if (!ok) {
        return "Unknown";
    }

    return formatUptime(static_cast<qint64>(uptimeSeconds));
}

QDateTime SystemUtils::getBootTime()
{
    QString uptimeStr = readFile(PROC_UPTIME);
    if (uptimeStr.isEmpty()) {
        return QDateTime();
    }

    QStringList parts = uptimeStr.split(' ');
    if (parts.isEmpty()) {
        return QDateTime();
    }

    bool ok;
    double uptimeSeconds = parseDouble(parts[0], &ok);
    if (!ok) {
        return QDateTime();
    }

    // Boot time = current time - up time
    return QDateTime::currentDateTime().addSecs(-static_cast<qint64>(uptimeSeconds));
}

// ===================================================================
// CPU INFORMATION (Pi 3B+ ARM Cortex-A53)
// ===================================================================

int SystemUtils::getCPUCoreCount()
{
    QStringList lines = readFileLines(PROC_CPUINFO);
    int coreCount = 0;

    for (const QString &line : lines) {
        if (line.startsWith("processor")) {
            coreCount++;
        }
    }

    // Pi 3B+ should have 4 core, fallback if detection fails
    return (coreCount > 0) ? coreCount : 4;
}

QString SystemUtils::getCPUModel()
{
    QString model = extractValueFromProcFile(PROC_CPUINFO, "model name");

    if (model.isEmpty()) {
        // Try "Hardware" field for ARM processors
        model = extractValueFromProcFile(PROC_CPUINFO, "Hardware");
    }

    if (model.isEmpty()) {
        // Fallback for pi 3B+
        model = "ARM Cortex-A53";
    }

    return model;
}

double SystemUtils::getCPUFrequency()
{
    QString freStr = readFile(CPUFREQ_PATH);
    if (freStr.isEmpty()) {
        return 0.0;
    }

    bool ok;
    qint64 freqKHz = parseInt64(freStr, &ok);
    if (!ok) {
        return 0.0;
    }

    // Convert kHz to MHz
    return freqKHz / 1000.0;
}

double SystemUtils::getCPUTemperature()
{
    QString tempStr = readFile(THERMAL_ZONE_PATH);
    if (tempStr.isEmpty()) {
        return 0.0;
    }

    bool ok;
    qint64 tempMilliC = parseInt64(tempStr, &ok);
    if (!ok) {
        return 0.0;
    }

    // Convert milliCelsius to Celsius
    double tempC = tempMilliC / 1000.0;

    // Valicate temperature range for Pi 3B+
    if (!isValidTemperature(tempC)) {
        qWarning() << "Invalid CPU temperature: " << tempC;
        return 0.0;
    }

    return tempC;
}

// ===================================================================
// MEMORY INFORMATION (Pi 3B+ 1GB RAM)
// ===================================================================

qint64 SystemUtils::getTotalMemory()
{
    QString value = extractValueFromProcFile(PROC_MEMINFO, "MemTotal");
    return parseMemoryLine("MemTotal: " + value);
}

qint64 SystemUtils::getAvailableMemory()
{
    QString value = extractValueFromProcFile(PROC_MEMINFO, "MemAvailable");
    return parseMemoryLine("MemAvailable: " + value);
}

qint64 SystemUtils::getFreeMemory()
{
    QString value = extractValueFromProcFile(PROC_MEMINFO, "MemFree");
    return parseMemoryLine("MemFree: " + value);
}

qint64 SystemUtils::getBufferMemory()
{
    QString value = extractValueFromProcFile(PROC_MEMINFO, "Buffers");
    return parseMemoryLine("Buffers: " + value);
}

qint64 SystemUtils::getCacheMemory()
{
    QString value = extractValueFromProcFile(PROC_MEMINFO, "Cached");
    return parseMemoryLine("Cached: " + value);
}

// ===================================================================
// NETWORK INFORMATION
// ===================================================================

QStringList SystemUtils::getNetworkInterfaces()
{
    QStringList interfaces;
    QStringList lines = readFileLines(PROC_NET_DEV);

    // Skip header lines (first 2 lines)
    for (int i = 2; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();
        if (line.isEmpty()) continue;

        // Extract interface name (before first colon)
        int colonIndex = line.indexOf(':');
        if (colonIndex > 0) {
            QString interfaceName = line.left(colonIndex).trimmed();
            interfaces.append(interfaceName);
        }
    }

    return interfaces;
}

QString SystemUtils::getActiveNetworkInterface()
{
    QStringList interfaces = getNetworkInterfaces();

    // Prefer common interfaces in order
    QStringList preferredOrder = {"eth0", "wlan0", "enp0s3", "wlp2s0"};

    for (const QString& preferred : preferredOrder) {
        if (interfaces.contains(preferred)) {
            return preferred;
        }
    }

    // Return first non-loopback interface
    for (const QString& iface : interfaces) {
        if (iface != "lo") {
            return iface;
        }
    }

    return interfaces.isEmpty() ?  "unknown" : interfaces.first();
}

// ===================================================================
// STORAGE INFORMATION
// ===================================================================

qint64 SystemUtils::getStorageTotal(const QString &path)
{
    QStorageInfo storage(path);
    if (!storage.isValid()) {
        qWarning() << "Invalid storage path: " << path;
        return 0;
    }

    return storage.bytesTotal();
}

qint64 SystemUtils::getStorageUsed(const QString &path)
{
    QStorageInfo storage(path);
    if (!storage.isValid()) {
        return 0;
    }

    return storage.bytesTotal() - storage.bytesAvailable();
}

qint64 SystemUtils::getStorageAvailable(const QString &path)
{
    QStorageInfo storage(path);
    if (!storage.isValid()) {
        return 0;
    }

    return storage.bytesAvailable();
}

// ===================================================================
// FORMAT UTILITIES
// ===================================================================

QString SystemUtils::formatBytes(qint64 bytes)
{
    if (bytes < 0) {
        return "0 B";
    }

    const QStringList units = {"B", "KB", "MB", "GB", "TB"};
    double size = bytes;
    int unitIndex = 0;

    while (size >= 1024.0 && unitIndex < units.size() - 1) {
        size /= 1024.0;
        unitIndex++;
    }

    return QString("%1 %2").arg(size, 0, 'f', (unitIndex == 0) ? 0 : 1).arg(units[unitIndex]);
}

QString SystemUtils::formatUptime(qint64 seconds)
{
    if (seconds < 0) {
        return "0s";
    }

    qint64 days = seconds / 86400;
    qint64 hours = (seconds % 86400) / 3600;
    qint64 minutes = (seconds % 3600) / 60;
    qint64 secs = seconds % 60;

    QStringList parts;

    if (days > 0) {
        parts.append(QString("%1d").arg(days));
    }
    if (hours > 0) {
        parts.append(QString("%1h").arg(hours));
    }
    if (minutes > 0) {
        parts.append(QString("%1m").arg(minutes));
    }
    if (secs > 0 || parts.isEmpty()) {
        parts.append(QString("%1s").arg(secs));
    }

    return parts.join(" ");
}

QString SystemUtils::formatPercentage(double percentage)
{
    return QString("%1%").arg(percentage, 0, 'f', 1);
}

QString SystemUtils::formatTemperature(double celsius)
{
    return QString("%1°C").arg(celsius, 0, 'f', 1);
}

// ===================================================================
// VALIDATION UTILITIES
// ===================================================================

bool SystemUtils::isValidPercentage(double value)
{
    return value >= 0.0 && value <= 100.0;
}

bool SystemUtils::isValidTemperature(double celsius)
{
    // Reasonable temperature range for Pi 3B+: -40°C to 150°C
    return celsius >= -40.0 && celsius <= 150.0;
}

// ===================================================================
// PERFORMANCE UTILITIES
// ===================================================================

double SystemUtils::calculateCPUUsage(qint64 totalTime, qint64 idleTime, qint64 lastTotalTime, qint64 lastIdleTime)
{
    qint64 totalDiff = totalTime - lastTotalTime;
    qint64 idleDiff = idleTime - lastIdleTime;

    if (totalDiff <= 0) {
        return 0.0;
    }

    double usage = (1.0 - static_cast<double>(idleDiff) / totalDiff) * 100.0;

    // Clamp to valid percentage range
    usage = qMax(0.0, qMin(100.0, usage));

    return usage;
}

// ===================================================================
// INTERNAL HELPER METHODS
// ===================================================================

QString SystemUtils::extractValueFromProcFile(const QString& filePath, const QString& key)
{
    QStringList lines = readFileLines(filePath);

    for (const QString& line: lines) {
        if (line.startsWith(key, Qt::CaseInsensitive)) {
            // Find colon separator
            int colonIndex = line.indexOf(':');
            if (colonIndex > 0) {
                return line.mid(colonIndex + 1).trimmed();
            }
        }
    }

    return QString();
}

qint64 SystemUtils::parseMemoryLine(const QString &line)
{
    // Parse lines like "MemTotal:    1000000 kB"
    QRegularExpression regex("(\\d+)\\s*kB");
    QRegularExpressionMatch match = regex.match(line);

    if (match.hasMatch()) {
        bool ok;
        qint64 kilobytes = parseInt64(match.captured(1), &ok);
        if (ok) {
            return kilobytes * 1024; // Convert kB to bytes
        }
    }

    return 0;
}
