#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QStyle>
#include <QWidget>
#include <QStringList>

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    void setTitle(const QString &title);

    QMenuBar* menuBar() const { return m_menuBar; }
    void populateThemesMenu(const QStringList &themeNames, const QStringList &themeKeys, const QString &currentThemeKey);

signals:
    void themeSelected(const QString &themeKey);
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
    void onAbout();
    void onOpen();
    void onExit();

private:
    void setupUi();
    void setupMenuBar();
    void updateIcons();
    static QIcon makeIcon(QStyle::StandardPixmap standardIcon, const QColor &color);

    QLabel *m_iconLabel;
    QLabel *m_titleLabel;
    QMenuBar *m_menuBar;
    QMenu *m_themesMenu;
    QPushButton *m_minButton;
    QPushButton *m_maxButton;
    QPushButton *m_closeButton;

    QIcon m_iconMin;
    QIcon m_iconMax;
    QIcon m_iconNormal;
    QIcon m_iconClose;
};

#endif // TITLEBAR_H
