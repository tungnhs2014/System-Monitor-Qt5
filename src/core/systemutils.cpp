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

QString SystemUtils::formatUptime(qint64 seconds)
{

}

bool SystemUtils::isValidTemperature(double celsius)
{

}

QString SystemUtils::extractValueFromProcFile(const QString &filePath, const QString &key)
{

}
