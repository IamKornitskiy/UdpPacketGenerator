#include "traffic_generator.h"
#include "packet_builder.h"
#include <memory>
#include <QStringList> // Include for string list processing

TrafficGenerator::TrafficGenerator(QObject *parent)
    : QObject(parent)
{}

TrafficGenerator::~TrafficGenerator()
{
    stop();
}

// Modification 1: Accept a comma-separated QString instead of a single QHostAddress
void TrafficGenerator::configure(const QString &destAddrsStr, 
                                 quint16 destPort,
                                 const QHostAddress &localAddr,
                                 quint16 localPort,
                                 int intervalMs,
                                 const std::vector<std::unique_ptr<BasePacketField>> &fields)
{
    // Modification 2: Parse the comma-separated IP list
    m_destAddresses.clear();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QStringList ipList = destAddrsStr.split(",", Qt::SkipEmptyParts);
#else
    QStringList ipList = destAddrsStr.split(",", QString::SkipEmptyParts);
#endif
    for (const QString &ipStr : ipList) {
        QHostAddress ip(ipStr.trimmed());
        if (!ip.isNull()) {
            m_destAddresses.append(ip);
        }
    }

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
        
        // Modification 3: Iterate through all parsed target IPs for broadcasting
        for (const auto &destAddr : m_destAddresses) {
            qint64 ret = m_socket->writeDatagram(packet, destAddr, m_destPort);
            if (ret == -1) {
                // Log error if sending to a specific IP fails, but do not block other transmissions
                emit errorOccurred("Send error to " + destAddr.toString() + ": " + m_socket->errorString());
            }
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