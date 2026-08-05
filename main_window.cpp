#include "main_window.h"
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <QUiLoader>
#include <QTimer>
#include <QUrl>
#include "./ui_main_window.h"
#include "field_editor_factory.h"
#include "custom_statusbar.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    QWidget *oldCentral = takeCentralWidget();

    QWidget *container = new QWidget(this);
    QVBoxLayout *vLayout = new QVBoxLayout(container);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);

    m_titleBar = new TitleBar(this);
    vLayout->addWidget(m_titleBar);

    connect(m_titleBar, &TitleBar::openRequested, this, &MainWindow::onLoadJson);
    connect(m_titleBar, &TitleBar::exitRequested, this, &QWidget::close);
    connect(m_titleBar, &TitleBar::aboutRequested, this, &MainWindow::onAbout);
    connect(m_titleBar, &TitleBar::themeToggled, this, &MainWindow::onThemeToggled);

    // Main content from UI
    if (oldCentral) {
        oldCentral->setParent(container);
        vLayout->addWidget(oldCentral);
    }

    setCentralWidget(container);

    m_titleBar->setTitle(QString("Udp Packet Generator v%1").arg(APP_VERSION));
    m_dynamicLayout = new QFormLayout(ui->scrollAreaWidgetContents);
    ui->scrollAreaWidgetContents->setLayout(m_dynamicLayout);

    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(false);

    connect(ui->btnLoadJson, &QPushButton::clicked, this, &MainWindow::onLoadJson);
    connect(ui->btnStart, &QPushButton::clicked, this, &MainWindow::onStart);
    connect(ui->btnStop, &QPushButton::clicked, this, &MainWindow::onStop);

    ui->sbInterval->setValue(1000);

    // Initialize version checker
    m_versionChecker = new VersionChecker(this);
    connect(m_versionChecker, &VersionChecker::checkComplete,
            this, &MainWindow::onVersionCheckComplete);

    // Check for updates after a short delay (to not block UI startup)
    QTimer::singleShot(1000, this, [this]() {
        m_versionChecker->checkForUpdates();
    });

    statusBar()->showMessage("Ready");
    CustomStatusBar *customStatusBar = new CustomStatusBar(this);
    setStatusBar(customStatusBar);
    customStatusBar->setStatusReady();
}

MainWindow::~MainWindow()
{
    if (m_isRunning)
        onStop();
    delete ui;
}

void MainWindow::onLoadJson()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open JSON Packet Template",
                                                    "",
                                                    "JSON Files (*.json)");
    if (fileName.isEmpty())
        return;

    ui->leLoadPath->setText(fileName);

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Cannot open file: " + file.errorString());
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QString err = m_packetTemplate.loadFromJson(data);
    if (!err.isEmpty()) {
        QMessageBox::critical(this, "Invalid JSON", err);
        return;
    }

    if (m_isRunning)
        onStop();

    buildDynamicFields();

    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
    CustomStatusBar *customBar = qobject_cast<CustomStatusBar*>(statusBar());
    if (customBar) {
        customBar->setStatus("Loaded " + fileName, false);
    } else {
        statusBar()->showMessage("Loaded " + fileName);
    }
}

void MainWindow::onStart()
{
    // eng comments is coming soon
    if (m_isRunning)
        return;

    if (m_packetTemplate.fields().empty()) {
        QMessageBox::warning(this, "Error", "Load a JSON template first.");
        return;
    }

    QHostAddress destAddr;
    destAddr.setAddress(ui->leDestAddress->text());
    if (destAddr.isNull()) {
        QMessageBox::warning(this, "Error", "Invalid destination IP address.");
        return;
    }
    quint16 destPort = static_cast<quint16>(ui->sbDestPort->value());

    m_generator = new TrafficGenerator();
    m_workerThread = new QThread(this);
    m_generator->moveToThread(m_workerThread);

    connect(m_generator, &TrafficGenerator::packetSent, this, &MainWindow::onPacketSent);
    connect(m_generator, &TrafficGenerator::errorOccurred, this, &MainWindow::onError);
    connect(ui->sbInterval, &QSpinBox::valueChanged, m_generator, &TrafficGenerator::setIntervalMs);

    QHostAddress localAddr("0.0.0.0");
    quint16 localPort = ui->sbSrcPort->value();
    int intervalMs = ui->sbInterval->value();

    m_generator->configure(destAddr,
                           destPort,
                           localAddr,
                           localPort,
                           intervalMs,
                           m_packetTemplate.fields());

    m_workerThread->start();
    QMetaObject::invokeMethod(m_generator, "start", Qt::QueuedConnection);

    m_isRunning = true;
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);
    CustomStatusBar *customBar = qobject_cast<CustomStatusBar*>(statusBar());
    if (customBar) {
        customBar->setStatusRunning();
    } else {
        statusBar()->showMessage("Running...");
    }
}

