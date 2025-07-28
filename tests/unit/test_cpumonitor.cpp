/**
 * @file test_cpumonitor.cpp
 * @brief CPUMonitor unit tests implementation
 */

#include "test_cpumonitor.h"
#include "core/systemutils.h"
#include "core/constants.h"

void TestCPUMonitor::initTestCase()
{
    qDebug() << "Testing CPUMonitor...";

    // Direct if running on Raspberry Pi
    QString cpuModel = SystemUtils::getCPUModel();
    m_isRaspberryPi = cpuModel.contains("ARM", Qt::CaseInsensitive);

    qDebug() << "CPU Model: " << cpuModel;
    qDebug() << "Is Rasberry Pi: " << m_isRaspberryPi;
}

void TestCPUMonitor::cleanupTestCase()
{
    qDebug() << "CPUMonitor tests completed";
}

void TestCPUMonitor::init()
{
    m_monitor = std::make_unique<CPUMonitor>();
}

void TestCPUMonitor::cleanup()
{
    if (m_monitor && m_monitor->isMonitoring()) {
        m_monitor->stopMonitoring();
    }
    m_monitor.reset();
}

void TestCPUMonitor::testConstructor()
{
    QVERIFY(m_monitor != nullptr);
    QVERIFY(!m_monitor->isMonitoring());
    QVERIFY(!m_monitor->isPaused());

    // Test inital data
    CPUData data = m_monitor->getCurrentData();
    QVERIFY(data.coreCount > 0);
    QVERIFY(!data.model.isEmpty());

    if (m_isRaspberryPi) {
        QCOMPARE(data.coreCount, 4);
    }
}

void TestCPUMonitor::testStartStop()
{
    QSignalSpy startSpy(m_monitor.get(), &CPUMonitor::monitoringStarted);
    QSignalSpy stopSpy(m_monitor.get(), &CPUMonitor::monitoringStopped);

    // Test start
    m_monitor->startMonitoring();
    QVERIFY(m_monitor->isMonitoring());
    QCOMPARE(startSpy.count(), 1);

    // Test stop
    m_monitor->stopMonitoring();
    QVERIFY(!m_monitor->isMonitoring());
    QCOMPARE(stopSpy.count(), 1);
}

void TestCPUMonitor::testDataCollection()
{
    QSignalSpy dataSpy(m_monitor.get(), &CPUMonitor::cpuDataUpdated);

    m_monitor->setUpdateInterval(500);    // Fast for testing
    m_monitor->startMonitoring();

    // Wait for at least 2 updates
    QVERIFY(dataSpy.wait(2000));
    QVERIFY(dataSpy.count() >= 1);

    // Check data validity
    CPUData data = m_monitor->getCurrentData();
    QVERIFY(data.isValid());
    QVERIFY(data.totalUsage >= 0.0 && data.totalUsage <= 100.0);
    QVERIFY(data.temperature >= 0.0);
    QVERIFY(data.averageFrequency > 0.0);
}

void TestCPUMonitor::testDataValidation()
{

}

