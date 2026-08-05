#include "custom_button.h"
#include <QPainter>
#include <QStyleOption>

CustomButton::CustomButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
    , m_animationValue(100)
    , m_isHovered(false)
{
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);

    m_animationGroup = new QParallelAnimationGroup(this);

    m_scaleAnimation = new QPropertyAnimation(this, "animationValue");
    m_scaleAnimation->setDuration(200);
    m_scaleAnimation->setEasingCurve(QEasingCurve::OutCubic);

    m_opacityAnimation = new QPropertyAnimation(this, "animationValue");
    m_opacityAnimation->setDuration(200);
    m_opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);

    m_animationGroup->addAnimation(m_scaleAnimation);
    m_animationGroup->addAnimation(m_opacityAnimation);

    setAnimationValue(100);
}

CustomButton::CustomButton(QWidget *parent)
    : CustomButton("", parent)
{
}

void CustomButton::setIcon(const QIcon &icon)
{
    QPushButton::setIcon(icon);
}

void CustomButton::setIconSize(const QSize &size)
{
    QPushButton::setIconSize(size);
}

void CustomButton::enterEvent(QEnterEvent *event)
{
    m_isHovered = true;
    m_scaleAnimation->setEndValue(110);
    m_opacityAnimation->setEndValue(100);
    m_animationGroup->start();
    QPushButton::enterEvent(event);
}

void CustomButton::leaveEvent(QEvent *event)
{
    m_isHovered = false;
    m_scaleAnimation->setEndValue(100);
    m_opacityAnimation->setEndValue(100);
    m_animationGroup->start();
    QPushButton::leaveEvent(event);
}

void CustomButton::setAnimationValue(int value)
{
    m_animationValue = value;
    update();
}

void CustomButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QStyleOptionButton option;
    option.initFrom(this);

    QRect rect = this->rect();
    int padding = (100 - m_animationValue) / 2;
    QRect animatedRect = rect.adjusted(padding, padding, -padding, -padding);

    painter.save();

    if (m_isHovered) {
        QColor shadowColor(0, 0, 0, 30);
        painter.setBrush(shadowColor);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect.adjusted(2, 2, -2, -2), 6, 6);
    }

    QBrush brush;
    if (!isEnabled()) {
        brush = QBrush(QColor(61, 79, 93));
    } else if (isDown()) {
        brush = QBrush(QColor(0, 90, 158));
    } else if (m_isHovered) {
        brush = QBrush(QColor(16, 110, 190));
    } else {
        brush = QBrush(QColor(0, 120, 212));
    }

    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(animatedRect, 6, 6);

    painter.restore();

    painter.save();
    painter.setPen(isEnabled() ? Qt::white : QColor(136, 136, 136));
    painter.setFont(font());

    QRect textRect = animatedRect.adjusted(8, 4, -8, -4);
    painter.drawText(textRect, Qt::AlignCenter, text());
    painter.restore();

    if (!icon().isNull()) {
        QSize iconSize = this->iconSize();
        QRect iconRect = animatedRect.adjusted(4, 4, -4, -4);
        iconRect.setWidth(iconSize.width());
        iconRect.setHeight(iconSize.height());
        iconRect.moveLeft(animatedRect.left() + 8);
        iconRect.moveTop((animatedRect.height() - iconSize.height()) / 2 + animatedRect.top());

        painter.save();
        painter.setClipRect(iconRect);
        icon().paint(&painter, iconRect, Qt::AlignCenter);
        painter.restore();
    }
}
