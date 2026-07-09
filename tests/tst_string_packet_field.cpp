#include <QJsonObject>
#include <QtConcurrent>
#include <QtTest>
#include "string_packet_field.h"

class TestStringPacketField : public QObject
{
    Q_OBJECT

private slots:
    void testConstructor()
    {
        StringPacketField field("string", "string", 4, FieldSource::Input, "test");

        QCOMPARE(field.name(), QString("string"));
        QCOMPARE(field.size(), quint32(4));
        QCOMPARE(field.source(), FieldSource::Input);
        QCOMPARE(field.value(), QString("test"));
    }

    void testSetValueAndValue()
    {
        StringPacketField field("string", "string", 0, FieldSource::Input);
        field.setValue("text");
        QCOMPARE(field.value(), QString("text"));
        field.setValue("new text");
        QCOMPARE(field.value(), QString("new text"));
    }

    void testBytes()
    {
        StringPacketField fieldString("string", "string", 0, FieldSource::Input, "test");
        QString testString("Any Text"); // test english
        fieldString.setValue(testString);
        QByteArray bytes = fieldString.bytes();
        QCOMPARE(bytes.size(), testString.toUtf8().size());
        QCOMPARE((quint8) bytes.at(0), 0x41);
        QCOMPARE((quint8) bytes.at(1), 0x6E);
        QCOMPARE((quint8) bytes.at(2), 0x79);
        QCOMPARE((quint8) bytes.at(3), 0x20);
        QCOMPARE((quint8) bytes.at(4), 0x54);
        QCOMPARE((quint8) bytes.at(5), 0x65);
        QCOMPARE((quint8) bytes.at(6), 0x78);
        QCOMPARE((quint8) bytes.at(7), 0x74);

        testString = "Любой Текст"; // test russian
        fieldString.setValue(testString);
        bytes = fieldString.bytes();
        QCOMPARE(bytes.size(), testString.toUtf8().size());
        QCOMPARE((quint8) bytes.at(0), 0xD0);
        QCOMPARE((quint8) bytes.at(1), 0x9B);
        QCOMPARE((quint8) bytes.at(2), 0xD1);
        QCOMPARE((quint8) bytes.at(3), 0x8E);
        QCOMPARE((quint8) bytes.at(4), 0xD0);
        QCOMPARE((quint8) bytes.at(5), 0xB1);
        QCOMPARE((quint8) bytes.at(6), 0xD0);
        QCOMPARE((quint8) bytes.at(7), 0xBE);
        QCOMPARE((quint8) bytes.at(8), 0xD0);
        QCOMPARE((quint8) bytes.at(9), 0xB9);
        QCOMPARE((quint8) bytes.at(10), 0x20);
        QCOMPARE((quint8) bytes.at(11), 0xD0);
        QCOMPARE((quint8) bytes.at(12), 0xA2);
        QCOMPARE((quint8) bytes.at(13), 0xD0);
        QCOMPARE((quint8) bytes.at(14), 0xB5);
        QCOMPARE((quint8) bytes.at(15), 0xD0);
        QCOMPARE((quint8) bytes.at(16), 0xBA);
        QCOMPARE((quint8) bytes.at(17), 0xD1);
        QCOMPARE((quint8) bytes.at(18), 0x81);
        QCOMPARE((quint8) bytes.at(19), 0xD1);
        QCOMPARE((quint8) bytes.at(20), 0x82);
    }

    void testZeroEnds()
    {
        StringPacketField fieldString("string", "string", 6, FieldSource::Input, "Text");
        QByteArray bytes = fieldString.bytes();
        QCOMPARE((quint8) bytes.at(0), 0x54);
        QCOMPARE((quint8) bytes.at(1), 0x65);
        QCOMPARE((quint8) bytes.at(2), 0x78);
        QCOMPARE((quint8) bytes.at(3), 0x74);
        QCOMPARE((quint8) bytes.at(4), 0x00);
        QCOMPARE((quint8) bytes.at(5), 0x00);
    }

    void testThreadSafety()
    {
        StringPacketField field("string", "string", 0, FieldSource::Input, "");
        QtConcurrent::run([&field]() {
            for (int i = 0; i < 100; ++i) {
                field.setValue(QString("%1").arg(i));
                field.incrementCounter();
                field.bytes();
                field.value();
            }
        }).waitForFinished();
    }
};

QTEST_MAIN(TestStringPacketField)
#include "tst_string_packet_field.moc"