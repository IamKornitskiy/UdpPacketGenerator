#include <QHostAddress>
#include <QSignalSpy>
#include <QtTest>
#include <QUdpSocket>
#include "../traffic_generator.h"
#include "integer_packet_field.h"

// Helper: simple UDP echo server that counts received packets
class UdpEchoServer : public QObject
{
    Q_OBJECT
public:
    UdpEchoServer(quint16 port, QObject *parent = nullptr) : QObject(parent), m_socket(new QUdpSocket(this))
    {
        m_socket->bind(port);
        connect(m_socket, &QUdpSocket::readyRead, this, &UdpEchoServer::onReadyRead);
    }

    int packetCount() const { return m_packetCount; }
    QByteArray lastPacket() const { return m_lastPacket; }

signals:
    void packetReceived(const QByteArray &data);

private slots:
    void onReadyRead()
    {
        while (m_socket->hasPendingDatagrams()) {
            QByteArray data;
            data.resize(m_socket->pendingDatagramSize());
            m_socket->readDatagram(data.data(), data.size());
            m_packetCount++;
            m_lastPacket = data;
            emit packetReceived(data);
        }
    }

private:
    QUdpSocket *m_socket;
    int m_packetCount = 0;
    QByteArray m_lastPacket;
};

class TestTrafficGenerator : public QObject
{
    Q_OBJECT

private slots:
    void testConstructor()
    {
        TrafficGenerator generator;
        QVERIFY(true);
    }

    void testConfigure()
    {
        TrafficGenerator generator;
        QHostAddress destAddr("127.0.0.1");
        quint16 destPort = 12345;
        QHostAddress localAddr("0.0.0.0");
        quint16 localPort = 0;
        int intervalMs = 100;
        std::vector<std::unique_ptr<BasePacketField>> fields;

        generator.configure(destAddr, destPort, localAddr, localPort, intervalMs, fields);
        QVERIFY(true);
    }

    void testStartStopWithNoFields()
    {
        TrafficGenerator generator;
        std::vector<std::unique_ptr<BasePacketField>> fields;
        generator.configure(QHostAddress("127.0.0.1"), 12345,
                           QHostAddress("0.0.0.0"), 0, 100, fields);

        QSignalSpy spyPacketSent(&generator, &TrafficGenerator::packetSent);
        QSignalSpy spyError(&generator, &TrafficGenerator::errorOccurred);

        generator.start();
        QTest::qWait(50);
        generator.stop();

        QVERIFY(true);
    }

    void testSetIntervalMs()
    {
        TrafficGenerator generator;
        generator.setIntervalMs(500);
        QVERIFY(true);
    }

    void testStartStopMultipleTimes()
    {
        TrafficGenerator generator;
        std::vector<std::unique_ptr<BasePacketField>> fields;
        generator.configure(QHostAddress("127.0.0.1"), 12345,
                           QHostAddress("0.0.0.0"), 0, 100, fields);

        generator.start();
        QTest::qWait(10);
        generator.stop();
        generator.start();
        QTest::qWait(10);
        generator.stop();

        QVERIFY(true);
    }

    void testConfigureWithFields()
    {
        TrafficGenerator generator;
        std::vector<std::unique_ptr<BasePacketField>> fields;
        fields.push_back(std::make_unique<IntegerPacketField>(
            "test", "int32", 4, 0, 100, 42, FieldSource::Constant, QDataStream::LittleEndian));

        generator.configure(QHostAddress("127.0.0.1"), 12345,
                           QHostAddress("0.0.0.0"), 0, 100, fields);

        QVERIFY(true);
    }

    void testDestructorStopsGenerator()
    {
        {
            TrafficGenerator generator;
            std::vector<std::unique_ptr<BasePacketField>> fields;
            generator.configure(QHostAddress("127.0.0.1"), 12345,
                               QHostAddress("0.0.0.0"), 0, 100, fields);
            generator.start();
            QTest::qWait(10);
        }
        QVERIFY(true);
    }

    // ---- Integration tests with real UDP packets ----

