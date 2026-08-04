#include <QJsonObject>
#include <QtConcurrent>
#include <QtTest>
#include "csv_packet_field.h"

class TestCsvPacketField : public QObject
{
    Q_OBJECT

private slots:
    void testConstructor()
    {
        CsvPacketField field("csv", "csv", 4, FieldSource::Input, "1,2,3");

        QCOMPARE(field.name(), QString("csv"));
        QCOMPARE(field.size(), quint32(4));
        QCOMPARE(field.source(), FieldSource::Input);
        QCOMPARE(field.value(), QString("1,2,3"));
    }

    void testIsValidValidCsv()
    {
        // Valid CSV strings
        QVERIFY(!CsvPacketField::isValid("1,2,3"));
        QVERIFY(!CsvPacketField::isValid("a,b,c"));
        QVERIFY(!CsvPacketField::isValid("1,2,3\r\n4,5,6"));
        QVERIFY(!CsvPacketField::isValid("\"quoted\",value"));
        QVERIFY(!CsvPacketField::isValid("\"hello\",\"world\""));
        QVERIFY(!CsvPacketField::isValid("1,\"2\",3"));
        QVERIFY(!CsvPacketField::isValid("\"escaped ""quote""\",value"));
        QVERIFY(!CsvPacketField::isValid("\"multi\nline\",value"));
    }

    void testIsValidErrors_data()
    {
        QTest::addColumn<QString>("csv");
        QTest::addColumn<QString>("expectedError");

        QTest::newRow("empty") << "" << "CSV string is empty";
        QTest::newRow("unclosed_quote") << "\"unclosed" << "Unclosed quote";
        QTest::newRow("unclosed_quote_with_comma") << "\"unclosed,value" << "Unclosed quote";
        QTest::newRow("unclosed_quote_multiline") << "\"multi\nline" << "Unclosed quote";
    }

    void testIsValidErrors()
    {
        QFETCH(QString, csv);
        QFETCH(QString, expectedError);

        CsvValidationError error = CsvPacketField::isValid(csv);
        QVERIFY(error);
        QVERIFY2(error.message.contains(expectedError),
                 qPrintable(QString("Expected error containing '%1', got '%2'").arg(expectedError, error.message)));
    }

    void testSetValueValid()
    {
        CsvPacketField field("csv", "csv", 0, FieldSource::Input);
        QString validCsv = "1,2,3";
        QString errorMsg;

        bool result = field.setValue(validCsv, &errorMsg);
        QVERIFY(result);
        QVERIFY(errorMsg.isEmpty());
        QCOMPARE(field.value(), validCsv);
    }

    void testSetValueInvalid()
    {
        CsvPacketField field("csv", "csv", 0, FieldSource::Input);
        QString invalidCsv = "\"unclosed";
        QString errorMsg;

        bool result = field.setValue(invalidCsv, &errorMsg);
        QVERIFY(!result);
        QVERIFY(!errorMsg.isEmpty());
        QVERIFY(errorMsg.contains("Unclosed quote"));
        // Value should not change
        QCOMPARE(field.value(), QString(""));
    }

    void testFromJsonValid()
    {
        QJsonObject obj;
        obj["name"] = "csv_data";
        obj["type"] = "csv";
        obj["size"] = 64;
        obj["source"] = "input";
        obj["value"] = "1,2,3";

        QString error;
        auto field = CsvPacketField::fromJson(obj, &error);

        QVERIFY(field != nullptr);
        QVERIFY(error.isEmpty());
        QCOMPARE(field->name(), QString("csv_data"));
        QCOMPARE(field->type(), QString("csv"));
        QCOMPARE(field->size(), quint32(64));
        QCOMPARE(field->source(), FieldSource::Input);
        QCOMPARE(field->value(), QString("1,2,3"));
    }

    void testFromJsonInvalidValue()
    {
        QJsonObject obj;
        obj["name"] = "csv_data";
        obj["type"] = "csv";
        obj["size"] = 64;
        obj["source"] = "input";
        obj["value"] = "\"unclosed"; // Invalid CSV

        QString error;
        auto field = CsvPacketField::fromJson(obj, &error);

        // Field is created, but value is reset and error is set
        QVERIFY(field != nullptr);
        QVERIFY(!error.isEmpty());
        QVERIFY(error.contains("Unclosed quote"));
        QCOMPARE(field->value(), QString(""));
    }

    void testFromJsonMissingRequired()
    {
        QJsonObject obj;
        obj["type"] = "csv";
        // Missing "name"

        QString error;
        auto field = CsvPacketField::fromJson(obj, &error);

        QVERIFY(field == nullptr);
        QVERIFY(!error.isEmpty());
    }

    void testThreadSafety()
    {
        CsvPacketField field("csv", "csv", 0, FieldSource::Input, "1,2,3");

        QtConcurrent::run([&field]() {
            for (int i = 0; i < 100; ++i) {
                QString csv = QString("%1,%2,%3").arg(i).arg(i * 2).arg(i * 3);
                field.setValue(csv);
                field.incrementCounter();
                field.bytes();
                field.value();
                CsvPacketField::isValid(csv);
            }
        }).waitForFinished();
    }
};

QTEST_MAIN(TestCsvPacketField)
#include "tst_csv_packet_field.moc"
