/**
 * @file metriccard.cpp
 * @brief Metric card widget implementation
 * @author TungNHS
 * @version 1.0.0
 */

#include "metriccard.h"
#include "core/systemutils.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QDateTime>
#include <QDebug>

MetricCard::MetricCard(QWidget *parent)
    : QWidget(parent)
    , m_cardType(CardType::CPU)
    , m_status(MetricStatus::Normal)
    , m_progress(0.0)
    , m_title("Metric")
    , m_hoverEnabled(true)
    , m_isPressed(false)
    , m_mainLayout(nullptr)
    , m_headerLayout(nullptr)
    , m_infoLayout(nullptr)
    , m_titleLabel(nullptr)
    , m_circularProgress(nullptr)
    , m_primaryLabel(nullptr)
    , m_secondaryLabel1(nullptr)
    , m_secondaryLabel2(nullptr)
    , m_networkWidget(nullptr)
    , m_dateTimeWidget(nullptr)
    , m_downloadLabel(nullptr)
    , m_uploadLabel(nullptr)
    , m_dateLabel(nullptr)
    , m_timeLabel(nullptr)
    , m_shadowEffect(nullptr)
    , m_hoverAnimation(nullptr)
    , m_clickAnimation(nullptr)
{
    initializeUI();
}

MetricCard::MetricCard(const QString &title, CardType type, QWidget *parent)
{
    setTitle(title);
    setCardType(type);
}

void MetricCard::setProgress(double progress)
{
    progress = qBound(0.0, progress, 100.0);

    if (qAbs(progress - m_progress) < EPSILON) {
        return;
    }

    m_progress = progress;

    if (m_circularProgress) {
        m_circularProgress->setValue(progress);
    }

    emit progressChanged(m_progress);
}

void MetricCard::setTitle(const QString &title)
{
    if (m_title == title) {
        return;
    }

    m_title = title;

    if (m_titleLabel) {
        m_timeLabel->setText(title);
    }
}

void MetricCard::setPrimaryValue(const QString &value)
{
    if (m_primaryLabel) {
        m_primaryLabel->setText(value);
    }
}

void MetricCard::setSecondaryInfo(const QString &line1, const QString &line2)
{
    if (m_secondaryLabel1) {
        m_secondaryLabel1->setText(line1);
        m_secondaryLabel1->setVisible(!line1.isEmpty());
    }

    if (m_secondaryLabel2) {
        m_secondaryLabel2->setText(line2);
        m_secondaryLabel2->setVisible(!line2.isEmpty());
    }
}

void MetricCard::setStatus(MetricStatus status)
{
    if (m_status == status) {
        return;
    }

    m_status = status;

    if (m_circularProgress) {
        m_circularProgress->setStatusColor(status);
    }

    setupStyling();
    update();
}

void MetricCard::setCardType(CardType type)
{
    if (m_cardType == type) {
        return;
    }

    m_cardType == type;
    updateProgressType();
    setupStyling();
}

void MetricCard::updateCPUData(const CPUData &data)
{
    if (m_cardType != CardType::CPU) {
        return;
    }

    setProgress(data.totalUsage);
    setStatus(data.status);

    setPrimaryValue(formatPercentage(data.totalUsage));
    setSecondaryInfo(
        QString("TEMP %1").arg(formatTemperature(data.temperature)),
        QString("CLOCK %1G").arg(data.averageFrequency / 1000.0, 0, 'f', 1)
    );
}

void MetricCard::updateMemoryData(const MemoryData &data)
{
    if (m_cardType != CardType::Memory) {
        return;
    }

    setProgress(data.usagePercentage);
    setStatus(data.status);

    setPrimaryValue(QString("%1/%2").arg(
        formatMemoryValue(data.usedRAM),
        formatMemoryValue(data.totalRAM))
    );
    setSecondaryInfo(
        QString("MEM %1").arg(formatMemoryValue(data.usedRAM)),
        QString("USAGE %1%").arg(QString::number(data.usagePercentage, 'f', 1))
    );
}

void MetricCard::updateGPUData(const GPUData &data)
{
    if (m_cardType != CardType::GPU) {
        return;
    }

    setProgress(data.usage);
    setStatus(data.status);

    setPrimaryValue(formatPercentage(data.usage));
    setSecondaryInfo(
        QString("TEMP %1").arg(formatTemperature(data.temperature)),
        QString("MEM %1").arg(formatMemoryValue(data.memoryUsed))
    );
}