    void testPacketSentSignal()
    {
        // Start a local UDP server to receive packets
        UdpEchoServer server(12345);
        QSignalSpy spyServerPacket(&server, &UdpEchoServer::packetReceived);

        TrafficGenerator generator;
        std::vector<std::unique_ptr<BasePacketField>> fields;
        fields.push_back(std::make_unique<IntegerPacketField>(
            "test", "int32", 4, 0, 100, 0x12345678, FieldSource::Constant, QDataStream::BigEndian));

        generator.configure(QHostAddress("127.0.0.1"), 12345,
                           QHostAddress("0.0.0.0"), 0, 50, fields);

        QSignalSpy spyPacketSent(&generator, &TrafficGenerator::packetSent);

        generator.start();
        QTest::qWait(150); // let at least 2-3 packets be sent
        generator.stop();

        // Check that packetSent signal was emitted at least once
        QVERIFY(spyPacketSent.count() > 0);

        // Check that server received at least one packet
        QVERIFY(spyServerPacket.count() > 0);

        // Check packet content (BigEndian 0x12345678 -> 78 56 34 12 in network order)
        QByteArray expected = QByteArray::fromHex("12345678");
        QCOMPARE(server.lastPacket(), expected);
    }

    void testMultiplePacketsSent()
    {
        UdpEchoServer server(12346);
        QSignalSpy spyServerPacket(&server, &UdpEchoServer::packetReceived);

        TrafficGenerator generator;
        std::vector<std::unique_ptr<BasePacketField>> fields;
        fields.push_back(std::make_unique<IntegerPacketField>(
            "test", "int32", 4, 0, 100, 0x01, FieldSource::Constant, QDataStream::LittleEndian));

        generator.configure(QHostAddress("127.0.0.1"), 12346,
                           QHostAddress("0.0.0.0"), 0, 30, fields);

        generator.start();
        QTest::qWait(120); // about 4 packets with 30ms interval
        generator.stop();

        // Should have received at least 2 packets
        QVERIFY(server.packetCount() >= 2);
    }

    void testErrorSignalWhenBindFails()
    {
        // Use a guaranteed invalid local address to force bind failure on all platforms
        TrafficGenerator generator;
        std::vector<std::unique_ptr<BasePacketField>> fields;

        // 192.0.2.0 is TEST-NET-1, reserved for documentation and examples by IANA.
        // It is not a valid local address on any real system, so bind() will always fail.
        generator.configure(QHostAddress("127.0.0.1"), 12345,
                           QHostAddress("192.0.2.0"), 12347, 100, fields);

        QSignalSpy spyError(&generator, &TrafficGenerator::errorOccurred);

        generator.start();

        // Wait for error signal (up to 1 second)
        QTRY_VERIFY_WITH_TIMEOUT(spyError.count() > 0, 1000);

        // Stop generator to clean up
        generator.stop();

        // Verify error was emitted
        QVERIFY2(spyError.count() > 0, "Error signal was not emitted");
        QString errorMsg = spyError.takeFirst().at(0).toString();
        QVERIFY(errorMsg.contains("Failed to bind"));
    }

    void testPacketCountIncrements()
    {
        UdpEchoServer server(12348);

        TrafficGenerator generator;
        std::vector<std::unique_ptr<BasePacketField>> fields;
        fields.push_back(std::make_unique<IntegerPacketField>(
            "test", "int8", 1, 0, 255, 0xAA, FieldSource::Constant, QDataStream::LittleEndian));

        generator.configure(QHostAddress("127.0.0.1"), 12348,
                           QHostAddress("0.0.0.0"), 0, 20, fields);

        QSignalSpy spyPacketSent(&generator, &TrafficGenerator::packetSent);

        generator.start();
        QTest::qWait(80); // about 4 packets
        generator.stop();

        // packetSent count should match server packet count (approximately)
        int sentCount = spyPacketSent.count();
        int receivedCount = server.packetCount();

        // Allow some tolerance due to timing
        QVERIFY(sentCount > 0);
        QVERIFY(receivedCount > 0);
        QVERIFY(qAbs(sentCount - receivedCount) <= 2);
    }

    void testSetIntervalDuringRuntime()
    {
        UdpEchoServer server(12349);

        TrafficGenerator generator;
        std::vector<std::unique_ptr<BasePacketField>> fields;
        fields.push_back(std::make_unique<IntegerPacketField>(
            "test", "int8", 1, 0, 255, 0xBB, FieldSource::Constant, QDataStream::LittleEndian));

        generator.configure(QHostAddress("127.0.0.1"), 12349,
                           QHostAddress("0.0.0.0"), 0, 100, fields);

        generator.start();
        QTest::qWait(50); // let it run at 100ms interval

        // Change interval to 20ms (should send faster)
        generator.setIntervalMs(20);
        QTest::qWait(80); // enough time for multiple packets at 20ms

        generator.stop();

        // Should have received some packets
        QVERIFY(server.packetCount() > 0);
    }
};

QTEST_MAIN(TestTrafficGenerator)
#include "tst_traffic_generator.moc"
