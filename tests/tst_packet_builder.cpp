#include <QtTest>
#include "integer_packet_field.h"
#include "packet_builder.h"
#include <memory>

class TestPacketBuilder : public QObject
{
    Q_OBJECT

private slots:
    void testEmptyFields()
    {
        std::vector<std::unique_ptr<BasePacketField>> fields;
        QByteArray packet = PacketBuilder::buildPacket(fields);
        QCOMPARE(packet.size(), 0);
    }

    void testSingleConstantField()
    {
        auto field = std::make_unique<IntegerPacketField>("length",
                                                          "int16",
                                                          2,
                                                          -100,
                                                          100,
                                                          0x1234,
                                                          FieldSource::Constant,
                                                          QDataStream::LittleEndian);

        std::vector<std::unique_ptr<BasePacketField>> fields;
        fields.push_back(std::move(field));

        QByteArray packet = PacketBuilder::buildPacket(fields);
        QCOMPARE(packet.size(), 2);
        QCOMPARE((quint8) packet.at(0), 0x12);
        QCOMPARE((quint8) packet.at(1), 0x34);

        auto intField = dynamic_cast<IntegerPacketField *>(fields[0].get());
        QVERIFY(intField);
        QCOMPARE(intField->value(), qint64(0x1234));
    }

    void testCounterField()
    {
        auto field = std::make_unique<IntegerPacketField>("counter",
                                                          "int32",
                                                          4,
                                                          0,
                                                          1000,
                                                          42,
                                                          FieldSource::Counter,
                                                          QDataStream::BigEndian);

        std::vector<std::unique_ptr<BasePacketField>> fields;
        fields.push_back(std::move(field));

        QByteArray packet = PacketBuilder::buildPacket(fields);
        QCOMPARE(packet.size(), 4);
        QCOMPARE((quint8) packet.at(0), 0x2A);
        QCOMPARE((quint8) packet.at(1), 0x00);

        auto intField = dynamic_cast<IntegerPacketField *>(fields[0].get());
        QVERIFY(intField);
        QCOMPARE(intField->value(), qint64(43));
    }

    //  Counter Constant
    void testMixedFields()
    {
        std::vector<std::unique_ptr<BasePacketField>> fields;
        fields.push_back(std::make_unique<IntegerPacketField>(
            "id", "int8", 1, 0, 100, 10, FieldSource::Input, QDataStream::LittleEndian));
        fields.push_back(std::make_unique<IntegerPacketField>(
            "cnt", "int16", 2, 0, 1000, 100, FieldSource::Counter, QDataStream::BigEndian));
        fields.push_back(std::make_unique<IntegerPacketField>(
            "reserved", "int8", 1, 0, 255, 0xFF, FieldSource::Constant, QDataStream::LittleEndian));

        QByteArray packet = PacketBuilder::buildPacket(fields);

        QCOMPARE(packet.size(), 4); // 1+2+1

        QCOMPARE((quint8) packet.at(0), 0x0A);

        QCOMPARE((quint8) packet.at(1), 0x64);
        QCOMPARE((quint8) packet.at(2), 0x00);

        // reserved: 0xFF -> 0xFF
        QCOMPARE((quint8) packet.at(3), 0xFF);

        auto idField = dynamic_cast<IntegerPacketField *>(fields[0].get());
        auto cntField = dynamic_cast<IntegerPacketField *>(fields[1].get());
        auto resField = dynamic_cast<IntegerPacketField *>(fields[2].get());
        QCOMPARE(idField->value(), qint64(10));
        QCOMPARE(cntField->value(), qint64(101));
        QCOMPARE(resField->value(), qint64(0xFF));
    }
};

QTEST_MAIN(TestPacketBuilder)
#include "tst_packet_builder.moc"