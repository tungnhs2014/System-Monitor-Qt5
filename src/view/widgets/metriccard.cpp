/**
 * @file metriccard.cpp
 * @brief Metric card widget implementation
 * @author TungNHS
 * @version 1.0.0
 */sss

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
{}
