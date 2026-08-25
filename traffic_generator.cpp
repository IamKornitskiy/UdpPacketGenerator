#include "traffic_generator.h"
#include "packet_builder.h"
#include <QStringList>

TrafficGenerator::TrafficGenerator(QObject *parent)
    : QObject(parent)
{}

TrafficGenerator::~TrafficGenerator()
{
    stop();
}

// 修改点 1：接收多端口字符串 destPortsStr
void TrafficGenerator::configure(const QString &destAddrsStr, 
                                 const QString &destPortsStr,
                                 const QHostAddress &localAddr,
                                 quint16 localPort,
                                 int intervalMs,
                                 const std::vector<std::unique_ptr<BasePacketField>> &fields)
{
    // 修改点 2：彻底移除 Qt5 判断，纯 Qt6 标准写法
    m_destAddresses.clear();
    QStringList ipList = destAddrsStr.split(",", Qt::SkipEmptyParts);
    for (const QString &ipStr : ipList) {
        QHostAddress ip(ipStr.trimmed());
        if (!ip.isNull()) {
            m_destAddresses.append(ip);
        }
    }

    // 修改点 3：解析逗号分隔的端口字符串
    m_destPorts.clear();
    QStringList portList = destPortsStr.split(",", Qt::SkipEmptyParts);
    for (const QString &portStr : portList) {
        bool ok;
        quint16 port = portStr.trimmed().toUShort(&ok);
        if (ok && port > 0) {
            m_destPorts.append(port);
        }
    }

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
        
        // 修改点 4：双重循环！遍历所有目标 IP 和目标端口进行广播
        for (const auto &destAddr : m_destAddresses) {
            for (quint16 destPort : m_destPorts) {
                qint64 ret = m_socket->writeDatagram(packet, destAddr, destPort);
                if (ret == -1) {
                    // 记录具体的 IP 和端口发送失败信息
                    emit errorOccurred("Send error to " + destAddr.toString() + ":" + QString::number(destPort) + " - " + m_socket->errorString());
                }
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