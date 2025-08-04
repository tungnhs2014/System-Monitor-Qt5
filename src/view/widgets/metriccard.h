/**
 * @file metriccard.h
 * @brief Metric card widget for dashboard display
 * @author TungNHS
 * @version 1.0.0
 */

#ifndef METRICCARD_H
#define METRICCARD_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include "circularprogress.h"
#include "core/constants.h"
#include "core/types.h"

/**
 * @brief Dashboard metric card widget optimized for 2x3 grid layout
 *
 * Based on mockup designs:
 * - CPU MOD: 57%, TEMP 84°, CLOCK 4.8G
 * - GPU MOD: 54%, TEMP 45°, MEM 256M
 * - RAM: 73%, USED 410M/485M, FREE 75M
 * - HDD: 95%, linear progress, TEMP 45°
 * - Network: Download/Upload speeds
 * - DateTime: Current date and time
 */

class MetricCard : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double progress READ progress WRITE setProgress NOTIFY progressChanged)
public:
    explicit MetricCard(QWidget *parent = nullptr);
    explicit MetricCard(const QString& title, CardType type, QWidget* parent = nullptr);

    // ===================================================================
    // PUBLIC INTERFACE
    // ===================================================================

    /**
     * @brief Get current progress value
     * @return Progress percentage (0.0-100.0)
     */
    double progress() const { return m_progress; }

    /**
     * @brief Set progress value with animation
     * @param progress Progress percentage (0.0-100.0)
     */
    void setProgress(double progress);

    /**
     * @brief Set card title
     * @param title Card title ("CPU MOD", "RAM", "HDD", etc.)
     */
    void setTitle(const QString& title);

    /**
     * @brief Set primary value display
     * @param value Main value ("57%", "410M/485M", "95%")
     */
    void setPrimaryValue(const QString& value);

    /**
     * @brief Set secondary information lines
     * @param line1 First info line ("TEMP 84°", "USED 410M")
     * @param line2 Second info line ("CLOCK 4.8G", "FREE 75M")
     */
    void setSecondaryInfo(const QString& line1, const QString& line2 = QString());

    /**
     * @brief Set metric status for color coding
     * @param status Current metric status
     */
    void setStatus(MetricStatus status);

    /**
     * @brief Set card type for navigation and styling
     * @param type Card type (CPU, Memory, Storage, etc.)
     */
    void setCardType(CardType type);

    /**
     * @brief Get current card type
     * @return Current card type
     */
    CardType cardType() const { return m_cardType; }

    /**
     * @brief Update CPU data
     * @param data CPU monitoring data
     */
    void updateCPUData(const CPUData& data);

    /**
     * @brief Update memory data
     * @param data Memory monitoring data
     */
    void updateMemoryData(const MemoryData& data);

    /**
     * @brief Update GPU data
     * @param data GPU monitoring data
     */
    void updateGPUData(const GPUData& data);

    /**
     * @brief Update storage data
     * @param data Storage monitoring data
     */
    void updateStorageData(const StorageData& data);

    /**
     * @brief Update network data
     * @param data Network monitoring data
     */
    void updateNetworkData(const NetworkData& data);

    /**
     * @brief Update system data (for DateTime card)
     * @param data System monitoring data
     */
    void updateSystemData(const SystemData& data);

    /**
     * @brief Set hover effect enabled
     * @param enabled True to enable hover animations
     */
    void setHoverEnabled(bool enabled);

    /**
     * @brief Set card enabled/disabled state
     * @param enabled True if card is interactive
     */
    void setEnabled(bool enabled);

public slots:
    /**
     * @brief Trigger card click animation
     */
    void animateClick();

signals:
    /**
     * @brief Emitted when progress changes
     * @param progress New progress value
     */
    void progressChanged(double progress);

    /**
     * @brief Emitted when card is clicked
     * @param type Card type for navigation
     */
    void cardClicked(CardType type);

    /**
     * @brief Emitted when card is double-clicked
     * @param type Card type for navigation
     */
    void cardDoubleClicked(CardType type);

protected:
    // ===================================================================
    // QT WIDGET OVERRIDES
    // ===================================================================

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private slots:
    /**
     * @brief Handle circular progress click
     */
    void onProgressClicked();

    /**
     * @brief Handle hover animation finished
     */
    void onHoverAnimationFinished();

private:
    // ===================================================================
    // INITIALIZATION METHODS
    // ===================================================================

    /**
     * @brief Initialize widget UI
     */
    void initializeUI();

    /**
     * @brief Setup widget layout
     */
    void setupLayout();

    /**
     * @brief Setup animations
     */
    void setupAnimations();

    /**
     * @brief Setup card styling
     */
    void setupStyling();

    // ===================================================================
    // UTILITY METHODS
    // ===================================================================

    /**
     * @brief Get card color based on type
     * @return Color string for card type
     */
    QString getCardColor() const;

    /**
     * @brief Get card background color based on status
     * @return Background color string
     */
    QString getBackgroundColor() const;

    /**
     * @brief Update progress display type (circular/linear)
     */
    void updateProgressType();

    /**
     * @brief Format memory value for display
     * @param bytes Memory value in bytes
     * @return Formatted string (e.g., "410M", "1.2G")
     */
    QString formatMemoryValue(qint64 bytes) const;

    /**
     * @brief Format percentage for display
     * @param percentage Percentage value
     * @return Formatted string (e.g., "57%", "100%")
     */
    QString formatPercentage(double percentage) const;

    /**
     * @brief Format temperature for display
     * @param celsius Temperature in Celsius
     * @return Formatted string (e.g., "84°", "45°")
     */
    QString formatTemperature(double celsius) const;

private:
    // ===================================================================
    // MEMBER VARIABLES
    // ===================================================================

    // Card properties
    CardType m_cardType;                // Card type for navigation
    MetricStatus m_status;              // Current metric status
    double m_progress;                  // Current progress value
    QString m_title;                    // Card title
    bool m_hoverEnabled;                // Hover effects enabled
    bool m_isPressed;                   // Mouse press state

    // UI Components
    QVBoxLayout* m_mainLayout;          // Main card layout
    QHBoxLayout* m_headerLayout;        // Title and progress layout
    QVBoxLayout* m_infoLayout;          // Secondary info layout

    QLabel* m_titleLabel;                   // Card title label
    CircularProgress* m_circularProgress;   // Circular progress widget
    QLabel* m_primaryLabel;                 // Primary value label
    QLabel* m_secondaryLabel1;              // First secondary info label
    QLabel* m_secondaryLabel2;              // Second secondary info label

    // Special layouts for different card types
    QWidget* m_networkWidget;           // Network speed display
    QWidget* m_dateTimeWidget;          // Date/time display
    QLabel* m_downloadLabel;            // Download speed label
    QLabel* m_uploadLabel;              // Upload speed label
    QLabel* m_dateLabel;                // Date label
    QLabel* m_timeLabel;                // Time label

    // Effects and animations
    QGraphicsDropShadowEffect* m_shadowEffect; // Card shadow effect
    QPropertyAnimation* m_hoverAnimation;       // Hover scale animation
    QPropertyAnimation* m_clickAnimation;       // Click animation

    // Constants
    static constexpr int CARD_MARGIN = 8;           // Card margin
    static constexpr int CARD_PADDING = 12;         // Internal padding
    static constexpr int PROGRESS_SIZE = 50;        // Progress widget size
    static constexpr double HOVER_SCALE = 1.05;     // Hover scale factor
    static constexpr int ANIMATION_DURATION = 200;  // Animation duration
};

#endif // METRICCARD_H
