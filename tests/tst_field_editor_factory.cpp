#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QSignalSpy>
#include <QSpinBox>
#include <QtTest>
#include "../field_editor_factory.h"
#include "float_packet_field.h"
#include "integer_packet_field.h"
#include "string_packet_field.h"

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

    // ---- Float + Input -> QDoubleSpinBox ----
    void testFloatInputEditor()
    {
        FloatPacketField field("test", "float32", 2, -100, 100, 50, 4, FieldSource::Input);
        QWidget *editor = FieldEditorFactory::createEditor(field);
        QVERIFY(editor != nullptr);

        QDoubleSpinBox *doubleSpinBox = qobject_cast<QDoubleSpinBox *>(editor);
        QVERIFY(doubleSpinBox != nullptr);
        QCOMPARE(doubleSpinBox->minimum(), -100);
        QCOMPARE(doubleSpinBox->maximum(), 100);
        QCOMPARE(doubleSpinBox->decimals(), 4);
        QCOMPARE(doubleSpinBox->value(), 50);

        doubleSpinBox->setValue(77);
        QCOMPARE(field.value(), double(77));

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

    // ---- Text -> QPushButton ----
    void testTextEditor()
    {
        StringPacketField field("text", "text", 0, FieldSource::Input);
        QWidget *editor = FieldEditorFactory::createEditor(field);
        QVERIFY(editor != nullptr);
        QPushButton *button = qobject_cast<QPushButton *>(editor);
        QVERIFY(button != nullptr);
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