void MetricCard::updateStorageData(const StorageData &data)
{
    if (m_cardType != CardType::Storage) {
        return;
    }

    setProgress(data.totalUsagePercentage);
    setStatus(data.status);

    setPrimaryValue(formatPercentage(data.totalUsagePercentage));

    if (!data.devices.isEmpty()) {
        const auto& primaryDevice = data.devices.first();
        setSecondaryInfo(
            QString("C: %1").arg(formatPercentage(primaryDevice.usagePercentage)),
            QString("TEMP %1").arg(formatTemperature(primaryDevice.temperature))
        );
    }

}

void MetricCard::updateNetworkData(const NetworkData &data)
{
    if (m_cardType != CardType::Network) {
        return;
    }

    // Network card uses special layout
    if (m_downloadLabel && m_uploadLabel) {
        m_downloadLabel->setText(QString("↓%1").arg(
            SystemUtils::formatBytes(static_cast<qint64>(data.totalDownloadSpeed))
        ));
        m_uploadLabel->setText(QString("↑%1").arg(
            SystemUtils::formatBytes(static_cast<qint64>(data.totalUploadSpeed))
        ));
    }

}

void MetricCard::updateSystemData(const SystemData &data)
{
    if (m_cardType != CardType::System) {
        return;
    }

    // DateTime card uses special layout
    QDateTime currentTime = QDateTime::currentDateTime();

    if (m_dateLabel) {
        m_dateLabel->setText(currentTime.toString("yyyy/M/d"));
    }

    if (m_timeLabel) {
        m_timeLabel->setText(currentTime.toString("hh:mm"));
    }
}

void MetricCard::setHoverEnabled(bool enabled)
{
    m_hoverEnabled = enabled;
}

void MetricCard::setEnabled(bool enabled)
{
    QWidget::setEnabled(enabled);

    if (m_circularProgress) {
        m_circularProgress->setEnabled(enabled);
    }

    setupStyling();
}

void MetricCard::animateClick()
{
    if (m_clickAnimation && m_clickAnimation->state() != QAbstractAnimation::Running) {
        m_clickAnimation->start();
    }
}

void MetricCard::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Card background
    QRect cardRect = rect().adjusted(CARD_MARGIN, CARD_MARGIN, -CARD_MARGIN, -CARD_MARGIN);

    painter.setBrush(QColor(getBackgroundColor()));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(cardRect, 8, 8);

    // Border for focus/hover state
    if (hasFocus() || underMouse()) {
        QPen borderPen{QColor(getCardColor())};
        borderPen.setWidth(2);
        painter.setPen(borderPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(cardRect, 8, 8);
    }

    QWidget::paintEvent(event);
}
void MetricCard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isEnabled()) {
        m_isPressed = true;
        animateClick();
    }
    QWidget::mousePressEvent(event);
}

void MetricCard::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_isPressed && isEnabled()) {
        m_isPressed = false;

        if (rect().contains(event->pos())) {
            emit cardClicked(m_cardType);
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void MetricCard::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isEnabled()) {
        emit cardDoubleClicked(m_cardType);
    }
    QWidget::mouseDoubleClickEvent(event);
}

void MetricCard::enterEvent(QEnterEvent *event)
{
    if (m_hoverEnabled && isEnabled() && m_hoverAnimation) {
        m_hoverAnimation->setDirection(QAbstractAnimation::Forward);
        m_hoverAnimation->start();
    }
    QWidget::enterEvent(event);
}

void MetricCard::leaveEvent(QEvent *event)
{
    if (m_hoverEnabled && isEnabled() && m_hoverAnimation) {
        m_hoverAnimation->setDirection(QAbstractAnimation::Backward);
        m_hoverAnimation->start();
    }
    QWidget::leaveEvent(event);
}

void MetricCard::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

QSize MetricCard::sizeHint() const
{
    return QSize(CARD_MIN_WIDTH, CARD_MIN_HEIGHT);
}

QSize MetricCard::minimumSizeHint() const
{
    return QSize(CARD_MIN_WIDTH, CARD_MIN_HEIGHT);
}

void MetricCard::onProgressClicked()
{
    emit cardClicked(m_cardType);
}

void MetricCard::onHoverAnimationFinished()
{

}

void MetricCard::initializeUI()
{
    // Widget setup
    setMinimumSize(CARD_MIN_WIDTH, CARD_MIN_HEIGHT);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::ClickFocus);

    setupLayout();
    setupAnimations();
    setupStyling();
}

