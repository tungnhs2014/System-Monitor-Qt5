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




