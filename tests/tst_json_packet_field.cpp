#include <QJsonObject>
#include <QtConcurrent>
#include <QtTest>
#include "json_packet_field.h"

class TestJsonPacketField : public QObject
{
    Q_OBJECT

private slots:
    void testConstructor()
    {
        JsonPacketField field("json", "json", 4, FieldSource::Input, "");

        QCOMPARE(field.name(), QString("json"));
        QCOMPARE(field.size(), quint32(4));
        QCOMPARE(field.source(), FieldSource::Input);
        QCOMPARE(field.value(), QString(""));
    }

    void testIsValid()
    {
        QString json("{"
                     "\"count\": ["
                     "{"
                     "\"1\": 1,"
                     "\"2\": \"2\""
                     "},"
                     "{"
                     "\"1\": 1,"
                     "\"2\": \"2\""
                     "}"
                     "]"
                     "}");

        QCOMPARE(JsonPacketField::isValid(json), std::nullopt);
    }

    void testIsValidErrors_data()
    {
        QTest::addColumn<QString>("json");
        QTest::addColumn<QString>("expectedError");

        QString closingBracket("{"
                               "\"count\": ["
                               "{"
                               "\"1\": 1,"
                               "\"2\": \"2\""
                               ","
                               "{"
                               "\"1\": 1,"
                               "\"2\": \"2\""
                               "}"
                               "]"
                               "}");

        QTest::newRow("closingBraket") << closingBracket << "unterminated object";

        QString valueSeparator("{"
                               "\"count\": ["
                               "{"
                               "\"1\": 1,"
                               "\"2\": \"2\""
                               "}"
                               "{"
                               "\"1\": 1,"
                               "\"2\": \"2\""
                               "}"
                               "]"
                               "}");

        QTest::newRow("valueSeparator") << valueSeparator << "missing value separator";

        QString unterminatedArray("{"
                                  "\"count\": ["
                                  "{"
                                  "\"1\": 1,"
                                  "\"2\": \"2\""
                                  "},"
                                  "{"
                                  "\"1\": 1,"
                                  "\"2\": \"2\""
                                  "}"
                                  ""
                                  "}");

        QTest::newRow("unterminatedArray") << unterminatedArray << "unterminated array";

        // It probably doesn't make sense to test QJsonParseError
    }

    void testIsValidErrors()
    {
        QFETCH(QString, json);
        QFETCH(QString, expectedError);

        QString error = JsonPacketField::isValid(json).value().toUtf8();
        QVERIFY2(error.contains(expectedError.toUtf8()),
                 qPrintable(
                     QString(expectedError)
                         .toUtf8())); // there may be encoding issues, so it needs to be checked on different systems
    }
};

QTEST_MAIN(TestJsonPacketField)
#include "tst_json_packet_field.moc"