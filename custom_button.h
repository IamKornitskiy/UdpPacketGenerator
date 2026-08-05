#ifndef CUSTOM_BUTTON_H
#define CUSTOM_BUTTON_H

#include <QPushButton>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEnterEvent>

class CustomButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(int animationValue READ animationValue WRITE setAnimationValue)

public:
    explicit CustomButton(const QString &text = "", QWidget *parent = nullptr);
    explicit CustomButton(QWidget *parent = nullptr);

    void setIcon(const QIcon &icon);
    void setIconSize(const QSize &size);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    int animationValue() const { return m_animationValue; }
    void setAnimationValue(int value);

    QParallelAnimationGroup *m_animationGroup;
    QPropertyAnimation *m_scaleAnimation;
    QPropertyAnimation *m_opacityAnimation;
    int m_animationValue;
    bool m_isHovered;
};

#endif // CUSTOM_BUTTON_H
