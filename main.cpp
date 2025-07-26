
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


    return app.exec();
}
