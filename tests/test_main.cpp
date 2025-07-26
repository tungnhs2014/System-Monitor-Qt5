/**
 * @file test_main.cpp
 * @brief Test runner for SystemMonitor
 */

#include <QCoreApplication>
#include <QTest>
#include <QDebug>

#include "unit/test_systemutils.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "=== SystemMonitor Tests ===";

    int result = 0;

    // Run SystemUtils tests
    {
        TestSystemUtils test;
        result += QTest::qExec(&test, argc, argv);
    }

    if (result == 0) {
        qDebug() << "All tests PASSED!";
    } else {
        qDebug() << "Tests FAILED!";
    }

    return result;
}
