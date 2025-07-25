/**
 * @file constants.h
 * @brief System constants for Raspberry Pi 3B+ System Monitor
 * @author TungNHS
 * @version 1.0.0
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QStringList>

// ===================================================================
// APPLICATION INFORMATION
// ===================================================================
const QString APP_NAME = "System Monitor";
const QString APP_VERSION = "1.0.0";
const QString APP_ORGANIZATION = "EmbeddedSystems";

// ===================================================================
// UPDATE INTERVALS (milliseconds)
// ===================================================================
const int UPDATE_INTERVAL = 1000;              // 1s - CPU, RAM, Temperature
const int FAST_UPDATE_INTERVAL = 500;          // 0.5s - UI animations only
const int SLOW_UPDATE_INTERVAL = 5000;         // 5s - Storage, System info
const int NETWORK_UPDATE_INTERVAL = 2000;      // 2s - Network stats
const int ALERT_CHECK_INTERVAL = 3000;         // 3s - Alert checking
const int ALERT_CLEANUP_INTERVAL = 300000;     // 5 minutes - Alert cleanup

// ===================================================================
// MEMORY CONSTRAINTS (Pi 3B+ - 1GB RAM)
// ===================================================================
const int MAX_HISTORY_SIZE = 120;              // 2 minutes at 1Hz (120 points)
const int MAX_ALERTS_HISTORY = 200;            // Maximum stored alerts
const int MAX_APPLICATION_MEMORY_MB = 50;      // <50MB total app usage

// ===================================================================
// PERFORMANCE THRESHOLDS (Pi 3B+ Quad-core ARM Cortex-A53)
// ===================================================================
const double CPU_WARNING_THRESHOLD = 75.0;     // 75% CPU usage warning
const double CPU_CRITICAL_THRESHOLD = 90.0;    // 90% CPU usage critical
const double RAM_WARNING_THRESHOLD = 80.0;     // 80% RAM usage warning
const double RAM_CRITICAL_THRESHOLD = 95.0;    // 95% RAM usage critical
const double TEMP_WARNING_THRESHOLD = 70.0;    // 70°C temperature warning
const double TEMP_CRITICAL_THRESHOLD = 80.0;   // 80°C temperature critical
const double STORAGE_WARNING_THRESHOLD = 85.0; // 85% storage warning
const double STORAGE_CRITICAL_THRESHOLD = 95.0;// 95% storage critical
const double NETWORK_WARNING_THRESHOLD = 50.0; // 50 MB/s network warning

// ===================================================================
// UI DIMENSIONS (ILI9341 320x240 Display)
// ===================================================================
const int WINDOW_WIDTH = 320;                  // Target display width
const int WINDOW_HEIGHT = 240;                 // Target display height
const int MIN_WINDOW_WIDTH = 280;              // Minimum window width
const int MIN_WINDOW_HEIGHT = 200;             // Minimum window height
const int CARD_MIN_WIDTH = 100;                // Minimum card width
const int CARD_MIN_HEIGHT = 75;                // Minimum card height
const int CIRCULAR_PROGRESS_SIZE = 60;         // Circular progress diameter

// ===================================================================
// LINUX SYSTEM PATHS (Pi 3B+ Specific)
// ===================================================================
const QString PROC_STAT = "/proc/stat";
const QString PROC_MEMINFO = "/proc/meminfo";
const QString PROC_CPUINFO = "/proc/cpuinfo";
const QString PROC_VERSION = "/proc/version";
const QString PROC_NET_DEV = "/proc/net/dev";
const QString PROC_MOUNTS = "/proc/mounts";
const QString PROC_UPTIME = "/proc/uptime";
const QString PROC_LOADAVG = "/proc/loadavg";
const QString THERMAL_ZONE_PATH = "/sys/class/thermal/thermal_zone0/temp";
const QString CPUFREQ_PATH = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq";

// ===================================================================
// COLOR SCHEME (Professional Dark Theme)
// ===================================================================
const QString BG_MAIN = "#1a1d23";             // Main background
const QString BG_CARD = "#2d3142";             // Card background
const QString BG_HOVER = "#3a3f52";            // Hover state
const QString TEXT_PRIMARY = "#ffffff";        // Primary text
const QString TEXT_SECONDARY = "#a8b2d1";      // Secondary text
const QString TEXT_MUTED = "#6c7293";          // Muted text
const QString ACCENT_BLUE = "#4f5b93";         // Primary accent
const QString ACCENT_SUCCESS = "#27ae60";      // Success/normal
const QString ACCENT_WARNING = "#f39c12";      // Warning state
const QString ACCENT_CRITICAL = "#e74c3c";     // Critical/error

// Metric-specific colors
const QString CPU_COLOR = "#9d4edd";           // CPU metrics (purple)
const QString GPU_COLOR = "#06ffa5";           // GPU metrics (cyan)
const QString RAM_COLOR = "#3498db";           // RAM metrics (blue)
const QString STORAGE_COLOR = "#f39c12";       // Storage metrics (orange)
const QString NETWORK_UP_COLOR = "#27ae60";    // Upload (green)
const QString NETWORK_DOWN_COLOR = "#3498db";  // Download (blue)

// ===================================================================
// ALERT TYPES
// ===================================================================
const QString ALERT_INFO = "INFO";
const QString ALERT_WARNING = "WARNING";
const QString ALERT_CRITICAL = "CRITICAL";

// ===================================================================
// NETWORK INTERFACES (Common Linux names)
// ===================================================================
const QStringList NETWORK_INTERFACES = {
    "eth0", "wlan0", "enp0s3", "wlp2s0", "ens33", "ens32"
};

// ===================================================================
// STORAGE PATHS TO MONITOR
// ===================================================================
const QStringList STORAGE_PATHS = {
    "/", "/home", "/var", "/tmp"
};

// ===================================================================
// SETTINGS KEYS
// ===================================================================
const QString SETTINGS_UPDATE_INTERVAL = "update_interval";
const QString SETTINGS_ALERTS_ENABLED = "alerts_enabled";
const QString SETTINGS_WINDOW_GEOMETRY = "window_geometry";
const QString SETTINGS_WINDOW_STATE = "window_state";
const QString SETTINGS_CPU_WARNING = "cpu_warning_threshold";
const QString SETTINGS_CPU_CRITICAL = "cpu_critical_threshold";
const QString SETTINGS_RAM_WARNING = "ram_warning_threshold";
const QString SETTINGS_RAM_CRITICAL = "ram_critical_threshold";

// ===================================================================
// BYTE CONVERSION
// ===================================================================
const qint64 BYTES_PER_KB = 1024;
const qint64 BYTES_PER_MB = 1024 * 1024;
const qint64 BYTES_PER_GB = 1024 * 1024 * 1024;

// ===================================================================
// ANIMATION & UI
// ===================================================================
const int ANIMATION_DURATION = 300;            // 300ms animations
const int HOVER_ANIMATION_DURATION = 150;      // 150ms hover effects
const double EPSILON = 0.001;                  // Float comparison tolerance

#endif // CONSTANTS_H
