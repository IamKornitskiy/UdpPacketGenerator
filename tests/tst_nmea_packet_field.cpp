#include <QJsonObject>
#include <QtConcurrent>
#include <QtTest>
#include "nmea_packet_field.h"

class TestNmeaPacketField : public QObject
{
    Q_OBJECT

private slots:
    void testConstructor()
    {
        NmeaPacketField field("nmea", "nmea", 4, FieldSource::Input, "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n");

        QCOMPARE(field.name(), QString("nmea"));
        QCOMPARE(field.size(), quint32(4));
        QCOMPARE(field.source(), FieldSource::Input);
        QCOMPARE(field.value(), QString("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n"));
    }

    void testIsValidValidSentences()
    {
        // Valid NMEA sentences
        QVERIFY(!NmeaPacketField::isValid("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n"));
        QVERIFY(!NmeaPacketField::isValid("$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A\r\n"));
        QVERIFY(!NmeaPacketField::isValid(
            "$GPGSA,A,3,04,05,09,12,24,25,28,31,32,33,34,35,1.2,1.0,0.8*3A\r\n"));
        QVERIFY(!NmeaPacketField::isValid(
            "$GPGSV,3,1,12,04,80,000,45,05,75,000,42,09,60,000,38,12,55,000,37*77\r\n"));
        // Valid with LF only
        QVERIFY(!NmeaPacketField::isValid("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\n"));
    }

    void testIsValidErrors_data()
    {
        QTest::addColumn<QString>("sentence");
        QTest::addColumn<QString>("expectedError");

        QTest::newRow("empty") << "" << "Empty sentence";
        QTest::newRow("no_dollar") << "GPGGA,123519*47\r\n" << "Must start with '\$'";
        QTest::newRow("no_terminator") << "$GPGGA,123519*47" << "Must end with CR+LF or LF";
        QTest::newRow("no_star") << "$GPGGA,123519\r\n" << "Missing '*' before checksum";
        QTest::newRow("checksum_too_short") << "$GPGGA,123519*4\r\n" << "Checksum must be exactly two hex digits";
        QTest::newRow("checksum_invalid_chars") << "$GPGGA,123519*Y\r\n"
                                                << "Checksum must be exactly two hex digits";
        QTest::newRow("checksum_mismatch") << "$GPGGA,123519*FF\r\n" << "Checksum mismatch";
    }

    void testIsValidErrors()
    {
        QFETCH(QString, sentence);
        QFETCH(QString, expectedError);

        NmeaValidationError error = NmeaPacketField::isValid(sentence);
        QVERIFY(error);
        QVERIFY2(error.message.contains(expectedError),
                 qPrintable(QString("Expected error containing '%1', got '%2'").arg(expectedError, error.message)));
    }

    void testSetValueValid()
    {
        NmeaPacketField field("nmea", "nmea", 0, FieldSource::Input);
        QString validSentence = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";
        QString errorMsg;

        bool result = field.setValue(validSentence, &errorMsg);
        QVERIFY(result);
        QVERIFY(errorMsg.isEmpty());
        QCOMPARE(field.value(), validSentence);
    }

    void testSetValueInvalid()
    {
        NmeaPacketField field("nmea", "nmea", 0, FieldSource::Input);
        QString invalidSentence = "$GPGGA,123519*FF\r\n";
        QString errorMsg;

        bool result = field.setValue(invalidSentence, &errorMsg);
        QVERIFY(!result);
        QVERIFY(!errorMsg.isEmpty());
        QVERIFY(errorMsg.contains("Checksum mismatch"));
        // Value should not change
        QCOMPARE(field.value(), QString(""));
    }

    void testFromJsonValid()
    {
        QJsonObject obj;
        obj["name"] = "gps";
        obj["type"] = "nmea";
        obj["size"] = 64;
        obj["source"] = "input";
        obj["value"] = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";

        QString error;
        auto field = NmeaPacketField::fromJson(obj, &error);

        QVERIFY(field != nullptr);
        QVERIFY(error.isEmpty());
        QCOMPARE(field->name(), QString("gps"));
        QCOMPARE(field->type(), QString("nmea"));
        QCOMPARE(field->size(), quint32(64));
        QCOMPARE(field->source(), FieldSource::Input);
        QCOMPARE(field->value(), QString("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n"));
    }

    void testFromJsonInvalidValue()
    {
        QJsonObject obj;
        obj["name"] = "gps";
        obj["type"] = "nmea";
        obj["size"] = 64;
        obj["source"] = "input";
        obj["value"] = "$GPGGA,123519*FF\r\n"; // Invalid checksum

        QString error;
        auto field = NmeaPacketField::fromJson(obj, &error);

        // Should still create field but with empty value and error set
        QVERIFY(field != nullptr);
        QVERIFY(!error.isEmpty());
        QVERIFY(error.contains("Checksum mismatch"));
        QCOMPARE(field->value(), QString(""));
    }

    void testFromJsonMissingRequired()
    {
        QJsonObject obj;
        obj["type"] = "nmea";
        // Missing "name"

        QString error;
        auto field = NmeaPacketField::fromJson(obj, &error);

        QVERIFY(field == nullptr);
        QVERIFY(!error.isEmpty());
    }

    void testThreadSafety()
    {
        NmeaPacketField field("nmea", "nmea", 0, FieldSource::Input, "$GPGGA,123519*47\r\n");

        QtConcurrent::run([&field]() {
            for (int i = 0; i < 100; ++i) {
                QString sentence = QString("$GPGGA,%1,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n")
                                       .arg(i, 6, 10, QChar('0'));
                field.setValue(sentence);
                field.incrementCounter();
                field.bytes();
                field.value();
                NmeaPacketField::isValid(sentence);
            }
        }).waitForFinished();
    }
};

QTEST_MAIN(TestNmeaPacketField)
#include "tst_nmea_packet_field.moc"
