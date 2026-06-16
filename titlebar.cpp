#include "titlebar.h"
#include <QApplication>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QWindow>

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    updateMaximizeIcon(false);

    if (QWidget *win = window()) {
        win->installEventFilter(this);
    }
}

static QIcon makeWhiteIcon(QStyle::StandardPixmap standardIcon)
{
    QPixmap pix = qApp->style()->standardIcon(standardIcon).pixmap(16, 16);
    QPainter painter(&pix);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pix.rect(), Qt::white);
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
    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(20, 20);
    m_iconLabel->setScaledContents(true);
    QIcon defaultIcon = qApp->style()->standardIcon(QStyle::SP_ComputerIcon);
    m_iconLabel->setPixmap(defaultIcon.pixmap(20, 20));
    layout->addWidget(m_iconLabel);

    m_titleLabel = new QLabel(QCoreApplication::applicationName().isEmpty()
                                  ? QStringLiteral("UDP Packet Generator")
                                  : QCoreApplication::applicationName(),
                              this);
    m_titleLabel->setObjectName(QStringLiteral("titleLabel"));
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

    m_closeButton = new QPushButton(this); // ✕
    m_closeButton->setObjectName(QStringLiteral("closeButton"));
    m_closeButton->setIcon(s->standardIcon(QStyle::SP_TitleBarCloseButton));
    m_closeButton->setIconSize(QSize(16, 16));
    m_closeButton->setFixedSize(32, 28);
    m_closeButton->setFlat(true);
    connect(m_closeButton, &QPushButton::clicked, this, &TitleBar::onClose);
    layout->addWidget(m_closeButton);

    QIcon whiteMin = makeWhiteIcon(QStyle::SP_TitleBarMinButton);
    QIcon whiteMax = makeWhiteIcon(QStyle::SP_TitleBarMaxButton);
    QIcon whiteNorm = makeWhiteIcon(QStyle::SP_TitleBarNormalButton);
    QIcon whiteClose = makeWhiteIcon(QStyle::SP_TitleBarCloseButton);

    m_minButton->setIcon(whiteMin);
    m_maxButton->setIcon(whiteMax); // начальная иконка (не развёрнуто)
    m_closeButton->setIcon(whiteClose);

    // Сохраняем белые иконки для последующего переключения
    // Можно запомнить их как члены класса или использовать лямбды.
    // Проще сохранить в членах:
    m_iconMin = whiteMin;
    m_iconMax = whiteMax;
    m_iconNormal = whiteNorm;
    m_iconClose = whiteClose;
}

void TitleBar::applyDefaultStyle()
{
    setStyleSheet(R"(
        #titleBar {
            background-color: #f0f0f0;
            border-bottom: 1px solid #c0c0c0;
        }
        QPushButton {
            background: transparent;
            border: none;
            border-radius: 4px;
            color: #333333;
            padding: 0px;
        }
        QPushButton:hover {
            background-color: #e0e0e0;
        }
        QPushButton:pressed {
            background-color: #c0c0c0;
        }
        #closeButton:hover {
            background-color: #E81123;
            color: white;
        }
        #closeButton:pressed {
            background-color: #BF0F1A;
        }
    )");
}

void TitleBar::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

void TitleBar::setIcon(const QIcon &icon)
{
    m_iconLabel->setPixmap(icon.pixmap(20, 20));
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
