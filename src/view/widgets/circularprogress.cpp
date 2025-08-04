/**
 * @file circularprogress.cpp
 * @brief Circular progress widget implementation
 * @author TungNHS
 * @version 1.0.0
 */

#include "circularprogress.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QFontMetrics>
#include <QDebug>
#include <QtMath>

CircularProgress::CircularProgress(QWidget *parent)
    : QWidget(parent)
    , m_value(0.0)
    , m_targetValue(0.0)
    , m_color(ACCENT_BLUE)
    , m_diameter(CIRCULAR_PROGRESS_SIZE)
    , m_lineWidth(8)
    , m_showText(8)
    , m_animationEnabled(true)
    , m_animation(nullptr)
    , m_rectDirty(true)
{
    // Widget setup
    setMinimumSize(40, 40);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setFocusPolicy(Qt::StrongFocus);

    // Animation setup
    m_animation = new QPropertyAnimation(this, "value");
    m_animation->setDuration(ANIMATION_DURATION);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_animation, &QPropertyAnimation::valueChanged, this, &CircularProgress::onAnimationValueChanged);
}

void CircularProgress::setValue(double value)
{
    // Clamp value to valid range
    value = qBound(0.0, value, 100.0);

    if (qAbs(value - m_targetValue) < EPSILON) {
        return;
    }

    m_targetValue = value;

    if (m_animationEnabled && m_animation) {
        // Animate to new value
        m_animation->stop();
        m_animation->setStartValue(m_value);
        m_animation->setEndValue(value);
        m_animation->start();
    }
    else {
        // Set immediately
        setValueInstant(value);
    }
}

void CircularProgress::setValueInstant(double value)
{
    value = qBound(0.0, value, 100.0);
    if (qAbs(value - m_value) < EPSILON) {
        return;
    }

    m_value = value;
    update();
    emit colorChanged(m_color);
}

void CircularProgress::setColor(const QString &color)
{
    if (m_color == color) {
        return;
    }

    m_color = color;
    update();
    emit colorChanged(m_color);
}

void CircularProgress::setStatusColor(MetricStatus status)
{
    QString color;

    switch (status) {
        case MetricStatus::Normal:
            color = ACCENT_SUCCESS;
            break;
        case MetricStatus::Warning:
            color = ACCENT_WARNING;
            break;
        case MetricStatus::Critical:
            color = ACCENT_CRITICAL;
            break;
        case MetricStatus::Unknown:
        default:
            color = TEXT_SECONDARY;
            break;
    }

    setColor(color);
}

void CircularProgress::setDiameter(int diameter)
{
    diameter = qMax(40, diameter);      // Minimum size for touch

    if (m_diameter == diameter) {
        return;
    }

    m_diameter = diameter;
    m_rectDirty = true;

    // Update widget size
    setFixedSize(diameter, diameter);
    update();
}

void CircularProgress::setLineWidth(int width)
{
    width = qBound(MIN_LINE_WIDTH, width, MAX_LINE_WIDTH);

    if (m_lineWidth == width) {
        return;
    }

    m_lineWidth = width;
    m_rectDirty = true;
    update();
}

void CircularProgress::setShowText(bool show)
{
    if (m_showText == show) {
        return;
    }

    m_showText = show;
    update();
}

void CircularProgress::setCustomText(const QString &text)
{
    if (m_customText == text) {
        return;
    }

    m_customText = text;
    update();
}

void CircularProgress::setAnimationEnabled(bool enabled)
{
    m_animationEnabled = enabled;

    if (!enabled && m_animation && m_animation->state() == QAbstractAnimation::Running) {
        m_animation->stop();
        setValueInstant(m_targetValue);
    }
}

void CircularProgress::reset()
{
    setValue(0.0);
}

void CircularProgress::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Calculate drawing rectangle
    if (m_rectDirty) {
        int size = qMin(width(), height());
        int margin = m_lineWidth / 2 + 2;
        m_drawRect = QRect(margin, margin, size - 2 * margin, size - 2 * margin);
        m_rectDirty = false;
    }

    // Draw components
    drawBackground(painter, m_drawRect);
    drawProgress(painter, m_drawRect, m_value);

    if (m_showText) {
        drawText(painter, m_drawRect);
    }
}

void CircularProgress::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

void CircularProgress::resizeEvent(QResizeEvent *event)
{
    m_rectDirty = true;
    QWidget::resizeEvent(event);
}

QSize CircularProgress::sizeHint() const
{
    return QSize(m_diameter, m_diameter);
}

QSize CircularProgress::minimumSizeHint() const
{
    return QSize(40, 40);
}

void CircularProgress::onAnimationValueChanged(const QVariant &value)
{
    double newValue = value.toDouble();

    if (qAbs(newValue - m_value) >= EPSILON) {
        m_value = newValue;
        update();
        emit valueChanged(m_value);
    }
}

void CircularProgress::drawBackground(QPainter &painter, const QRect &rect)
{
    // Background circle
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(BG_CARD));
    painter.drawEllipse(rect);

    // Background ring
    QPen bgPen;
    bgPen.setColor(TEXT_MUTED);
    bgPen.setWidth(m_lineWidth);
    bgPen.setCapStyle(Qt::RoundCap);
    painter.setPen(bgPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(rect);
}

void CircularProgress::drawProgress(QPainter &painter, const QRect &rect, double value)
{
    if (value <= 0.0) {
        return;     // Nothing to draw
    }

    // Progress arc
    QPen progressPen;
    progressPen.setColor(m_color);
    progressPen.setWidth(m_lineWidth);
    progressPen.setCapStyle(Qt::RoundCap);
    painter.setPen(progressPen);
    painter.setBrush(Qt::NoBrush);

    // Calculate span angle (360 degress = 5760 sixtennths)
    int spanAngle = static_cast<int>((value / 100.0) * 3600 * 16);
    int startAngle = static_cast<int>(START_ANGLE * 16);

    painter.drawArc(rect, startAngle, spanAngle);
}

void CircularProgress::drawText(QPainter &painter, const QRect &rect)
{
    QString text;

    if (!m_customText.isEmpty()) {
        text = m_customText;
    }
    else {
        text = QString("%1%").arg(static_cast<int>(qRound(m_value)));
    }

    // Font setup
    QFont font = painter.font();
    font.setPixelSize(calculateFontSize());
    font.setWeight(QFont::Bold);
    painter.setFont(font);

    // Text color
    painter.setPen(QColor(TEXT_PRIMARY));

    // Draw text centered
    painter.drawText(rect, Qt::AlignCenter, text);

}

int CircularProgress::calculateFontSize() const
{
    // Font size relative to diameter
    int fontSize = m_diameter / 4;

    // Clamp to reasonable range for 320x240 display
    fontSize = qBound(8, fontSize, 24);

    return fontSize;
}

QString CircularProgress::getStatusColor() const
{
    // Auto-determine color based on value if using default color
    if (m_color == ACCENT_BLUE) {
        if (m_value >= 90.0) {
            return ACCENT_CRITICAL;
        } else if (m_value >= 75.0) {
            return ACCENT_WARNING;
        } else {
            return ACCENT_SUCCESS;
        }
    }

    return m_color;
}
