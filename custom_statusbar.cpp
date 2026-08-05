#include "custom_statusbar.h"
#include <QPainter>
#include <QTimer>
#include <QHBoxLayout>

CustomStatusBar::CustomStatusBar(QWidget *parent)
    : QStatusBar(parent)
    , m_isError(false)
{
    initIndicators();
    setStatusReady();
}

CustomStatusBar::~CustomStatusBar()
{
    stopAnimation();
}

void CustomStatusBar::initIndicators()
{
    QWidget *container = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    m_indicatorLabel = new QLabel(container);
    m_indicatorLabel->setObjectName("statusIndicator");
    m_indicatorLabel->setFixedSize(12, 12);

    m_statusLabel = new QLabel(container);
    m_statusLabel->setObjectName("statusText");
    m_statusLabel->setText("Ready");

    layout->addWidget(m_indicatorLabel);
    layout->addWidget(m_statusLabel);
    layout->addStretch();

    addPermanentWidget(container, 1);
}

void CustomStatusBar::setStatus(const QString &message, bool isError)
{
    m_statusLabel->setText(message);
    updateIndicator(isError);
}

void CustomStatusBar::setStatusReady()
{
    m_statusLabel->setText("Ready");
    updateIndicator(false);
    m_indicatorLabel->setStyleSheet(
        "background: #4a5c6a;"
        "border-radius: 6px;"
        "border: 1px solid #5a7a8a;"
    );
}

void CustomStatusBar::setStatusRunning()
{
    m_statusLabel->setText("Running...");
    updateIndicator(false);
    m_indicatorLabel->setStyleSheet(
        "background: #27ae60;"
        "border-radius: 6px;"
        "border: 1px solid #2ecc71;"
    );
    animateIndicator();
}

void CustomStatusBar::setStatusStopped()
{
    stopAnimation();
    m_statusLabel->setText("Stopped");
    updateIndicator(false);
    m_indicatorLabel->setStyleSheet(
        "background: #e67e22;"
        "border-radius: 6px;"
        "border: 1px solid #f39c12;"
    );
}

void CustomStatusBar::setStatusError(const QString &error)
{
    stopAnimation();
    m_statusLabel->setText("Error: " + error);
    updateIndicator(true);
    m_indicatorLabel->setStyleSheet(
        "background: #e74c3c;"
        "border-radius: 6px;"
        "border: 1px solid #ff6b6b;"
    );
}

void CustomStatusBar::updateIndicator(bool isError)
{
    m_isError = isError;
    if (isError) {
        m_indicatorLabel->setStyleSheet(
            "background: #e74c3c;"
            "border-radius: 6px;"
            "border: 1px solid #ff6b6b;"
        );
    }
}

void CustomStatusBar::stopAnimation()
{
    if (!m_animation.isNull()) {
        m_animation->stop();
        m_animation->deleteLater();
        m_animation.clear();
    }
}

void CustomStatusBar::animateIndicator()
{
    stopAnimation();

    QPropertyAnimation *anim = new QPropertyAnimation(m_indicatorLabel, "geometry", this);
    anim->setDuration(1000);
    anim->setLoopCount(-1);

    QRect startRect = m_indicatorLabel->geometry();
    QRect endRect = startRect;
    endRect.setSize(startRect.size() * 1.2);
    endRect.moveCenter(startRect.center());

    anim->setStartValue(startRect);
    anim->setKeyValueAt(0.5, endRect);
    anim->setEndValue(startRect);
    anim->setEasingCurve(QEasingCurve::InOutCubic);

    m_animation = anim;
    anim->start();
}

void CustomStatusBar::resizeEvent(QResizeEvent *event)
{
    QStatusBar::resizeEvent(event);
}
