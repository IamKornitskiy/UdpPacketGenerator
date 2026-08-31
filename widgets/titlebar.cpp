#include "titlebar.h"
#include <QApplication>
#include <QEvent>
#include <QFile>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QTextStream>
#include <QWindow>

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    setupMenuBar();
    updateMaximizeIcon(false);

    if (QWidget *win = window()) {
        win->installEventFilter(this);
    }
}

QIcon TitleBar::makeIcon(QStyle::StandardPixmap standardIcon, const QColor &color)
{
    QPixmap pix = qApp->style()->standardIcon(standardIcon).pixmap(16, 16);
    QPainter painter(&pix);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pix.rect(), color);
    painter.end();
    return QIcon(pix);
}

void TitleBar::setupUi()
{
    setObjectName(QStringLiteral("titleBar"));
    setFixedHeight(38);
    setCursor(Qt::ArrowCursor);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 0, 8, 0);
    layout->setSpacing(8);

    // app icon
    // m_iconLabel = new QLabel(this);
    // m_iconLabel->setFixedSize(38, 38);
    // m_iconLabel->setScaledContents(true);
    // QIcon defaultIcon = QIcon("://style/logo.svg");
    // m_iconLabel->setPixmap(defaultIcon.pixmap(38, 38));
    // layout->addWidget(m_iconLabel);

    m_menuBar = new QMenuBar(this);
    m_menuBar->setObjectName(QStringLiteral("titleMenuBar"));
    layout->addWidget(m_menuBar);

    layout->addStretch();

    m_titleLabel = new QLabel(QCoreApplication::applicationName().isEmpty()
                                  ? QStringLiteral("UDP Packet Generator")
                                  : QCoreApplication::applicationName(),
                              this);
    m_titleLabel->setObjectName(QStringLiteral("titleLabel"));
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    layout->addWidget(m_titleLabel);

    layout->addStretch();

    QStyle *s = qApp->style();

    m_minButton = new QPushButton(this);
    m_minButton->setObjectName(QStringLiteral("minimizeButton"));
    m_minButton->setIcon(s->standardIcon(QStyle::SP_TitleBarMinButton));
    m_minButton->setIconSize(QSize(16, 16));
    m_minButton->setFixedSize(32, 28);
    m_minButton->setFlat(true);
    connect(m_minButton, &QPushButton::clicked, this, &TitleBar::onMinimize);
    layout->addWidget(m_minButton);

    m_maxButton = new QPushButton(this);
    m_maxButton->setObjectName(QStringLiteral("maximizeButton"));
    m_maxButton->setIconSize(QSize(16, 16));
    m_maxButton->setFixedSize(32, 28);
    m_maxButton->setFlat(true);
    connect(m_maxButton, &QPushButton::clicked, this, &TitleBar::onMaximizeRestore);
    layout->addWidget(m_maxButton);

    m_closeButton = new QPushButton(this);
    m_closeButton->setObjectName(QStringLiteral("closeButton"));
    m_closeButton->setIcon(s->standardIcon(QStyle::SP_TitleBarCloseButton));
    m_closeButton->setIconSize(QSize(16, 16));
    m_closeButton->setFixedSize(32, 28);
    m_closeButton->setFlat(true);
    connect(m_closeButton, &QPushButton::clicked, this, &TitleBar::onClose);
    layout->addWidget(m_closeButton);

    updateIcons();
}

void TitleBar::setupMenuBar()
{
    // File menu
    QMenu *fileMenu = m_menuBar->addMenu("&File");
    QAction *openAction = new QAction("&Open Template...", this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &TitleBar::onOpen);
    fileMenu->addAction(openAction);

    fileMenu->addSeparator();

    QAction *exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &TitleBar::onExit);
    fileMenu->addAction(exitAction);

    // View menu
    QMenu *viewMenu = m_menuBar->addMenu("&View");
    m_themesMenu = viewMenu->addMenu("&Themes");

    // Help menu
    QMenu *helpMenu = m_menuBar->addMenu("&Help");
    QAction *aboutAction = new QAction("&About", this);
    connect(aboutAction, &QAction::triggered, this, &TitleBar::onAbout);
    helpMenu->addAction(aboutAction);
}

void TitleBar::populateThemesMenu(const QStringList &themeNames, const QStringList &themeKeys, const QString &currentThemeKey)
{
    if (!m_themesMenu || themeNames.size() != themeKeys.size())
        return;

    m_themesMenu->clear();

    for (int i = 0; i < themeNames.size(); ++i) {
        QAction *action = new QAction(themeNames[i], this);
        action->setData(themeKeys[i]);
        action->setCheckable(true);
        action->setChecked(themeKeys[i] == currentThemeKey);
        connect(action, &QAction::triggered, this, [this, key = themeKeys[i]]() {
            emit themeSelected(key);
        });
        m_themesMenu->addAction(action);
    }
}

void TitleBar::onAbout()
{
    emit aboutRequested();
}

void TitleBar::onOpen()
{
    emit openRequested();
}

void TitleBar::onExit()
{
    emit exitRequested();
}

void TitleBar::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

bool TitleBar::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == window() && event->type() == QEvent::WindowStateChange) {
        updateMaximizeIcon(window()->isMaximized());
    }
    return QWidget::eventFilter(watched, event);
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (auto *win = window()->windowHandle()) {
            win->startSystemMove();
        }
    }
    QWidget::mousePressEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        onMaximizeRestore();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void TitleBar::onMinimize()
{
    window()->showMinimized();
}

void TitleBar::onMaximizeRestore()
{
    if (window()->isMaximized())
        window()->showNormal();
    else
        window()->showMaximized();
}

void TitleBar::onClose()
{
    window()->close();
}

void TitleBar::updateMaximizeIcon(bool maximized)
{
    if (maximized)
        m_maxButton->setIcon(m_iconNormal);
    else
        m_maxButton->setIcon(m_iconMax);
}

void TitleBar::updateIcons()
{
    QColor color = QColor(0x88, 0x88, 0x88);
    m_iconMin = makeIcon(QStyle::SP_TitleBarMinButton, color);
    m_iconMax = makeIcon(QStyle::SP_TitleBarMaxButton, color);
    m_iconNormal = makeIcon(QStyle::SP_TitleBarNormalButton, color);
    m_iconClose = makeIcon(QStyle::SP_TitleBarCloseButton, color);

    m_minButton->setIcon(m_iconMin);
    m_maxButton->setIcon(m_iconMax);
    m_closeButton->setIcon(m_iconClose);
}
