/**
 * @file test_systemutils.h
 * @brief Tests for SystemUtils class
 */

#ifndef TEST_SYSTEMUTILS_H
#define TEST_SYSTEMUTILS_H

#include <QObject>
#include <QTest>

class TestSystemUtils : public QObject
{
    Q_OBJECT

private slots:
    // File I/O tests
    void testFileExists();
    void testReadFile();

    // System info tests
    void testGetHostname();
    void testGetKernelVersion();
    void testGetUptime();

    // CPU tests
    void testGetCPUCoreCount();
    void testGetCPUModel();
    void testGetCPUTemperature();

    // Memory tests
    void testGetTotalMemory();
    void testGetAvailableMemory();

    // Network tests
    void testGetNetworkInterfaces();
    void testGetActiveNetworkInterface();

    // Storage tests
    void testGetStorageTotal();
    void testGetStorageUsed();

    // Format tests
    void testFormatBytes();
    void testFormatUptime();
    void testFormatPercentage();
    void testFormatTemperature();

    // Validation tests
    void testIsValidPercentage();
    void testIsValidTemperature();
};

#endif // TEST_SYSTEMUTILS_H
