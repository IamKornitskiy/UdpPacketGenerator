#ifndef STRING_PACKET_FIELD_H
#define STRING_PACKET_FIELD_H

#include "base_packet_field.h"

class StringPacketField : public BasePacketField
{
public:
    explicit StringPacketField(const QString &name,
                               const QString &type,
                               FieldSource source = FieldSource::Constant,
                               const QString &initialValue = QString());

    QByteArray bytes() const override;   // return value in bytes
    void incrementCounter() override {}; // not required

    QString value() const;
    void setValue(const QString &newValue);

protected:
private:
    QString m_value{};
};

#endif // STRING_PACKET_FIELD_H