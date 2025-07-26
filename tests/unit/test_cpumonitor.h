/**
 * @file test_cpumonitor.h
 * @brief CPUMonitor unit tests
 */

#ifndef TEST_CPUMONITOR__H
#define TEST_CPUMONITOR__H

#include <QObject>
#include <QTest>
#include <QSignalSpy>
#include <memory>
#include "model/monitors/cpumonitor.h"

class TestCPUMonitor : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality
    void testConstructor();
    void testStartStop();
    void testDataCollection();
    void testDataValidation();


private:
    std::unique_ptr<CPUMonitor> m_monitor;
    bool m_isRaspberryPi;
};


#endif // TEST_CPUMONITOR__H
