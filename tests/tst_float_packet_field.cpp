#include <QJsonObject>
#include <QtConcurrent>
#include <QtTest>
#include "float_packet_field.h"
#include <memory>

class TestFloatPacketField : public QObject
{
    Q_OBJECT

private slots:
    void testConstructor()
    {
        FloatPacketField field("constant",
                               "float",
                               4,
                               -100.2,
                               100.4,
                               50.5,
                               3,
                               FieldSource::Constant,
                               QDataStream::BigEndian);

        QCOMPARE(field.name(), QString("constant"));
        QCOMPARE(field.size(), quint32(4));
        QCOMPARE(field.source(), FieldSource::Constant);
        QCOMPARE(field.value(), double(50.5));
        QCOMPARE(field.min(), double(-100.2));
        QCOMPARE(field.max(), double(100.4));
        QCOMPARE(field.decimals(), quint8(3));
    }

    void testSetValueAndValue()
    {
        FloatPacketField field("v", "float", 4, -1000, 1000);
        field.setValue(123.1);
        QCOMPARE(field.value(), double(123.1));
        field.setValue(-1.2);
        QCOMPARE(field.value(), double(-1.2));
    }

    void testIncrementCounter()
    {
        FloatPacketField field("cnt", "float", 4, 0, 1000, 10, 3, FieldSource::Counter);
        field.incrementCounter();
        QCOMPARE(field.value(), double(10));
        field.incrementCounter();
        QCOMPARE(field.value(), double(10));
    }

    void testFromJsonValid_data() // success
    {
        QTest::addColumn<QJsonObject>("json");
        QTest::addColumn<QString>("expectedName");
        QTest::addColumn<double>("expectedMin");
        QTest::addColumn<double>("expectedMax");
        QTest::addColumn<FieldSource>("expectedSource");
        QTest::addColumn<QDataStream::ByteOrder>("expectedOrder");

        QJsonObject minimal;
        minimal["name"] = "length";
        minimal["type"] = "float";
        QTest::newRow("minimal") << minimal << "length" << qint64(std::numeric_limits<float>::min())
                                 << qint64(std::numeric_limits<float>::max())
                                 << FieldSource::Constant << QDataStream::LittleEndian;

        QJsonObject full;
        full["name"] = "const";
        full["type"] = "double";
        full["source"] = "constant";
        full["order"] = "be";
        full["min"] = -5;
        full["max"] = 1000.5;
        QTest::newRow("full") << full << "count" << double(-5) << double(1000.5)
                              << FieldSource::Counter << QDataStream::BigEndian;
    }

    // void testFromJsonValid()
    // {
    //     QFETCH(QJsonObject, json);
    //     QFETCH(QString, expectedName);
    //     QFETCH(double, expectedMin);
    //     QFETCH(double, expectedMax);
    //     QFETCH(FieldSource, expectedSource);
    //     QFETCH(QDataStream::ByteOrder, expectedOrder);

    //     QString error;
    //     auto field = FloatPacketField::fromJson(json, &error);
    //     QVERIFY2(field != nullptr, qPrintable("fromJson failed: " + error));
    //     QCOMPARE(field->name(), expectedName);
    //     QCOMPARE(field->min(), expectedMin);
    //     QCOMPARE(field->max(), expectedMax);
    //     QCOMPARE(field->source(), expectedSource);

    //     field->setValue(0x0102);
    //     QByteArray bytes = field->bytes();
    //     if (expectedOrder == QDataStream::LittleEndian) {
    //         QCOMPARE((quint8) bytes.at(0), 0x01);
    //         QCOMPARE((quint8) bytes.at(1), 0x02);
    //     } else {
    //         QCOMPARE((quint8) bytes.at(0), 0x02);
    //         QCOMPARE((quint8) bytes.at(1), 0x01);
    //     }
    // }

    // void testFromJsonErrors_data() // for error
    // {
    //     QTest::addColumn<QJsonObject>("json");
    //     QTest::addColumn<QString>("expectedErrorSubstr");

    //     QJsonObject noName;
    //     QTest::newRow("missing name") << noName << "Field missing 'name'";

