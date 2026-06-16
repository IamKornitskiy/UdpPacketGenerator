#include "main_window.h"
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QRegularExpression>
#include <QUiLoader>
#include "./ui_main_window.h"

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

    // Основной контент из UI
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

    statusBar()->showMessage("Ready");
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
    statusBar()->showMessage("Loaded " + fileName);
}

void MainWindow::onStart()
{
    if (m_isRunning)
        return;

    // Проверка, что шаблон загружен
    if (m_packetTemplate.m_fields.isEmpty()) {
        QMessageBox::warning(this, "Error", "Load a JSON template first.");
        return;
    }

    // Проверка адреса назначения
    QHostAddress destAddr;
    destAddr.setAddress(ui->leDestAddress->text());
    if (destAddr.isNull()) {
        QMessageBox::warning(this, "Error", "Invalid destination IP address.");
        return;
    }
    quint16 destPort = static_cast<quint16>(ui->sbDestPort->value());

    // Сбор значений константных полей
    QHash<QString, QByteArray> userValues = collectConstantValues();

    // Создаём сборщик пакетов
    m_packetBuilder = std::make_shared<PacketBuilder>();
    m_packetBuilder->setup(m_packetTemplate.m_fields, userValues);

    // Создаём генератор и поток
    m_generator = new TrafficGenerator();
    m_workerThread = new QThread(this);
    m_generator->moveToThread(m_workerThread);

    connect(m_generator, &TrafficGenerator::packetSent, this, &MainWindow::onPacketSent);
    connect(m_generator, &TrafficGenerator::errorOccurred, this, &MainWindow::onError);
    connect(ui->sbInterval, &QSpinBox::valueChanged, m_generator, &TrafficGenerator::setIntervalMs);

    QHostAddress localAddr("0.0.0.0");
    quint16 localPort = ui->sbSrcPort->value();
    int intervalMs = ui->sbInterval->value();

    m_generator->configure(destAddr, destPort, localAddr, localPort, intervalMs, m_packetBuilder);

    m_workerThread->start();
    QMetaObject::invokeMethod(m_generator, "start", Qt::QueuedConnection);

    m_isRunning = true;
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);
    statusBar()->showMessage("Running...");
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
    statusBar()->showMessage("Stopped");
}

void MainWindow::onPacketSent(int count)
{
    statusBar()->showMessage("Packets sent: " + QString::number(count));
}

void MainWindow::onError(const QString &msg)
{
    statusBar()->showMessage("Error: " + msg);
}

void MainWindow::onFieldChanged()
{
    if (!m_isRunning || !m_generator)
        return;
    QHash<QString, QByteArray> values = collectConstantValues();

    QMetaObject::invokeMethod(
        m_generator,
        [gen = m_generator, vals = std::move(values)]() { gen->updateFields(vals); },
        Qt::QueuedConnection);
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

    for (const auto &field : m_packetTemplate.m_fields) {
        if (field.input == FieldInput::SpinBox) {
            QSpinBox *spinBox = new QSpinBox();
            spinBox->setMaximum(field.maxInt);
            spinBox->setMinimum(field.minInt);
            connect(spinBox,
                    QOverload<int>::of(&QSpinBox::valueChanged),
                    this,
                    &MainWindow::onFieldChanged);
            m_dynamicLayout->addRow(field.name, spinBox);
            m_fieldEditors.insert(field.name, spinBox);
        } else if (field.input == FieldInput::DoubleSpinBox) {
            QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox();
            doubleSpinBox->setMaximum(field.maxDouble);
            doubleSpinBox->setMinimum(field.minDouble);
            connect(doubleSpinBox,
                    QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                    this,
                    &MainWindow::onFieldChanged);
            m_dynamicLayout->addRow(field.name, doubleSpinBox);
            m_fieldEditors.insert(field.name, doubleSpinBox);
        } else if (field.input == FieldInput::None) {
            if (field.source == FieldSource::Constant) {
                QLabel *lbl = new QLabel(QString("%1").arg(field.defaultValue.toInt()));
                m_dynamicLayout->addRow(field.name + " (" + field.type + ")", lbl);
            } else if (field.source == FieldSource::Counter) {
                QLabel *lbl = new QLabel("[counter]");
                m_dynamicLayout->addRow(field.name + " (" + field.type + ")", lbl);
            } else if (field.source == FieldSource::Reserved) {
                QLabel *lbl = new QLabel("[reserved]");
                m_dynamicLayout->addRow(field.name + " (" + field.type + ")", lbl);
            }
        }
    }
}

QHash<QString, QByteArray> MainWindow::collectConstantValues()
{
    QHash<QString, QByteArray> values;
    for (auto it = m_fieldEditors.begin(); it != m_fieldEditors.end(); ++it) {
        const QString &fieldName = it.key();
        QWidget *w = it.value();
        if (!w)
            continue;

        // Находим описание поля из шаблона
        const PacketField *field = nullptr;
        for (const auto &f : m_packetTemplate.m_fields) {
            if (f.name == fieldName) {
                field = &f;
                break;
            }
        }
        if (!field)
            continue;

        QByteArray bytes;
        if (auto *sb = qobject_cast<QSpinBox *>(w)) {
            // Целое число (int/uint)
            qint64 val = sb->value();
            bytes = integerToBytes(val, field->size);
        } else if (auto *dsb = qobject_cast<QDoubleSpinBox *>(w)) {
            // Число с плавающей точкой (float/double)
            double val = dsb->value();
            bytes = floatToBytes(val, field->type);
        } else {
            QMessageBox::warning(this, "Error", "Unknown editor type for field " + fieldName);
            continue;
        }

        // Дополняем/обрезаем до точного размера поля
        bytes.resize(field->size);
        values.insert(fieldName, bytes);
    }
    return values;
}

QByteArray MainWindow::integerToBytes(qint64 value, int size)
{
    QByteArray buf(size, '\0');
    switch (size) {
    case 1:
        buf[0] = static_cast<char>(value);
        break;
    case 2: {
        qint16 v = static_cast<qint16>(value);
        qToLittleEndian(v, buf.data());
        break;
    }
    case 4: {
        qint32 v = static_cast<qint32>(value);
        qToLittleEndian(v, buf.data());
        break;
    }
    case 8:
        qToLittleEndian(value, buf.data());
        break;
    default:
        break;
    }
    return buf;
}

QByteArray MainWindow::floatToBytes(double value, const QString &type)
{
    QByteArray buf;
    if (type == "float32") {
        buf.resize(4);
        float f = static_cast<float>(value);
        qToLittleEndian(f, buf.data());
    } else if (type == "float64") {
        buf.resize(8);
        qToLittleEndian(value, buf.data());
    }
    return buf;
}
