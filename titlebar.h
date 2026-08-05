#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyle>
#include <QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    void setTitle(const QString &title);
    void setIcon(const QIcon &icon);

    QMenuBar* menuBar() const { return m_menuBar; }

signals:
    void themeToggled();
    void aboutRequested();
    void openRequested();
    void exitRequested();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    void onMinimize();
    void onMaximizeRestore();
    void onClose();
    void updateMaximizeIcon(bool maximized);
    void onThemeToggle();
    void onAbout();
    void onOpen();
    void onExit();

private:
    void setupUi();
    void applyDefaultStyle();
    void setupMenuBar();
    void updateIcons();
    static QIcon makeIcon(QStyle::StandardPixmap standardIcon, const QColor &color);

    QLabel *m_iconLabel;
    QLabel *m_titleLabel;
    QMenuBar *m_menuBar;
    QPushButton *m_minButton;
    QPushButton *m_maxButton;
    QPushButton *m_closeButton;

    QIcon m_iconMin;
    QIcon m_iconMax;
    QIcon m_iconNormal;
    QIcon m_iconClose;

    bool m_darkTheme;
};

#endif // TITLEBAR_H