void MetricCard::setupLayout()
{
    // Main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(CARD_PADDING, CARD_PADDING, CARD_PADDING, CARD_PADDING);
    m_mainLayout->setSpacing(4);

    // Header layout (title + progress)
    m_headerLayout = new QHBoxLayout();
    m_headerLayout->setSpacing(8);

    // Title label
    m_titleLabel = new QLabel(m_title);
    m_titleLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 10px;").arg(TEXT_SECONDARY));
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Circular progress
    m_circularProgress = new CircularProgress();
    m_circularProgress->setDiameter(PROGRESS_SIZE);
    m_circularProgress->setColor(getCardColor());
    connect(m_circularProgress, &CircularProgress::clicked, this, &MetricCard::onProgressClicked);

    m_headerLayout->addWidget(m_titleLabel, 1);
    m_headerLayout->addWidget(m_circularProgress, 0);

    // Primary value label
    m_primaryLabel = new QLabel();
    m_primaryLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 12px;").arg(TEXT_PRIMARY));
    m_primaryLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Secondary info layout
    m_infoLayout = new QVBoxLayout();
    m_infoLayout->setSpacing(2);

    m_secondaryLabel1 = new QLabel();
    m_secondaryLabel1->setStyleSheet(QString("color: %1; font-size: 9px;").arg(TEXT_SECONDARY));
    m_secondaryLabel1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_secondaryLabel2 = new QLabel();
    m_secondaryLabel2->setStyleSheet(QString("color: %1; font-size: 9px;").arg(TEXT_SECONDARY));
    m_secondaryLabel2->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_infoLayout->addWidget(m_secondaryLabel1);
    m_infoLayout->addWidget(m_secondaryLabel2);

    // Add to main layout
    m_mainLayout->addLayout(m_headerLayout);
    m_mainLayout->addWidget(m_primaryLabel);
    m_mainLayout->addLayout(m_infoLayout);
    m_mainLayout->addStretch();

    updateProgressType();
}

void MetricCard::setupAnimations()
{
    // Hover animation
    m_hoverAnimation = new QPropertyAnimation(this, "geometry");
    m_hoverAnimation->setDuration(ANIMATION_DURATION);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutQuad);
    connect(m_hoverAnimation, &QPropertyAnimation::finished, this, &MetricCard::onHoverAnimationFinished);

    // Click animation
    m_clickAnimation = new QPropertyAnimation(this, "geometry");
    m_clickAnimation->setDuration(ANIMATION_DURATION / 2);
    m_clickAnimation->setEasingCurve(QEasingCurve::OutQuad);
}

void MetricCard::setupStyling()
{
    // Card shadow effect
    if (!m_shadowEffect) {
        m_shadowEffect = new QGraphicsDropShadowEffect();
        m_shadowEffect->setBlurRadius(8);
        m_shadowEffect->setOffset(2, 2);
        m_shadowEffect->setColor(QColor(0, 0, 0, 50));
        setGraphicsEffect(m_shadowEffect);
    }

    // Update progress color
    if (m_circularProgress) {
        m_circularProgress->setColor(getCardColor());
    }

    // Update styling based on enabled state
    setStyleSheet(QString(
                      "MetricCard { background-color: %1; border-radius: 8px; }"
                      "MetricCard:disabled { background-color: %2; }"
                      ).arg(getBackgroundColor(), TEXT_MUTED));
}

QString MetricCard::getCardColor() const
{
    switch (m_cardType) {
        case CardType::CPU:
            return CPU_COLOR;
        case CardType::GPU:
            return GPU_COLOR;
        case CardType::Memory:
            return RAM_COLOR;
        case CardType::Storage:
            return STORAGE_COLOR;
        case CardType::Network:
            return NETWORK_UP_COLOR;
        case CardType::System:
        default:
            return ACCENT_BLUE;
    }
}

QString MetricCard::getBackgroundColor() const
{
    if (!isEnabled()) {
        return BG_HOVER;
    }

    switch (m_status) {
        case MetricStatus::Warning:
            return QString(BG_CARD).replace("#2d3142", "#3d3142"); // Slightly warmer
        case MetricStatus::Critical:
            return QString(BG_CARD).replace("#2d3142", "#3d2142"); // Slightly redder
        case MetricStatus::Normal:
        case MetricStatus::Unknown:
        default:
            return BG_CARD;
    }
}

void MetricCard::updateProgressType()
{
    if (!m_circularProgress) {
        return;
    }

    // Most cards use circular progress Storage mioght use linear
    if (m_cardType == CardType::Storage) {
        // Could implement linear progress for HDD card
        m_circularProgress->setDiameter(40); // Smaller for linear style
    }
    else {
        m_circularProgress->setDiameter(PROGRESS_SIZE);
    }
}

QString MetricCard::formatMemoryValue(qint64 bytes) const
{
    return SystemUtils::formatBytes(bytes);
}

QString MetricCard::formatPercentage(double percentage) const
{
    return QString("%1%").arg(static_cast<int>(qRound(percentage)));
}

QString MetricCard::formatTemperature(double celsius) const
{
    return QString("%1°").arg(static_cast<int>(qRound(celsius)));
}

