#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    void setTitle(const QString &title);
    void setIcon(const QIcon &icon);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    void onMinimize();
    void onMaximizeRestore();
    void onClose();
    void updateMaximizeIcon(bool maximized);

private:
    void setupUi();
    void applyDefaultStyle();

    QLabel *m_iconLabel;
    QLabel *m_titleLabel;
    QPushButton *m_minButton;
    QPushButton *m_maxButton;
    QPushButton *m_closeButton;
};

#endif // TITLEBAR_H
