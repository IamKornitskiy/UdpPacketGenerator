#include "field_editor_factory.h"
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include "float_packet_field.h"
#include "integer_packet_field.h"
#include "json_highlighter.h"
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

        if (auto *jsonField = dynamic_cast<JsonPacketField *>(&field)) {
            auto *textEditorButton = new QPushButton(parent);
            textEditorButton->setText("Edit");

            QObject::connect(textEditorButton, &QPushButton::clicked, parent, [jsonField, parent]() {
                auto editorDialog = std::make_unique<TextFieldEditorDialog>(jsonField->name(),
                                                                            jsonField->type(),
                                                                            jsonField->value(),
                                                                            parent);

                editorDialog->setHighlighter(new JsonHighlighter(nullptr));
                editorDialog->setValidator([](const QString &text, QString *errorMsg) -> bool {
                    auto error = JsonPacketField::isValid(text);
                    if (error) {
                        if (errorMsg)
                            *errorMsg = error.value();
                        return false;
                    }
                    return true;
                });

                if (editorDialog->exec() == QDialog::Accepted) {
                    jsonField->setValue(editorDialog->plainText());
                }
            });

            return textEditorButton;
        }

        if (auto *stringField = dynamic_cast<StringPacketField *>(&field)) {
            auto *textEditorButton = new QPushButton(parent);
            textEditorButton->setText("Edit");

            QObject::connect(textEditorButton, &QPushButton::clicked, parent, [stringField, parent]() {
                auto editorDialog = std::make_unique<TextFieldEditorDialog>(stringField->name(),
                                                                            stringField->type(),
                                                                            stringField->value(),
                                                                            parent);


                if (editorDialog->exec() == QDialog::Accepted) {
                    stringField->setValue(editorDialog->plainText());
                }
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
