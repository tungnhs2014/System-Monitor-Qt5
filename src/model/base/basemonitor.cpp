/**
 * @file basemonitor.cpp
 * @brief BaseMonitor implemention with Template Method pattern
 * @author TungNHS
 * @version 1.0.0
 */

#include "basemonitor.h"
#include <QDebug>

BaseMonitor::BaseMonitor(QObject *parent)
    : QObject (parent)
    , m_updateTimer(new QTimer(this))
    , m_isMonitoring(false)
    , m_isPaused(false)
    , m_updateInterval(UPDATE_INTERVAL)
{
    connect(m_updateTimer, &QTimer::timeout, this, &BaseMonitor::onTimerTick);
    m_updateTimer->setSingleShot(false);
}

void BaseMonitor::startMonitoring()
{
    if (m_isMonitoring) return;

    m_isMonitoring = true;
    m_isPaused = false;
    m_updateTimer->start(m_updateInterval);

    emit monitoringStarted();
}

void BaseMonitor::stopMonitoring()
{
    if (!m_isMonitoring) return;

    m_updateTimer->stop();
    m_isMonitoring = false;
    m_isPaused = false;

    emit monitoringStopped();
}

void BaseMonitor::pauseMonitoring()
{
    m_isPaused = true;
}

void BaseMonitor::resumeMonitoring()
{
    m_isPaused= false;
}

void BaseMonitor::setUpdateInterval(int intervalMs)
{
    m_updateInterval = qMax(100, intervalMs);
    if (m_isMonitoring) {
        m_updateTimer->setInterval(m_updateInterval);
    }
}

void BaseMonitor::updateTimestamp()
{
    m_lastUpdateTime = QDateTime::currentDateTime();
}

bool BaseMonitor::isDataState(int maxAgeMs) const
{
    return m_lastUpdateTime.msecsTo(QDateTime::currentDateTime()) > maxAgeMs;
}

void BaseMonitor::onTimerTick()
{
    if (m_isPaused) return;

    try {
        QMutexLocker locker(&m_dataMutex);

        // Templdate Method execution
        collectData();
        processData();
        validateData();
        emitSignal();

        updateTimestamp();
        emit dataUpdated();
    } catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

