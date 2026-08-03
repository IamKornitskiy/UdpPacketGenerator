#pragma once

#include "string_packet_field.h"

struct NmeaValidationError
{
    int offset = -1; // -1, если ошибки нет
    QString message;
    int line = -1;
    int column = -1;

    explicit operator bool() const { return offset != -1; }
    QString value() { return message; }
};

class NmeaPacketField : public StringPacketField
{
public:
    NmeaPacketField(const QString &name,
                    const QString &type,
                    quint32 size,
                    FieldSource source = FieldSource::Constant,
                    const QString &initialValue = QString());

    static NmeaValidationError isValid(const QString &text);
    bool setValue(const QString &v, QString *errorMsg = nullptr);
};
