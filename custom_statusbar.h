#ifndef CUSTOM_STATUSBAR_H
#define CUSTOM_STATUSBAR_H

#include <QStatusBar>
#include <QLabel>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QPointer>

class CustomStatusBar : public QStatusBar
{
    Q_OBJECT

public:
    explicit CustomStatusBar(QWidget *parent = nullptr);
    ~CustomStatusBar();

    void setStatus(const QString &message, bool isError = false);
    void setStatusReady();
    void setStatusRunning();
    void setStatusStopped();
    void setStatusError(const QString &error);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void initIndicators();
    void updateIndicator(bool isError);
    void animateIndicator();
    void stopAnimation();

    QLabel *m_statusLabel;
    QLabel *m_indicatorLabel;
    QPointer<QPropertyAnimation> m_animation;
    bool m_isError;
};

#endif // CUSTOM_STATUSBAR_H
