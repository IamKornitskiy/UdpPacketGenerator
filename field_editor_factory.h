#ifndef FIELD_EDITOR_FACTORY
#define FIELD_EDITOR_FACTORY

#include <QWidget>
#include "base_packet_field.h"

class FieldEditorFactory
{
public:
    static QWidget *createEditor(BasePacketField &field, QWidget *parent = nullptr);
};

#endif // FIELD_EDITOR_FACTORY