    //     QJsonObject noType;
    //     noType["name"] = "f";
    //     QTest::newRow("missing type") << noType << "missing 'type'";

    //     QJsonObject badMin;
    //     badMin["name"] = "f";
    //     badMin["type"] = "int8";
    //     badMin["min"] = -200;
    //     QTest::newRow("min out of bounds") << badMin << "less than min limit";

    //     QJsonObject badMax;
    //     badMax["name"] = "f";
    //     badMax["type"] = "int8";
    //     badMax["max"] = 200;
    //     QTest::newRow("max out of bounds") << badMax << "greater than max limit";

    //     QJsonObject minEqMax;
    //     minEqMax["name"] = "f";
    //     minEqMax["type"] = "int8";
    //     minEqMax["min"] = 50;
    //     minEqMax["max"] = 50;
    //     QTest::newRow("min == max") << minEqMax << "greater than or equal to";

    //     QJsonObject minGtMax;
    //     minGtMax["name"] = "f";
    //     minGtMax["type"] = "int8";
    //     minGtMax["min"] = 60;
    //     minGtMax["max"] = 50;
    //     QTest::newRow("min > max") << minGtMax << "greater than or equal to";
    // }

    // void testFromJsonErrors()
    // {
    //     QFETCH(QJsonObject, json);
    //     QFETCH(QString, expectedErrorSubstr);

    //     QString error;
    //     auto field = FloatPacketField::fromJson(json, &error);
    //     QVERIFY2(field == nullptr, "Expected null but got a valid field");
    //     QVERIFY2(error.contains(expectedErrorSubstr),
    //              qPrintable(
    //                  QString("Error '%1' should contain '%2'").arg(error, expectedErrorSubstr)));
    // }

    // void testFromJsonInvalidSourceOrderIgnored()
    // {
    //     QJsonObject obj;
    //     obj["name"] = "test";
    //     obj["type"] = "int8";
    //     obj["source"] = "unknown";
    //     obj["order"] = "unknown";

    //     QString error;
    //     auto field = FloatPacketField::fromJson(obj, &error);
    //     QVERIFY(field != nullptr);
    //     QCOMPARE(field->source(), FieldSource::Constant); // default
    //     // byteOrder default = LittleEndian
    //     field->setValue(0x42);
    //     QByteArray bytes = field->bytes();
    //     QCOMPARE(bytes.size(), 1);
    //     QCOMPARE((quint8) bytes.at(0), 0x42);
    // }

    // void testBytesLittleEndian()
    // {
    //     FloatPacketField
    //         field("le", "int32", 4, 0, 1000, 0, FieldSource::Constant, QDataStream::LittleEndian);
    //     field.setValue(0x01020304);
    //     QByteArray bytes = field.bytes();
    //     QCOMPARE(bytes.size(), 4);
    //     QCOMPARE((quint8) bytes.at(0), 0x01);
    //     QCOMPARE((quint8) bytes.at(1), 0x02);
    //     QCOMPARE((quint8) bytes.at(2), 0x03);
    //     QCOMPARE((quint8) bytes.at(3), 0x04);
    // }

    // void testBytesBigEndian()
    // {
    //     FloatPacketField
    //         field("be", "int32", 4, 0, 1000, 0, FieldSource::Constant, QDataStream::BigEndian);
    //     field.setValue(0x01020304);
    //     QByteArray bytes = field.bytes();
    //     QCOMPARE(bytes.size(), 4);
    //     QCOMPARE((quint8) bytes.at(0), 0x04);
    //     QCOMPARE((quint8) bytes.at(1), 0x03);
    //     QCOMPARE((quint8) bytes.at(2), 0x02);
    //     QCOMPARE((quint8) bytes.at(3), 0x01);
    // }

    // void testThreadSafety()
    // {
    //     FloatPacketField field("ts", "int32", 4, 0, 1000);
    //     QtConcurrent::run([&field]() {
    //         for (int i = 0; i < 100; ++i) {
    //             field.setValue(i);
    //             field.incrementCounter();
    //             field.bytes();
    //             field.value();
    //         }
    //     }).waitForFinished();
    // }
};

QTEST_MAIN(TestFloatPacketField)
#include "tst_float_packet_field.moc"