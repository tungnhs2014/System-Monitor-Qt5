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

// ===================================================================
// MEMORY DATA STRUCTURES
// ===================================================================

/**
 * @brief Memory monitoring data structure (Pi 3B+ has 1GB RAM)
 */
struct MemoryData {
    qint64 totalRAM;            ///< Total RAM in bytes
    qint64 usedRAM;             ///< Used RAM in bytes
    qint64 freeRAM;             ///< Free RAM in bytes
    qint64 availableRAM;        ///< Available RAM in bytes
    qint64 buffers;             ///< Buffer memory in bytes
    qint64 cached;              ///< Cache memory in bytes
    qint64 swapTotal;           ///< Total swap in bytes
    qint64 swapUsed;            ///< Used swap in bytes
    double usagePercentage;     ///< RAM usage percentage
    double swapPercentage;      ///< Swap usage percentage
    MetricStatus status;        ///< Current status
    QDateTime timestamp;        ///< Data collection time

    // Constructor
    MemoryData() : totalRAM(0), usedRAM(0), freeRAM(0), availableRAM(0),
        buffers(0), cached(0), swapTotal(0), swapUsed(0),
        usagePercentage(0.0), swapPercentage(0.0),
        status(MetricStatus::Unknown) {
        timestamp = QDateTime::currentDateTime();
    }

    // Validation
    bool isValid() const {
        return totalRAM > 0 && usagePercentage >= 0.0 && usagePercentage <= 100.0;
    }
};

// ===================================================================
// GPU DATA STRUCTURES (VideoCore IV on Pi 3B+)
// ===================================================================

/**
 * @brief GPU monitoring data (VideoCore IV on Pi 3B+)
 */
struct GPUData {
    double usage;               ///< GPU usage percentage
    double temperature;         ///< GPU temperature (°C)
    qint64 memoryUsed;         ///< GPU memory used in bytes
    qint64 memoryTotal;        ///< Total GPU memory in bytes
    double memoryPercentage;   ///< GPU memory usage percentage
    double frequency;          ///< GPU frequency in MHz
    MetricStatus status;       ///< Current status
    QDateTime timestamp;       ///< Data collection time

    // Constructor
    GPUData() : usage(0.0), temperature(0.0), memoryUsed(0), memoryTotal(0),
        memoryPercentage(0.0), frequency(0.0), status(MetricStatus::Unknown) {
        timestamp = QDateTime::currentDateTime();
    }

    // Validation
    bool isValid() const {
        return memoryTotal > 0 && usage >= 0.0 && usage <= 100.0;
    }
};

// ===================================================================
// NETWORK DATA STRUCTURES
// ===================================================================

/**
 * @brief Network interface data
 */
struct NetworkInterfaceData {
    QString name;               ///< Interface name (eth0, wlan0, etc.)
    QString ipAddress;          ///< IP address
    qint64 bytesReceived;      ///< Total bytes received
    qint64 bytesSent;          ///< Total bytes sent
    double downloadSpeed;      ///< Current download speed (bytes/sec)
    double uploadSpeed;        ///< Current upload speed (bytes/sec)
    bool isActive;             ///< Interface is active
    QDateTime timestamp;       ///< Data collection time

    // Constructor
    NetworkInterfaceData() : bytesReceived(0), bytesSent(0),
        downloadSpeed(0.0), uploadSpeed(0.0), isActive(false) {
        timestamp = QDateTime::currentDateTime();
    }

    // Validation
    bool isValid() const {
        return !name.isEmpty() && bytesReceived >= 0 && bytesSent >= 0;
    }
};

/**
 * @brief Complete network monitoring data
 */
struct NetworkData {
    QVector<NetworkInterfaceData> interfaces;   ///< All network interfaces
    QString activeInterface;                    ///< Primary active interface
    double totalDownloadSpeed;                  ///< Total download speed
    double totalUploadSpeed;                    ///< Total upload speed
    MetricStatus status;                        ///< Current status
    QDateTime timestamp;                        ///< Data collection time

    // Constructor
    NetworkData() : totalDownloadSpeed(0.0), totalUploadSpeed(0.0),
        status(MetricStatus::Unknown) {
        timestamp = QDateTime::currentDateTime();
    }

