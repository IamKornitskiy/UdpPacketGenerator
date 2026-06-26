#include <QJsonObject>
#include <QtTest>
#include "base_packet_field.h"
#include <optional>

class SerializeTestField : public BasePacketField
{
public:
    SerializeTestField(const QString &type,
                       quint32 size,
                       QDataStream::ByteOrder order = QDataStream::LittleEndian)
        : BasePacketField("test", type, size, FieldSource::Constant, order)
    {}

    template<typename T>
    QByteArray serializePublic(const T &value, int size)
    {
        return serializeValue(value, size);
    }

    QByteArray bytes() const override { return QByteArray(); }
    void incrementCounter() override {}
};

class TestField : public BasePacketField
{
public:
    TestField(const QString &name,
              const QString &type,
              quint32 size,
              FieldSource source = FieldSource::Constant,
              QDataStream::ByteOrder byteOrder = QDataStream::LittleEndian)
        : BasePacketField(name, type, size, source, byteOrder)
    {}

    QByteArray bytes() const override { return serializeValue<int>(42, 4); }
    void incrementCounter() override {}
};

class TestBasePacketField : public QObject
{
    Q_OBJECT

private slots:
    void testJsonIsValid_data()
    {
        QTest::addColumn<QJsonObject>("obj");
        QTest::addColumn<bool>("expectError");
        QTest::addColumn<QString>("errorSubstring");

        QJsonObject valid;
        valid["name"] = "testField";
        valid["type"] = "uint32";

        QTest::newRow("valid") << valid << false << QString();

        QJsonObject noName;
        noName["type"] = "uint32";
        QTest::newRow("missing_name") << noName << true << "Field missing 'name'";

        QJsonObject nameNotString;
        nameNotString["name"] = 123;
        nameNotString["type"] = "uint32";
        QTest::newRow("name_not_string") << nameNotString << true << "Field missing 'name'";

        QJsonObject noType;
        noType["name"] = "testField";
        QTest::newRow("missing_type") << noType << true << "missing 'type'";

        QJsonObject typeNotString;
        typeNotString["name"] = "testField";
        typeNotString["type"] = 456;
        QTest::newRow("type_not_string") << typeNotString << true << "missing 'type'";
    }

    void testJsonIsValid()
    {
        QFETCH(QJsonObject, obj);
        QFETCH(bool, expectError);
        QFETCH(QString, errorSubstring);

        auto result = BasePacketField::jsonIsValid(obj);
        if (expectError) {
            QVERIFY2(result.has_value(), "Expected error but got std::nullopt");
            QVERIFY2(result->contains(errorSubstring),
                     qPrintable(
                         QString("Error '%1' should contain '%2'").arg(*result, errorSubstring)));
        } else {
            QVERIFY2(!result.has_value(), "Expected no error but got an error");
        }
    }

    void testConstructorAndGetters()
    {
        TestField field("count", "uint32", 4, FieldSource::Counter, QDataStream::BigEndian);
        QCOMPARE(field.name(), QString("count"));
        QCOMPARE(field.size(), quint32(4));
        QCOMPARE(field.source(), FieldSource::Counter);
    }

    void testDefaultSourceAndByteOrder()
    {
        TestField field("defaultTest", "uint8", 1);
        QCOMPARE(field.source(), FieldSource::Constant); // default
    }

    void testSerializeValueLittleEndian()
    {
        // При LittleEndian код делает reverse -> фактически Big Endian
        SerializeTestField field("uint32", 4, QDataStream::LittleEndian);
        QByteArray data = field.serializePublic(quint32(0x01020304), 4);
        QCOMPARE(data.size(), 4);
        // Ожидаем: 0x01 0x02 0x03 0x04 (результат переворота системного Little Endian)
        QCOMPARE(static_cast<quint8>(data[0]), quint8(0x01));
        QCOMPARE(static_cast<quint8>(data[1]), quint8(0x02));
        QCOMPARE(static_cast<quint8>(data[2]), quint8(0x03));
        QCOMPARE(static_cast<quint8>(data[3]), quint8(0x04));
    }

    void testSerializeValueBigEndian()
    {
        // При BigEndian реверса нет -> остаётся системный Little Endian
        SerializeTestField field("uint32", 4, QDataStream::BigEndian);
        QByteArray data = field.serializePublic(quint32(0x01020304), 4);
        QCOMPARE(data.size(), 4);
        // Ожидаем: 0x04 0x03 0x02 0x01 (как в памяти)
        QCOMPARE(static_cast<quint8>(data[0]), quint8(0x04));
        QCOMPARE(static_cast<quint8>(data[1]), quint8(0x03));
        QCOMPARE(static_cast<quint8>(data[2]), quint8(0x02));
        QCOMPARE(static_cast<quint8>(data[3]), quint8(0x01));
    }

    void testSerializeString()
    {
        SerializeTestField field("string", 10, QDataStream::LittleEndian);
        QByteArray data = field.serializePublic(QString("Hello"), 10);
        QCOMPARE(data.size(), 10);
        QByteArray expected("Hello\0\0\0\0\0", 10);
        QCOMPARE(data, expected);
    }
};

QTEST_MAIN(TestBasePacketField)
#include "tst_base_packet_field.moc"