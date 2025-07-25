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
enum class MertricStatus {
    Unknow = 0,     // Status not determined yet
    Normal,         // Operating within normal paramecters
    Warning,        // Approaching threshold limits
    Critical        // Exceeded critical thresholds
};

/**
 * @brief Dashboard card types for navigation
 */
enum class MertricStatus {
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
enum class MertricStatus {
    Info = 0,       // Informational alerts
    Warning,        // Warning level alerts
    Critical,       // Critical system alerts
    Emergency       // Emergency system alerts
};

/**
 * @brief CPU core information structure
 */

#endif // TYPES_H