    // Validation
    bool isValid() const {
        return !interfaces.isEmpty();
    }
};

// ===================================================================
// STORAGE DATA STRUCTURES
// ===================================================================

/**
 * @brief Storage device information
 */
struct StorageDeviceData {
    QString path;               ///< Mount path (/, /home, etc.)
    QString filesystem;         ///< Filesystem type (ext4, vfat, etc.)
    qint64 totalSpace;         ///< Total space in bytes
    qint64 usedSpace;          ///< Used space in bytes
    qint64 availableSpace;     ///< Available space in bytes
    double usagePercentage;    ///< Usage percentage
    double temperature;        ///< Storage temperature (if available)
    MetricStatus status;       ///< Current status
    QDateTime timestamp;       ///< Data collection time

    // Constructor
    StorageDeviceData() : totalSpace(0), usedSpace(0), availableSpace(0),
        usagePercentage(0.0), temperature(0.0),
        status(MetricStatus::Unknown) {
        timestamp = QDateTime::currentDateTime();
    }

    // Validation
    bool isValid() const {
        return !path.isEmpty() && totalSpace > 0 &&
               usagePercentage >= 0.0 && usagePercentage <= 100.0;
    }
};

/**
 * @brief Complete storage monitoring data
 */
struct StorageData {
    QVector<StorageDeviceData> devices;         ///< All storage devices
    double totalUsagePercentage;               ///< Overall usage percentage
    MetricStatus status;                       ///< Current status
    QDateTime timestamp;                       ///< Data collection time

    // Constructor
    StorageData() : totalUsagePercentage(0.0), status(MetricStatus::Unknown) {
        timestamp = QDateTime::currentDateTime();
    }

    // Validation
    bool isValid() const {
        return !devices.isEmpty();
    }
};

// ===================================================================
// SYSTEM DATA STRUCTURES
// ===================================================================

/**
 * @brief System information data
 */
struct SystemData {
    QString hostname;           ///< System hostname
    QString kernelVersion;      ///< Linux kernel version
    QString architecture;       ///< System architecture (armv7l for Pi 3B+)
    qint64 uptime;             ///< System uptime in seconds
    double loadAverage1min;    ///< 1-minute load average
    double loadAverage5min;    ///< 5-minute load average
    double loadAverage15min;   ///< 15-minute load average
    int processCount;          ///< Total number of processes
    QDateTime bootTime;        ///< System boot time
    QDateTime timestamp;       ///< Data collection time

    // Constructor
    SystemData() : uptime(0), loadAverage1min(0.0), loadAverage5min(0.0),
        loadAverage15min(0.0), processCount(0) {
        timestamp = QDateTime::currentDateTime();
    }

    // Validation
    bool isValid() const {
        return !hostname.isEmpty() && uptime > 0;
    }
};

// ===================================================================
// ALERT DATA STRUCTURES
// ===================================================================

/**
 * @brief Alert information
 */
struct AlertData {
    AlertSeverity severity;     ///< Alert severity level
    QString title;              ///< Alert title
    QString message;            ///< Alert message
    QString source;             ///< Alert source (CPU, RAM, etc.)
    QDateTime timestamp;        ///< When alert was created
    bool acknowledged;          ///< Whether alert was acknowledged

    // Constructor
    AlertData() : severity(AlertSeverity::Info), acknowledged(false) {
        timestamp = QDateTime::currentDateTime();
    }

    // Validation
    bool isValid() const {
        return !title.isEmpty() && !message.isEmpty();
    }
};

// ===================================================================
// REGISTER METATYPES (for Qt signals/slots)
// ===================================================================

// Register custom types for Qt's meta-object system
Q_DECLARE_METATYPE(MetricStatus)
Q_DECLARE_METATYPE(CardType)
Q_DECLARE_METATYPE(AlertSeverity)
Q_DECLARE_METATYPE(CPUData)
Q_DECLARE_METATYPE(MemoryData)
Q_DECLARE_METATYPE(GPUData)
Q_DECLARE_METATYPE(NetworkData)
Q_DECLARE_METATYPE(StorageData)
Q_DECLARE_METATYPE(SystemData)
Q_DECLARE_METATYPE(AlertData)

#endif // TYPES_H
