/**
 * @file datamanager.cpp
 * @brief DataManager implementation for central coordination
 */

#include "datamanager.h"
#include "model/monitors/cpumonitor.h"
#include "model/monitors/memorymonitor.h"
#include "alertmanager.h"
#include "core/constants.h"
#include <QDebug>

DataManager::DataManager(QObject *parent)
    : QObject(parent)
    , m_aggregationTimer(new QTimer(this))
    , m_isInitialized(false)
    , m_isRunning(false)
    , m_isPaused(false)
    , m_updateInterval(UPDATE_INTERVAL)
{
    connect(m_aggregationTimer, &QTimer::timeout, this, &DataManager::aggregateSystemData);
    m_aggregationTimer->setSingleShot(false);
}

DataManager::~DataManager()
{
    stop();
}

void DataManager::initialize()
{
    if (m_isInitialized) return;

    try {
        // Create monitor intances
        m_cpuMonitor = std::make_unique<CPUMonitor>(this);
        m_memoryMonitor = std::make_unique<MemoryMonitor>(this);
        m_alertManager = std::make_unique<AlertManager>(this);

        // Connect signalsvvvvvv    if (m_isInitialized) return;

        try {
            // Create monitor intances
            m_cpuMonitor = std::make_unique<CPUMonitor>(this);
            m_memoryMonitor = std::make_unique<MemoryMonitor>(this);
            m_alertManager = std::make_unique<AlertManager>(this);

            // Connect signals
            connectMonitorSignals();

            // Set update intervals
            m_cpuMonitor->setUpdateInterval(m_updateInterval);
            m_memoryMonitor->setUpdateInterval(m_updateInterval);

            m_isInitialized = true;
            emit initializationComplete();
        } catch (const std::exception &e) {
            emit errorOccurred(QString("Initialization failed: %1").arg(e.what()));
        }    if (m_isInitialized) return;

        try {
            // Create monitor intances
            m_cpuMonitor = std::make_unique<CPUMonitor>(this);
            m_memoryMonitor = std::make_unique<MemoryMonitor>(this);
            m_alertManager = std::make_unique<AlertManager>(this);

            // Connect signals
            connectMonitorSignals();

            // Set update intervals
            m_cpuMonitor->setUpdateInterval(m_updateInterval);
            m_memoryMonitor->setUpdateInterval(m_updateInterval);

            m_isInitialized = true;
            emit initializationComplete();
        } catch (const std::exception &e) {
            emit errorOccurred(QString("Initialization failed: %1").arg(e.what()));
        }
        connectMonitorSignals();

        // Set update intervals
        m_cpuMonitor->setUpdateInterval(m_updateInterval);
        m_memoryMonitor->setUpdateInterval(m_updateInterval);

        m_isInitialized = true;
        emit initializationComplete();
    } catch (const std::exception &e) {
        emit errorOccurred(QString("Initialization failed: %1").arg(e.what()));
    }
}

void DataManager::start()
{
    if (!m_isInitialized) {
        initialize();
    }

    if (m_isRunning) return;

    try {
        // Start all monitors
        m_cpuMonitor->startMonitoring();
        m_memoryMonitor->startMonitoring();

        // Start aggreation timer
        m_aggregationTimer->start(m_updateInterval);

        m_isRunning = true;
        m_isPaused = false;

        emit monitoringStateChanged(true);
    } catch (const  std::exception &e) {
        emit errorOccurred(QString("Start failed: %1").arg(e.what()));
    }
}

void DataManager::stop()
{
    if (!m_isRunning) return;

    // Stop aggregation timer
    m_aggregationTimer->stop();

    // Stop all monitors
    if (m_cpuMonitor) m_cpuMonitor->stopMonitoring();
    if (m_memoryMonitor) m_memoryMonitor->stopMonitoring();

    m_isRunning = false;
    m_isPaused = false;
    emit monitoringStateChanged(false);
}

void DataManager::pause()
{
    if (!m_isRunning || m_isPaused) return;

    m_cpuMonitor->pauseMonitoring();
    m_memoryMonitor->pauseMonitoring();
    m_aggregationTimer->stop();

    m_isPaused = true;
    emit monitoringStateChanged(false);
}

void DataManager::resume()
{
    if (!m_isRunning || !m_isPaused) return;

    m_cpuMonitor->resumeMonitoring();
    m_memoryMonitor->resumeMonitoring();
    m_aggregationTimer->start(m_updateInterval);

    m_isPaused = false;
    emit monitoringStateChanged(true);
}

SystemOverview DataManager::getCurrentSystemData() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_currentOverview;
}

CPUData DataManager::getCurrentCPUData() const
{
    if (m_cpuMonitor) {
        return m_cpuMonitor->getCurrentData();
    }
    return CPUData();
}

MemoryData DataManager::getCurrentMemoryData() const
{
    if (m_memoryMonitor) {
        return m_memoryMonitor->getCurrentData();
    }
    return MemoryData();
}

void DataManager::setUpdateInterval(int intervalMs)
{
    m_updateInterval = qMax(100, intervalMs);

    if (m_cpuMonitor) {
        m_cpuMonitor->setUpdateInterval(m_updateInterval);
    }
    if (m_memoryMonitor) {
        m_memoryMonitor->setUpdateInterval(m_updateInterval);
    }

    if (m_isRunning) {
        m_aggregationTimer->setInterval(m_updateInterval);
    }
}

void DataManager::setGlobalPaused(bool paused)
{
    if (paused) {
        pause();
    }
    else {
        resume();
    }
}

void DataManager::onCPUDataUpdated(const CPUData &data)
{
    QMutexLocker locker(&m_dataMutex);
    m_currentOverview.cpu = data;
}

void DataManager::onMemoryDataUpdated(const MemoryData &data)
{
    QMutexLocker locker(&m_dataMutex);
    m_currentOverview.memory = data;
}

void DataManager::aggregateSystemData()
{
    updateSystemOverview();

    SystemOverview overview = getCurrentSystemData();
    if (overview.isValid()) {
        emit systemDataUpdated(overview);
    }
}

void DataManager::connectMonitorSignals()
{
    // Connect CPU monitor
    connect(m_cpuMonitor.get(),&CPUMonitor::cpuDataUpdated, this, &DataManager::onCPUDataUpdated);

    // Connect Memory monitor
    connect(m_memoryMonitor.get(), &MemoryMonitor::memoryDataUpdated, this, &DataManager::onMemoryDataUpdated);

    // Connect monitor to alert manager
    connect(m_cpuMonitor.get(), &CPUMonitor::cpuDataUpdated, m_alertManager.get(), &AlertManager::checkCPUThresholds);

    connect(m_memoryMonitor.get(), &MemoryMonitor::memoryDataUpdated, m_alertManager.get(), &AlertManager::checkMemoryThresholds);
}

void DataManager::updateSystemOverview()
{
    QMutexLocker locker(&m_dataMutex);
    m_currentOverview.timestamp = QDateTime::currentDateTime();
}


