#include "string_packet_field.h"

QByteArray StringPacketField::bytes() const
{ /*return serializeValue(m_value)*/
}

QString StringPacketField::value() const
{
    return m_value;
}

void StringPacketField::setValue(const QString &newValue)
{
    m_value = newValue;
}
