#ifndef TRAFFIC_GENERATOR_H
#define TRAFFIC_GENERATOR_H

#include <QHostAddress>
#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include <QVector>
#include "base_packet_field.h"

class TrafficGenerator : public QObject
{
    Q_OBJECT
public:
    explicit TrafficGenerator(QObject *parent = nullptr);
    ~TrafficGenerator();

    void configure(const QString &destAddrsStr,
                   const QString &destPortsStr, // <--- 修改点 1：接收多端口字符串
                   const QHostAddress &localAddr,
                   quint16 localPort,
                   int intervalMs,
                   const std::vector<std::unique_ptr<BasePacketField>> &fields);

public slots:
    void start();
    void stop();
    void setIntervalMs(int newIntervalMs);

signals:
    void packetSent(int totalPackets);
    void errorOccurred(const QString &msg);

private:
    QUdpSocket *m_socket = nullptr;
    QTimer *m_timer = nullptr;
    QVector<QHostAddress> m_destAddresses;
    QVector<quint16> m_destPorts;               // <--- 修改点 2：把单个端口变成了端口列表
    QHostAddress m_localAddress;
    quint16 m_localPort = 0;
    int m_sentCount = 0;
    int m_intervalMs = 1000;
    const std::vector<std::unique_ptr<BasePacketField>> *m_fields{};
};

#endif // TRAFFIC_GENERATOR_H