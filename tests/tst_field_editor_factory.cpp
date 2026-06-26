#include <QLabel>
#include <QSignalSpy>
#include <QSpinBox>
#include <QtTest>
#include "../field_editor_factory.h"
#include "integer_packet_field.h"

class TestFieldEditorFactory : public QObject
{
    Q_OBJECT

private slots:
    // ---- Integer + Input -> QSpinBox ----
    void testIntegerInputEditor()
    {
        IntegerPacketField field("test", "int16", 2, -100, 100, 50, FieldSource::Input);
        QWidget *editor = FieldEditorFactory::createEditor(field);
        QVERIFY(editor != nullptr);

        QSpinBox *spinBox = qobject_cast<QSpinBox *>(editor);
        QVERIFY(spinBox != nullptr);
        QCOMPARE(spinBox->minimum(), -100);
        QCOMPARE(spinBox->maximum(), 100);
        QCOMPARE(spinBox->value(), 50);

        spinBox->setValue(77);
        QCOMPARE(field.value(), qint64(77));

        delete editor;
    }

    // ---- Constant -> QLabel ----
    void testConstantEditor()
    {
        IntegerPacketField field("const", "int8", 1, 0, 10, 5, FieldSource::Constant);
        QWidget *editor = FieldEditorFactory::createEditor(field);
        QVERIFY(editor != nullptr);
        QLabel *label = qobject_cast<QLabel *>(editor);
        QVERIFY(label != nullptr);
        QVERIFY(label->text().contains("Const"));
        delete editor;
    }

    // ---- Counter -> QLabel ----
    void testCounterEditor()
    {
        IntegerPacketField field("counter", "int32", 4, 0, 1000, 42, FieldSource::Counter);
        QWidget *editor = FieldEditorFactory::createEditor(field);
        QVERIFY(editor != nullptr);
        QLabel *label = qobject_cast<QLabel *>(editor);
        QVERIFY(label != nullptr);
        QVERIFY(label->text().contains("Counter"));
        delete editor;
    }

    // ---- Not Integer-Field with Input -> nullptr ----
    void testUnsupportedType()
    {
        class DummyField : public BasePacketField
        {
        public:
            DummyField()
                : BasePacketField("dummy", "unknown", 1, FieldSource::Input)
            {}
            QByteArray bytes() const override { return QByteArray(); }
            void incrementCounter() override {}
        };
        DummyField dummy;
        QWidget *editor = FieldEditorFactory::createEditor(dummy);
        QVERIFY(editor == nullptr);
    }
};

QTEST_MAIN(TestFieldEditorFactory)
#include "tst_field_editor_factory.moc"