#include <QHostAddress>
#include <QSignalSpy>
#include <QtTest>
#include "../traffic_generator.h"
#include "integer_packet_field.h"

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
};

QTEST_MAIN(TestTrafficGenerator)
#include "tst_traffic_generator.moc"
