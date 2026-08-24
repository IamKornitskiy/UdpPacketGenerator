#ifndef STRING_PACKET_FIELD_H
#define STRING_PACKET_FIELD_H

#include "base_packet_field.h"
#include <memory>

class StringPacketField : public BasePacketField
{
public:
    explicit StringPacketField(const QString &name,
                               const QString &type,
                               quint32 size,
                               FieldSource source = FieldSource::Constant,
                               const QString &initialValue = QString());

    QByteArray bytes() const override;   // return value in bytes
    void incrementCounter() override {}; // not required

    QString value() const;
    virtual void setValue(const QString &newValue);

    // static function for checking the validity of JSON object data, and pre-constructor
    static std::unique_ptr<StringPacketField> fromJson(const QJsonObject &obj,
                                                       QString *outError = nullptr);

protected:
    QString m_value{};
};

#endif // STRING_PACKET_FIELD_H