void MainWindow::onStop()
{
    if (!m_isRunning)
        return;

    if (m_generator && m_workerThread && m_workerThread->isRunning()) {
        QMetaObject::invokeMethod(m_generator, "stop", Qt::BlockingQueuedConnection);
        m_workerThread->quit();
        m_workerThread->wait();

        delete m_generator;
        m_generator = nullptr;
        delete m_workerThread;
        m_workerThread = nullptr;
    }

    m_isRunning = false;
    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
    CustomStatusBar *customBar = qobject_cast<CustomStatusBar*>(statusBar());
    if (customBar) {
        customBar->setStatusStopped();
    } else {
        statusBar()->showMessage("Stopped");
    }
}

void MainWindow::onPacketSent(int count)
{
    CustomStatusBar *customBar = qobject_cast<CustomStatusBar*>(statusBar());
    if (customBar) {
        customBar->setStatus("Packets sent: " + QString::number(count), false);
    } else {
        statusBar()->showMessage("Packets sent: " + QString::number(count));
    }
}

void MainWindow::onError(const QString &msg)
{
    CustomStatusBar *customBar = qobject_cast<CustomStatusBar*>(statusBar());
    if (customBar) {
        customBar->setStatusError(msg);
    } else {
        statusBar()->showMessage("Error: " + msg);
    }
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "About UDP Packet Generator",
                       QString("<h2>UDP Packet Generator v%1</h2>"
                               "<p>A flexible UDP packet generator configured through JSON templates.</p>"
                               "<p>Built with Qt6 and C++17.</p>"
                               "<p>© 2025-2026 Oleg Kornitskiy</p>"
                               "<p><a href='https://github.com/IamKornitskiy/UdpPacketGenerator'>GitHub</a></p>")
                           .arg(APP_VERSION));
}

void MainWindow::onThemeToggled()
{
    CustomStatusBar *customBar = qobject_cast<CustomStatusBar*>(statusBar());
    if (customBar) {
        customBar->setStatus("Theme toggled", false);
    }
}

void MainWindow::onVersionCheckComplete(bool newerAvailable, const QString &latestVersion, const QString &error)
{
    if (!error.isEmpty()) {
        // Silent fail: don't bother the user with network errors
        qDebug() << "Version check failed:" << error;
        return;
    }

    if (newerAvailable) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Update Available");
        msgBox.setText(QString("A new version of UDP Packet Generator is available!\n\n"
                               "Current version: v%1\n"
                               "Latest version: %2")
                           .arg(APP_VERSION, latestVersion));
        msgBox.setInformativeText("Would you like to download it now?");

        QPushButton *downloadButton = msgBox.addButton("Download", QMessageBox::AcceptRole);
        QPushButton *okButton = msgBox.addButton("Later", QMessageBox::RejectRole);
        msgBox.setDefaultButton(downloadButton);

        msgBox.exec();

        if (msgBox.clickedButton() == downloadButton) {
            QDesktopServices::openUrl(QUrl("https://github.com/IamKornitskiy/UdpPacketGenerator/releases"));
        }
    } else if (!latestVersion.isEmpty()) {
        qDebug() << "You are using the latest version (" << latestVersion << ")";
    }
}

void MainWindow::clearDynamicFields()
{
    if (!m_dynamicLayout)
        return;
    while (m_dynamicLayout->rowCount() > 0) {
        m_dynamicLayout->removeRow(0);
    }
    m_fieldEditors.clear();
}

void MainWindow::buildDynamicFields()
{
    clearDynamicFields();

    for (const auto &field : m_packetTemplate.fields()) {
        QWidget *editor = FieldEditorFactory::createEditor(*field, ui->scrollAreaWidgetContents);
        if (editor) {
            m_dynamicLayout->addRow(field->name(), editor);
            m_fieldEditors.insert(field->name(), editor);
        }
    }
}
