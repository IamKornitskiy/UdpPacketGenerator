#include <QJsonDocument>
#include <QJsonObject>
#include <QtTest>
#include "integer_packet_field.h"
#include "packet_template.h"
#include <memory>

class TestPacketTemplate : public QObject
{
    Q_OBJECT

private slots:
    void testLoadValidJson()
    {
        QByteArray json = R"({
            "fields": [
                {"name": "length", "type": "int16"},
                {"name": "counter", "type": "int32", "value_source": "counter", "min": 0, "max": 1000}
            ]
        })";

        PacketTemplate tmpl;
        QString error = tmpl.loadFromJson(json);
        QVERIFY2(error.isEmpty(), qPrintable("Unexpected error: " + error));
        QCOMPARE(tmpl.fields().size(), size_t(2));
        QCOMPARE(tmpl.fields()[0]->name(), QString("length"));
        QCOMPARE(tmpl.fields()[1]->name(), QString("counter"));

        auto intField = dynamic_cast<IntegerPacketField *>(tmpl.fields()[0].get());
        QVERIFY(intField != nullptr);
        QCOMPARE(intField->min(), qint64(std::numeric_limits<qint16>::min()));
        QCOMPARE(intField->max(), qint64(std::numeric_limits<qint16>::max()));
    }

    void testEmptyFieldsArray()
    {
        QByteArray json = R"({"fields": []})";
        PacketTemplate tmpl;
        QString error = tmpl.loadFromJson(json);
        QVERIFY2(!error.isEmpty(), "Expected error for empty fields array");
        QVERIFY(error.contains("No 'fields' array"));
    }

    void testMissingFieldsArray()
    {
        QByteArray json = R"({"other": []})";
        PacketTemplate tmpl;
        QString error = tmpl.loadFromJson(json);
        QVERIFY2(!error.isEmpty(), "Expected error for missing fields array");
        QVERIFY(error.contains("No 'fields' array"));
    }

    void testMalformedJson()
    {
        QByteArray json = "not a json";
        PacketTemplate tmpl;
        QString error = tmpl.loadFromJson(json);
        QVERIFY2(!error.isEmpty(), "Expected parse error");
        QVERIFY(error.contains("JSON parse error"));
    }

    void testUnknownFieldType()
    {
        QByteArray json = R"({
            "fields": [
                {"name": "str", "type": "string"}
            ]
        })";
        PacketTemplate tmpl;
        QString error = tmpl.loadFromJson(json);
        QVERIFY2(!error.isEmpty(), "Expected error for unknown type");
        QVERIFY(error.contains("Unknown type: string"));
    }

    void testDuplicateFieldNames()
    {
        QByteArray json = R"({
            "fields": [
                {"name": "length", "type": "int16"},
                {"name": "length", "type": "int32"}
            ]
        })";
        PacketTemplate tmpl;
        QString error = tmpl.loadFromJson(json);
        QVERIFY2(!error.isEmpty(), "Expected duplicate name error");
        QVERIFY(error.contains("Duplicate field name"));
    }

    void testInvalidIntegerField()
    {
        QByteArray json = R"({
            "fields": [
                {"name": "bad", "type": "int8", "min": 100, "max": 50}
            ]
        })";
        PacketTemplate tmpl;
        QString error = tmpl.loadFromJson(json);
        QVERIFY2(!error.isEmpty(), "Expected error from IntegerPacketField");
        QVERIFY(error.contains("greater than or equal to"));
    }

    void testMultipleFieldsSameType()
    {
        QByteArray json = R"({
            "fields": [
                {"name": "a", "type": "int8"},
                {"name": "b", "type": "int8", "min": 10, "max": 100}
            ]
        })";
        PacketTemplate tmpl;
        QVERIFY(tmpl.loadFromJson(json).isEmpty());
        QCOMPARE(tmpl.fields().size(), size_t(2));
    }
};

QTEST_MAIN(TestPacketTemplate)
#include "tst_packet_template.moc"