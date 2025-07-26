/**
 * @file test_main.cpp
 * @brief Test runner for SystemMonitor
 */

#include <QCoreApplication>
#include <QTest>
#include <QDebug>

#include "unit/test_systemutils.h"
#include "unit/test_cpumonitor.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "=== SystemMonitor Tests ===";

    int result = 0;

    // Phase 1 Tests
    qDebug() << "\n--- Phase 1: Core Foundation Tests ---";
    {
        TestSystemUtils test;
        result += QTest::qExec(&test, argc, argv);
    }
    // Phase 2 Tests
    qDebug() << "\n--- Phase 2: Monitoring Classes Tests ---";
    {
        TestCPUMonitor test;
        result += QTest::qExec(&test, argc, argv);
    }

    qDebug() << "\n=== Test Results ===";
    if (result == 0) {
        qDebug() << "All tests PASSED!";
    } else {
        qDebug() << "Tests FAILED!";
    }

    return result;
}
