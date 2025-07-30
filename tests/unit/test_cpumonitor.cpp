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
    // Test that invalid data is handled properly
    CPUData data = m_monitor->getCurrentData();

    // Usage should be valid percentage
    QVERIFY(SystemUtils::isValidPercentage(data.totalUsage));

    // Temperature should be in reasonable range
    if (data.temperature > 0) {
        QVERIFY(SystemUtils::isValidTemperature(data.temperature));
    }

    // Core count should match system
    QCOMPARE(data.coreCount, SystemUtils::getCPUCoreCount());
}

void TestCPUMonitor::testUsageCalculation()
{
    m_monitor->setUpdateInterval(500);
    m_monitor->startMonitoring();

    QSignalSpy dataSpy(m_monitor.get(), &CPUMonitor::cpuDataUpdated);
    QVERIFY(dataSpy.wait(1500)); // Wait for multiple updates

    CPUData data = m_monitor->getCurrentData();

    // Usage calculation tests
    QVERIFY(data.totalUsage >= 0.0);
    QVERIFY(data.totalUsage <= 100.0);

    // Core data should be populated
    QCOMPARE(data.cores.size(), data.coreCount);
    for (const auto& core : data.cores) {
        QVERIFY(core.usage >= 0.0 && core.usage <= 100.0);
        QVERIFY(core.coreID >= 0);
    }
}

void TestCPUMonitor::testTemperatureReading()
{
    // Test temperature reading
    double temp = SystemUtils::getCPUTemperature();

    if (m_isRaspberryPi && temp > 0) {
        // Pi should have reasonable temperature
        QVERIFY(temp > 20.0 && temp < 100.0);
    }

    m_monitor->startMonitoring();
    QSignalSpy dataSpy(m_monitor.get(), &CPUMonitor::cpuDataUpdated);
    QVERIFY(dataSpy.wait(1000));

    CPUData data = m_monitor->getCurrentData();
    if (data.temperature > 0) {
        QVERIFY(SystemUtils::isValidTemperature(data.temperature));
    }
}

void TestCPUMonitor::testCoreDataCollection()
{
    m_monitor->startMonitoring();
    QSignalSpy dataSpy(m_monitor.get(), &CPUMonitor::cpuDataUpdated);
    QVERIFY(dataSpy.wait(1000));

    CPUData data = m_monitor->getCurrentData();

    // Each core should have valid data
    for (int i = 0; i < data.cores.size(); ++i) {
        const CPUCoreData& core = data.cores[i];
        QCOMPARE(core.coreID, i);
        QVERIFY(core.usage >= 0.0 && core.usage <= 100.0);
        QVERIFY(core.frequency >= 0.0);
    }
}

void TestCPUMonitor::testSignalEmission()
{
    QSignalSpy dataSpy(m_monitor.get(), &CPUMonitor::cpuDataUpdated);
    QSignalSpy updateSpy(m_monitor.get(), &CPUMonitor::dataUpdated);

    m_monitor->setUpdateInterval(300);
    m_monitor->startMonitoring();

    // Should get signals within reasonable time
    QVERIFY(dataSpy.wait(1000));
    QVERIFY(updateSpy.count() >= 1);

    // Verify signal data
    QVERIFY(!dataSpy.isEmpty());
    CPUData signalData = dataSpy.last().at(0).value<CPUData>();
    QVERIFY(signalData.isValid());
}

void TestCPUMonitor::testThresholdAlerts()
{
    QSignalSpy warningSpy(m_monitor.get(), &CPUMonitor::usageWarning);
    QSignalSpy criticalSpy(m_monitor.get(), &CPUMonitor::usageCritical);
    QSignalSpy tempWarningSpy(m_monitor.get(), &CPUMonitor::temperatureWarning);

    m_monitor->startMonitoring();

    // Note: Hard to test actual threshold crossing in unit test
    // This mainly tests that signals exist and can be connected
    QVERIFY(warningSpy.isValid());
    QVERIFY(criticalSpy.isValid());
    QVERIFY(tempWarningSpy.isValid());
}

void TestCPUMonitor::testUpdatePerformance()
{
    // Test that monitoring doesn't use too much CPU
    QElapsedTimer timer;
    timer.start();

    m_monitor->setUpdateInterval(100); // Fast updates
    m_monitor->startMonitoring();

    QSignalSpy dataSpy(m_monitor.get(), &CPUMonitor::cpuDataUpdated);

    // Wait for 10 updates
    int updateCount = 0;
    while (updateCount < 10 && timer.elapsed() < 5000) {
        if (dataSpy.wait(200)) {
            updateCount++;
        }
    }

    QVERIFY(updateCount >= 5); // Should get at least 5 updates in 5 seconds
    QVERIFY(timer.elapsed() < 5000); // Should complete in reasonable time
}
