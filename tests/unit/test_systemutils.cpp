/**
 * @file test_systemutils.cpp
 * @brief Test implementation for SystemUtils
 */

#include "test_systemutils.h"
#include "core/systemutils.h"
#include "core/constants.h"

#include <QTemporaryFile>
#include <QTextStream>

// File I/O tests
void TestSystemUtils::testFileExists()
{
    // Test existing file
    QVERIFY(SystemUtils::fileExists(PROC_STAT));

    // Test non-existing file
    QVERIFY(!SystemUtils::fileExists("/nonexistent/file"));
}

void TestSystemUtils::testReadFile()
{
    // Create temp file
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());

    QTextStream stream(&tempFile);
    stream << "test content\nLine 2";
    tempFile.close();

    // Test reading
    QString content = SystemUtils::readFile(tempFile.fileName());
    QVERIFY(content.contains("test content"));
    QVERIFY(content.contains("Line 2"));
}

// System info tests
void TestSystemUtils::testGetHostname()
{
    QString hostname = SystemUtils::getHostname();
    QVERIFY(!hostname.isEmpty());
    QVERIFY(hostname != "unknown");
    qDebug() << "Hostname: " << hostname;
}

void TestSystemUtils::testGetKernelVersion()
{
    QString version = SystemUtils::getKernelVersion();
    QVERIFY(!version.isEmpty());
    QVERIFY(version != "unknown");
    qDebug() << "Kernel:" << version;
}

void TestSystemUtils::testGetUptime()
{
    QString uptime = SystemUtils::getUptime();
    QVERIFY(!uptime.isEmpty());
    QVERIFY(uptime != "unknown");
    qDebug() << "Uptime:" << uptime;
}

// CPU tests
void TestSystemUtils::testGetCPUCoreCount()
{
    int cores = SystemUtils::getCPUCoreCount();
    QVERIFY(cores > 0);
    QVERIFY(cores <= 64); // Reasonable upper limit
    qDebug() << "CPU cores: " << cores;
}

void TestSystemUtils::testGetCPUModel()
{
    QString model = SystemUtils::getCPUModel();
    QVERIFY(!model.isEmpty());
    qDebug() << "CPU model: " << model;
}

void TestSystemUtils::testGetCPUTemperature()
{

    double temp = SystemUtils::getCPUTemperature();
    if (temp > 0) {
        QVERIFY(temp > 10.0 && temp < 100.0); // Reasonable range
        qDebug() << "CPU temp:" << temp << "°C";
    }
    else {
        qDebug() << "CPU temperature not available";
    }
}

// Memory tests
void TestSystemUtils::testGetTotalMemory()
{
    qint64 total = SystemUtils::getTotalMemory();
    QVERIFY(total > 0);
    qDebug() << "Total RAM:" << SystemUtils::formatBytes(total);
}

void TestSystemUtils::testGetAvailableMemory()
{
    qint64 total = SystemUtils::getTotalMemory();
    qint64 available = SystemUtils::getAvailableMemory();

    QVERIFY(available >= 0);
    QVERIFY(available <= total);
    qDebug() << "Available RAM:" << SystemUtils::formatBytes(available);
}

// Network tests
void TestSystemUtils::testGetNetworkInterfaces()
{
    QStringList interfaces = SystemUtils::getNetworkInterfaces();
    QVERIFY(!interfaces.isEmpty());
    QVERIFY(interfaces.contains("lo")); // Should have loopback
    qDebug() << "Network interfaces:" << interfaces;
}

void TestSystemUtils::testGetActiveNetworkInterface()
{
    QString active = SystemUtils::getActiveNetworkInterface();
    QVERIFY(!active.isEmpty());
    QVERIFY(active != "unknown");
    qDebug() << "Active interface:" << active;
}

// Storage test
void TestSystemUtils::testGetStorageTotal()
{
    qint64 total = SystemUtils::getStorageTotal("/");
    QVERIFY(total > 0);
    qDebug() << "Storage total:" << SystemUtils::formatBytes(total);
}

void TestSystemUtils::testGetStorageUsed()
{
    qint64 total = SystemUtils::getStorageTotal("/");
    qint64 used = SystemUtils::getStorageUsed("/");

    QVERIFY(used >= 0);
    QVERIFY(used <= total);
    qDebug() << "Storage used:" << SystemUtils::formatBytes(used);
}

// Format tests
void TestSystemUtils::testFormatBytes()
{
    QCOMPARE(SystemUtils::formatBytes(0), "0 B");
    QCOMPARE(SystemUtils::formatBytes(1024), "1.0 KB");
    QCOMPARE(SystemUtils::formatBytes(1024 * 1024), "1.0 MB");
    QCOMPARE(SystemUtils::formatBytes(1024 * 1024 * 1024), "1.0 GB");
}

void TestSystemUtils::testFormatUptime()
{
    QCOMPARE(SystemUtils::formatUptime(0), "0s");
    QCOMPARE(SystemUtils::formatUptime(60), "1m");
    QCOMPARE(SystemUtils::formatUptime(3600), "1h");
    QCOMPARE(SystemUtils::formatUptime(86400), "1d");
    QCOMPARE(SystemUtils::formatUptime(90061), "1d 1h 1m 1s");
}

void TestSystemUtils::testFormatPercentage()
{
    QCOMPARE(SystemUtils::formatPercentage(0.0), "0.0%");
    QCOMPARE(SystemUtils::formatPercentage(50.5), "50.5%");
    QCOMPARE(SystemUtils::formatPercentage(100.0), "100.0%");
}

void TestSystemUtils::testFormatTemperature()
{
    QCOMPARE(SystemUtils::formatTemperature(0.0), "0.0°C");
    QCOMPARE(SystemUtils::formatTemperature(25.5), "25.5°C");
    QCOMPARE(SystemUtils::formatTemperature(100.0), "100.0°C");
}

// Validation tests
void TestSystemUtils::testIsValidPercentage()
{
    QVERIFY(SystemUtils::isValidPercentage(0.0));
    QVERIFY(SystemUtils::isValidPercentage(50.0));
    QVERIFY(SystemUtils::isValidPercentage(100.0));

    QVERIFY(!SystemUtils::isValidPercentage(-1.0));
    QVERIFY(!SystemUtils::isValidPercentage(101.0));
}

void TestSystemUtils::testIsValidTemperature()
{
    QVERIFY(SystemUtils::isValidTemperature(0.0));
    QVERIFY(SystemUtils::isValidTemperature(25.0));
    QVERIFY(SystemUtils::isValidTemperature(100.0));

    QVERIFY(!SystemUtils::isValidTemperature(-50.0));
    QVERIFY(!SystemUtils::isValidTemperature(200.0));
}

