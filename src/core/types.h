/**
 * @file types.h
 * @brief Data structures and enums for Pi 3B+ System Monitor
 * @author TungNHS
 * @version 1.0.0
 */

#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QDateTime>
#include <QMetaType>
#include <QDebug>
#include <QVector>

/**
 * @brief System metric status level
 */
enum class MetricStatus {
    Unknown = 0,     // Status not determined yet
    Normal,         // Operating within normal paramecters
    Warning,        // Approaching threshold limits
    Critical        // Exceeded critical thresholds
};

/**
 * @brief Dashboard card types for navigation
 */
enum class CardType {
    CPU = 0,        // CPU monitoring card
    GPU,            // CPU monitoring card
    Memory,         // RAM monitoring card
    Storage,        // Storage/HDD monitoring card
    Network,        // Network monitoring card
    System          // System information card
};

/**
 * @brief Alert serity levels
 */
enum class AlertSeverity {
    Info = 0,       // Informational alerts
    Warning,        // Warning level alerts
    Critical,       // Critical system alerts
    Emergency       // Emergency system alerts
};

/**
 * @brief CPU core information structure
 */
struct CPUCoreData {
    int coreID;             // Core ID (0-3 for Pi 3B+)
    double usage;           // Usage percentage (0.0-100.0)
    double frequency;       // Current frequency in MHz
    double temperature;     // Core temperature in Celsius

    // Constructor
    CPUCoreData() : coreID(-1), usage(0.0), frequency(0.0), temperature(0.0) {}

    // Validation
    bool isValid() const {
        return coreID >= 0 && usage >= 0.0 && usage <= 100.0;
    }
};

/**
 * @brief Complete CPU monitoring data
 */
struct CPUData {
    double totalUsage;              // Overall CPU usage percentage
    double averageFrequency;        // Average frequency across all cores
    double temperature;             // CPU temperature (°C)
    int coreCount;                  // Number of CPU cores
    QString model;                  // CPU model name
    QVector<CPUCoreData> cores;     // Per-core data
    MetricStatus status;            // Current status
    QDateTime timestamp;            // Data collection time

    // Constructor
    CPUData() : totalUsage(0.0), averageFrequency(0.0), temperature(0.0),
        coreCount(0), status(MetricStatus::Unknown) {
        timestamp = QDateTime::currentDateTime();
    }

    // Validation
    bool isValid() const {
        return coreCount > 0 && totalUsage >= 0.0 && totalUsage <= 100.0;
    }
};


#endif // TYPES_H
