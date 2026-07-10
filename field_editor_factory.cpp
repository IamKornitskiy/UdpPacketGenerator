#include "field_editor_factory.h"
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include "float_packet_field.h"
#include "integer_packet_field.h"
#include "json_packet_field.h"
#include "string_packet_field.h"
#include "text_field_editor_dialog.h"

QWidget *FieldEditorFactory::createEditor(BasePacketField &field, QWidget *parent)
{
    if (field.source() == FieldSource::Input) {
        if (auto *intField = dynamic_cast<IntegerPacketField *>(&field)) {
            auto *spinBox = new QSpinBox(parent);
            spinBox->setRange(intField->min(), intField->max());
            spinBox->setValue(intField->value());

            QObject::connect(spinBox,
                             QOverload<int>::of(&QSpinBox::valueChanged),
                             [intField](int newValue) { intField->setValue(newValue); });

            return spinBox;
        }

        if (auto *floatField = dynamic_cast<FloatPacketField *>(&field)) {
            auto *doubleSpinBox = new QDoubleSpinBox(parent);
            doubleSpinBox->setDecimals(floatField->decimals());
            doubleSpinBox->setRange(floatField->min(), floatField->max());
            doubleSpinBox->setValue(floatField->value());

            QObject::connect(doubleSpinBox,
                             QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                             [floatField](double newValue) { floatField->setValue(newValue); });

            return doubleSpinBox;
        }

        if (auto *stringField = dynamic_cast<StringPacketField *>(&field)) {
            auto *textEditorButton = new QPushButton(parent);
            textEditorButton->setText("Edit");

            QObject::connect(textEditorButton,
                             &QPushButton::clicked,
                             parent,
                             [stringField, parent]() {
                                 auto *editorDialog = new TextFieldEditorDialog(stringField->name(),
                                                                                stringField->type(),
                                                                                stringField->value(),
                                                                                parent);
                                 editorDialog->setAttribute(Qt::WA_DeleteOnClose);

                                 // QObject::connect(editorDialog,
                                 //                  &TextFieldEditorDialog::sendNewText,
                                 //                  stringField,
                                 //                  [stringField](const QString &newText) {
                                 //                      stringField->setValue(newText);
                                 //                  });

                                 editorDialog->show();
                             });

            return textEditorButton;
        }
    } else if (field.source() == FieldSource::Constant) {
        auto label = new QLabel(QString("Const"));
        return label;
    } else if (field.source() == FieldSource::Counter) {
        auto label = new QLabel(QString("Counter"));
        return label;
    }
    return nullptr;
}
