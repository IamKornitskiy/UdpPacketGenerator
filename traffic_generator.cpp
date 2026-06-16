#include "traffic_generator.h"

TrafficGenerator::TrafficGenerator(QObject *parent)
    : QObject(parent)
{}

TrafficGenerator::~TrafficGenerator()
{
    stop();
}

void TrafficGenerator::configure(const QHostAddress &destAddr,
                                 quint16 destPort,
                                 const QHostAddress &localAddr,
                                 quint16 localPort,
                                 int intervalMs,
                                 std::shared_ptr<PacketBuilder> builder)
{
    this->m_destAddress = destAddr;
    this->m_destPort = destPort;
    this->m_localAddress = localAddr;
    this->m_localPort = localPort;
    this->m_intervalMs = intervalMs;
    this->m_builder = builder;
}

void TrafficGenerator::start()
{
    if (m_timer)
        return;

    m_socket = new QUdpSocket(this);
    if (!m_localAddress.isNull() || m_localPort != 0) {
        if (!m_socket->bind(m_localAddress, m_localPort)) {
            emit errorOccurred("Failed to bind UDP socket: " + m_socket->errorString());
            delete m_socket;
            m_socket = nullptr;
            return;
        }
    }

    m_timer = new QTimer(this);
    m_timer->setInterval(m_intervalMs);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        if (!m_builder)
            return;
        QByteArray packet = m_builder->buildPacket();
        qint64 ret = m_socket->writeDatagram(packet, m_destAddress, m_destPort);
        if (ret == -1) {
            emit errorOccurred("Send error: " + m_socket->errorString());
        }
        m_sentCount++;
        emit packetSent(m_sentCount);
    });
    m_timer->start();
}

void TrafficGenerator::stop()
{
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }
    if (m_socket) {
        m_socket->close();
        delete m_socket;
        m_socket = nullptr;
    }
    m_sentCount = 0;
}

void TrafficGenerator::updateFields(const QHash<QString, QByteArray> &values)
{
    if (!m_builder)
        return;
    for (auto it = values.begin(); it != values.end(); ++it) {
        m_builder->updateField(it.key(), it.value());
    }
}

void TrafficGenerator::setIntervalMs(int newIntervalMs)
{
    m_intervalMs = newIntervalMs;
    if (m_timer)
        m_timer->setInterval(m_intervalMs);
}
