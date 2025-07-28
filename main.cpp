
/**
 * @file main.cpp
 * @brief SystemMonitor
 * @author TungNHS
 * @version 1.0.0
 */

#include <QCoreApplication>
#include <QDebug>

#include "core/constants.h"
#include "core/systemutils.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "=== SystemMonitor Phase 1 Demo ===";
    qDebug() << "Applicatin: " << APP_NAME << " v" << APP_VERSION;

    // System Info
    qDebug() << "--- SYSTEM INFO ---";
    qDebug() << "Hostname: " << SystemUtils::getHostname();
    qDebug() << "Kernel: " << SystemUtils::getKernelVersion();
    qDebug() << "Uptime: " << SystemUtils::getUptime();

    // CPU Info
    qDebug() << "--- CPU INFO ---";
    qDebug() << "Cores: " << SystemUtils::getCPUCoreCount();
    qDebug() << "Model: " << SystemUtils::getCPUModel();
    qDebug() << "Frequency: " << SystemUtils::getCPUFrequency();
    qDebug() << "Temperature: " << SystemUtils::formatTemperature(SystemUtils::getCPUTemperature());

    // Memory Info
    qDebug() << "---MEMORY INFO ---";
    qint64 totalRAM = SystemUtils::getTotalMemory();
    qint64 availableRAM = SystemUtils::getAvailableMemory();

    qDebug() << "Total RAM: " << SystemUtils::formatBytes(totalRAM);
    qDebug() << "Available RAM: " << SystemUtils::formatBytes(availableRAM);

    if (totalRAM > 0) {
        double usagePercent = ((double)(totalRAM - availableRAM) / totalRAM) * 100.0;
        qDebug() << "Memory Usage: " << SystemUtils::formatPercentage(usagePercent);
    }

    // Network Info
    qDebug() << "---NETWORK INFO ---";
    QStringList interfaces = SystemUtils::getNetworkInterfaces();
    qDebug() << "Interfaces: " << interfaces.join(", ");
    qDebug() << "Active: " << SystemUtils::getActiveNetworkInterface();

    // Storage info
    qDebug() << "---STORAGE INFO ---";
    qint64 total = SystemUtils::getStorageTotal("/");
    qint64 used = SystemUtils::getStorageUsed("/");
    qint64 available = SystemUtils::getStorageAvailable("/");

    if (total > 0) {
        double usagePercent = ((double)used / total) * 100.0;
        qDebug() << "root (/): " << SystemUtils::formatBytes(total)
                 << " total, "<< SystemUtils::formatBytes(used)
                 << " used (" << SystemUtils::formatPercentage(usagePercent) << ") "
                 << SystemUtils::formatBytes(available);
    }

    qDebug() << "=== Demo Complete";

    return app.exec();
}
