/**
 * @file main.cpp
 * @brief SystemMonitor
 * @author TungNHS
 * @version 1.0.0
 */
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <memory>
#include "core/systemutils.h"
#include "core/constants.h"
#include "model/managers/datamanager.h"
#include "model/managers/alertmanager.h"

class SystemMonitorDemo
{
public:
    SystemMonitorDemo() {
        m_dataManager = std::make_unique<DataManager>();
        m_demoTimer = std::make_unique<QTimer>();
        m_updateCount = 0;

        // Use lambda functions instead of slots
        QObject::connect(m_dataManager.get(), &DataManager::systemDataUpdated,
                         [this](const SystemOverview& data) {
                             this->onSystemUpdate(data);
                         });

        QObject::connect(m_dataManager->getAlertManager(), &AlertManager::alertAdded,
                         [this](const AlertData& alert) {
                             this->onAlert(alert);
                         });

        QObject::connect(m_demoTimer.get(), &QTimer::timeout,
                         [this]() {
                             this->checkExit();
                         });

        m_demoTimer->start(1000); // Check every second
    }

    ~SystemMonitorDemo() {
        if (m_dataManager) {
            m_dataManager->stop();
        }
    }

    void start() {
        printHeader();
        m_dataManager->initialize();
        m_dataManager->start();
    }

private:
    void onSystemUpdate(const SystemOverview& data) {
        m_updateCount++;
        qDebug() << QString("[%1] CPU:%2% Temp:%3°C | MEM:%4% Used:%5")
                        .arg(m_updateCount, 2)
                        .arg(data.cpu.totalUsage, 5, 'f', 1)
                        .arg(data.cpu.temperature, 4, 'f', 1)
                        .arg(data.memory.usagePercentage, 5, 'f', 1)
                        .arg(SystemUtils::formatBytes(data.memory.usedRAM));
    }

    void onAlert(const AlertData& alert) {
        QString level = (alert.severity == AlertSeverity::Critical) ? "CRITICAL" : "WARNING";
        qDebug() << QString("%1: %2").arg(level).arg(alert.message);
    }

    void checkExit() {
        if (m_updateCount >= 20) { // Demo for 20 updates
            qDebug() << "\nDemo completed successfully!";
            QCoreApplication::quit();
        }
    }

    void printHeader() {
        qDebug() << "=== SystemMonitor Demo ===";
        qDebug() << "App:" << APP_NAME << "v" << APP_VERSION;
        qDebug() << "";

        // Phase 1 - System Info
        qDebug() << "--- SYSTEM INFO (Phase 1) ---";
        qDebug() << "Hostname:" << SystemUtils::getHostname();
        qDebug() << "Kernel:" << SystemUtils::getKernelVersion();
        qDebug() << "CPU Model:" << SystemUtils::getCPUModel();
        qDebug() << "CPU Cores:" << SystemUtils::getCPUCoreCount();
        qDebug() << "Total RAM:" << SystemUtils::formatBytes(SystemUtils::getTotalMemory());
        qDebug() << "Uptime:" << SystemUtils::getUptime();
        qDebug() << "";
        qDebug() << "--- REAL-TIME MONITORING (Phase 2) ---";
    }

    std::unique_ptr<DataManager> m_dataManager;
    std::unique_ptr<QTimer> m_demoTimer;
    int m_updateCount;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Register Qt metatypes
    qRegisterMetaType<CPUData>("CPUData");
    qRegisterMetaType<MemoryData>("MemoryData");
    qRegisterMetaType<SystemOverview>("SystemOverview");
    qRegisterMetaType<AlertData>("AlertData");

    SystemMonitorDemo demo;
    demo.start();

    return app.exec();
}
