/**
 * @file systemutils.h
 * @brief System utilities for Linux /proc filesystem access
 * @author TungNHS
 * @version 1.0.0
 */

#ifndef SYSTEMUTILS_H
#define SYSTEMUTILS_H

#include <QString>
#include <QStringList>
#include <QDateTime>

/**
 * @brief System utilities for Linux /proc filesystem access
 *
 */
    class SystemUtils
{
public:
    // ===================================================================
    // FILE I/O OPERATIONS
    // ===================================================================

    /**
     * @brief Read entire file content as string
     * @param filePath Path to file (e.g., "/proc/stat")
     * @return File content as QString, empty if error
     */
    static QString readFile(const QString& filePath);

    /**
     * @brief Read file content as list of lines
     * @param filePath Path to file
     * @return QStringList of lines, empty if error
     */
    static QStringList readFileLines(const QString& filePath);

    /**
     * @brief Check if file exists and is readable
     * @param filePath Path to file
     * @return true if file exists and readable
     */
    static bool fileExists(const QString& filePath);

    // ===================================================================
    // DATA PARSING UTILITIES
    // ===================================================================

    /**
     * @brief Parse string to double with error handling
     * @param str String to parse
     * @param ok Optional pointer to bool indicating success
     * @return Parsed double value, 0.0 if error
     */
    static double parseDouble(const QString& str, bool* ok = nullptr);

    /**
     * @brief Parse string to 64-bit integer
     * @param str String to parse
     * @param ok Optional pointer to bool indicating success
     * @return Parsed qint64 value, 0 if error
     */
    static qint64 parseInt64(const QString& str, bool* ok = nullptr);

    // ===================================================================
    // SYSTEM INFORMATION
    // ===================================================================

    /**
     * @brief Get system hostname
     * @return System hostname
     */
    static QString getHostname();

    /**
     * @brief Get Linux kernel version
     * @return Kernel version string
     */
    static QString getKernelVersion();

    /**
     * @brief Get system uptime in human readable format
     * @return Uptime string (e.g., "2d 3h 45m")
     */
    static QString getUptime();

    /**
     * @brief Get system boot time
     * @return QDateTime of system boot
     */
    static QDateTime getBootTime();

    // ===================================================================
    // CPU INFORMATION (Pi 3B+ ARM Cortex-A53)
    // ===================================================================

    /**
     * @brief Get number of CPU cores
     * @return Number of CPU cores (4 for Pi 3B+)
     */
    static int getCPUCoreCount();

    /**
     * @brief Get CPU model name
     * @return CPU model string
     */
    static QString getCPUModel();

    /**
     * @brief Get current CPU frequency in MHz
     * @return CPU frequency in MHz
     */
    static double getCPUFrequency();

    /**
     * @brief Get CPU temperature in Celsius
     * @return CPU temperature in °C
     */
    static double getCPUTemperature();

    // ===================================================================
    // MEMORY INFORMATION (Pi 3B+ 1GB RAM)
    // ===================================================================

    /**
     * @brief Get total system memory in bytes
     * @return Total memory in bytes
     */
    static qint64 getTotalMemory();

    /**
     * @brief Get available memory in bytes
     * @return Available memory in bytes
     */
    static qint64 getAvailableMemory();

    /**
     * @brief Get free memory in bytes
     * @return Free memory in bytes
     */
    static qint64 getFreeMemory();

    /**
     * @brief Get buffer memory in bytes
     * @return Buffer memory in bytes
     */
    static qint64 getBufferMemory();

    /**
     * @brief Get cache memory in bytes
     * @return Cache memory in bytes
     */
    static qint64 getCacheMemory();

    // ===================================================================
    // NETWORK INFORMATION
    // ===================================================================

    /**
     * @brief Get list of network interfaces
     * @return QStringList of interface names
     */
    static QStringList getNetworkInterfaces();

    /**
     * @brief Get primary active network interface
     * @return Active interface name (eth0, wlan0, etc.)
     */
    static QString getActiveNetworkInterface();

    // ===================================================================
    // STORAGE INFORMATION
    // ===================================================================

    /**
     * @brief Get total storage space for path
     * @param path Filesystem path (e.g., "/")
     * @return Total space in bytes
     */
    static qint64 getStorageTotal(const QString& path);

    /**
     * @brief Get used storage space for path
     * @param path Filesystem path
     * @return Used space in bytes
     */
    static qint64 getStorageUsed(const QString& path);

    /**
     * @brief Get available storage space for path
     * @param path Filesystem path
     * @return Available space in bytes
     */
    static qint64 getStorageAvailable(const QString& path);

    // ===================================================================
    // FORMAT UTILITIES
    // ===================================================================

    /**
     * @brief Format bytes to human readable string
     * @param bytes Number of bytes
     * @return Formatted string (e.g., "1.5 GB", "256 MB")
     */
    static QString formatBytes(qint64 bytes);

    /**
     * @brief Format uptime seconds to readable string
     * @param seconds Uptime in seconds
     * @return Formatted string (e.g., "2d 3h 45m 30s")
     */
    static QString formatUptime(qint64 seconds);

    /**
     * @brief Format percentage with precision
     * @param percentage Percentage value (0.0-100.0)
     * @return Formatted string (e.g., "75.5%")
     */
    static QString formatPercentage(double percentage);

    /**
     * @brief Format temperature with unit
     * @param celsius Temperature in Celsius
     * @return Formatted string (e.g., "65.2°C")
     */
    static QString formatTemperature(double celsius);

    // ===================================================================
    // VALIDATION UTILITIES
    // ===================================================================

    /**
     * @brief Validate percentage range
     * @param value Percentage value to check
     * @return true if value is 0.0-100.0
     */
    static bool isValidPercentage(double value);

    /**
     * @brief Validate temperature range
     * @param celsius Temperature to check
     * @return true if temperature is reasonable (-40 to 150°C)
     */
    static bool isValidTemperature(double celsius);

    // ===================================================================
    // PERFORMANCE UTILITIES
    // ===================================================================

    /**
     * @brief Calculate CPU usage percentage between two measurements
     * @param totalTime Current total CPU time
     * @param idleTime Current idle CPU time
     * @param lastTotalTime Previous total CPU time
     * @param lastIdleTime Previous idle CPU time
     * @return CPU usage percentage
     */
    static double calculateCPUUsage(qint64 totalTime, qint64 idleTime,
                                    qint64 lastTotalTime, qint64 lastIdleTime);

private:
    SystemUtils() = delete; // Static class only

    // ===================================================================
    // INTERNAL HELPER METHODS
    // ===================================================================

    /**
     * @brief Extract value from /proc file by key
     * @param filePath Path to /proc file
     * @param key Key to search for
     * @return Value string, empty if not found
     */
    static QString extractValueFromProcFile(const QString& filePath,
                                            const QString& key);

    /**
     * @brief Parse memory line from /proc/meminfo
     * @param line Line from /proc/meminfo
     * @return Memory value in bytes
     */
    static qint64 parseMemoryLine(const QString& line);
};

#endif // SYSTEMUTILS_H
