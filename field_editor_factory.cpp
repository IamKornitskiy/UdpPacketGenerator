#include "field_editor_factory.h"
#include <QSpinBox>
#include "integer_packet_field.h"

QWidget *FieldEditorFactory::createEditor(BasePacketField &field, QWidget *parent)
{
    if (auto *intField = dynamic_cast<IntegerPacketField *>(&field)) {
        auto *spinBox = new QSpinBox(parent);
        spinBox->setRange(intField->min(), intField->max());
        spinBox->setValue(intField->value());

        QObject::connect(spinBox,
                         QOverload<int>::of(&QSpinBox::valueChanged),
                         [intField](int newValue) { intField->setValue(newValue); });

        // if dont input -- disabled.
        if (intField->source() != FieldSource::Input) {
            spinBox->setEnabled(false);
        }
        return spinBox;
    }

    // TO DO:
    // if (auto* floatField = dynamic_cast<FloatField*>(&field)) { ... }

    return nullptr;
}
