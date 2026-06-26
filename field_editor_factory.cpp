#include "field_editor_factory.h"
#include <QLabel>
#include <QSpinBox>
#include "integer_packet_field.h"

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

        // TO DO:
        // if (auto* floatField = dynamic_cast<FloatField*>(&field)) { ... }
    } else if (field.source() == FieldSource::Constant) {
        auto label = new QLabel(QString("Const"));
        return label;
    } else if (field.source() == FieldSource::Counter) {
        auto label = new QLabel(QString("Counter"));
        return label;
    }
    return nullptr;
}
