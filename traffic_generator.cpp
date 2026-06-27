#include "traffic_generator.h"
#include "packet_builder.h"

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
                                 const std::vector<std::unique_ptr<BasePacketField>> &fields)
{
    m_destAddress = destAddr;
    m_destPort = destPort;
    m_localAddress = localAddr;
    m_localPort = localPort;
    m_intervalMs = intervalMs;
    m_fields = &fields;
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
        if (!m_fields)
            return;
        QByteArray packet = PacketBuilder::buildPacket(*m_fields);
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

void TrafficGenerator::setIntervalMs(int newIntervalMs)
{
    m_intervalMs = newIntervalMs;
    if (m_timer)
        m_timer->setInterval(m_intervalMs);
}
