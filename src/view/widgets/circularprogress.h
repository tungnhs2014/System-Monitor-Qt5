/**
 * @file circularprogress.h
 * @brief Circular progress widget for Pi 3B+ System Monitor
 * @author TungNHS
 * @version 1.0.0
 */

#ifndef CIRCULARPROGRESS_H
#define CIRCULARPROGRESS_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QPropertyAnimation>
#include "core/constants.h"
#include "core/types.h"

/**
 * @brief Custom circular progress widget optimized for 320x240 display
 *
 * Features:
 * - Smooth animations for value changes
 * - Color coding based on metric status
 * - Configurable size (60px for cards, 120px for details)
 * - Percentage text display in center
 * - Touch-friendly for ILI9341 touchscreen
 */

class CircularProgress : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString color READ color WRITE setColor NOTIFY colorChanged)

public:
    explicit CircularProgress(QWidget *parent = nullptr);

    // ===================================================================
    // PUBLIC INTERFACE
    // ===================================================================

    /**
     * @brief Get current progress value
     * @return Progress value (0.0-100.0)
     */
    double value() const { return m_value; }

    /**
     * @brief Set progress value with animation
     * @param value Progress percentage (0.0-100.0)
     */
    void setValue(double value);

    /**
     * @brief Set progress value immediately (no animation)
     * @param value Progress percentage (0.0-100.0)
     */
    void setValueInstant(double value);

    /**
     * @brief Get current progress color
     * @return Color string (hex format)
     */
    QString color() const { return m_color; }

    /**
     * @brief Set progress ring color
     * @param color Color string (hex format, e.g., "#9d4edd")
     */
    void setColor(const QString& color);

    /**
     * @brief Set progress color based on metric status
     * @param status Metric status (Normal/Warning/Critical)
     */
    void setStatusColor(MetricStatus status);

    /**
     * @brief Set widget size (diameter)
     * @param diameter Circle diameter in pixels
     */
    void setDiameter(int diameter);

    /**
     * @brief Get current diameter
     * @return Circle diameter in pixels
     */
    int diameter() const { return m_diameter; }

    /**
     * @brief Set line width for progress ring
     * @param width Ring thickness in pixels
     */
    void setLineWidth(int width);

    /**
     * @brief Enable/disable percentage text display
     * @param show True to show percentage text
     */
    void setShowText(bool show);

    /**
     * @brief Set custom text instead of percentage
     * @param text Custom text to display
     */
    void setCustomText(const QString& text);

    /**
     * @brief Enable/disable smooth animations
     * @param enabled True to enable animations
     */
    void setAnimationEnabled(bool enabled);

public slots:
    /**
     * @brief Reset progress to 0%
     */
    void reset();

signals:
    /**
     * @brief Emitted when progress value changes
     * @param value New progress value
     */
    void valueChanged(double value);

    /**
     * @brief Emitted when color changes
     * @param color New color string
     */
    void colorChanged(const QString& color);

    /**
     * @brief Emitted when widget is clicked
     */
    void clicked();

protected:
    // ===================================================================
    // QT WIDGET OVERRIDES
    // ===================================================================

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private slots:
    /**
     * @brief Animation frame update
     * @param value Animated value
     */
    void onAnimationValueChanged(const QVariant& value);

private:
    // ===================================================================
    // DRAWING METHODS
    // ===================================================================

    /**
     * @brief Draw background circle
     * @param painter QPainter instance
     * @param rect Drawing rectangle
     */
    void drawBackground(QPainter& painter, const QRect& rect);

    /**
     * @brief Draw progress arc
     * @param painter QPainter instance
     * @param rect Drawing rectangle
     * @param value Progress value (0.0-100.0)
     */
    void drawProgress(QPainter& painter, const QRect& rect, double value);

    /**
     * @brief Draw center text
     * @param painter QPainter instance
     * @param rect Drawing rectangle
     */
    void drawText(QPainter& painter, const QRect& rect);

    /**
     * @brief Calculate optimal font size for current diameter
     * @return Font size in pixels
     */
    int calculateFontSize() const;

    /**
     * @brief Get status color based on current value and thresholds
     * @return Color string for current status
     */
    QString getStatusColor() const;

private:
    // ===================================================================
    // MEMBER VARIABLES
    // ===================================================================

    // Progress data
    double m_value;                 // Current progress value (0.0-100.0)
    double m_targetValue;           // Target value for animation
    QString m_color;                // Progress ring color
    QString m_customText;           // Custom text instead of percentage

    // Widget properties
    int m_diameter;                 // Circle diameter in pixels
    int m_lineWidth;                // Ring thickness in pixels
    bool m_showText;                // Show percentage text
    bool m_animationEnabled;        // Enable smooth animations

    // Animation
    QPropertyAnimation* m_animation; // Value change animation

    // Drawing optimization
    mutable QRect m_drawRect;       // Cached drawing rectangle
    mutable bool m_rectDirty;       // Rectangle needs recalculation

    // Constants for drawing
    static constexpr double START_ANGLE = -90.0;  // Start angle (top)
    static constexpr int ANIMATION_DURATION = 300; // Animation duration (ms)
    static constexpr int MIN_LINE_WIDTH = 3;       // Minimum ring thickness
    static constexpr int MAX_LINE_WIDTH = 15;      // Maximum ring thickness
};

#endif // CIRCULARPROGRESS_H
