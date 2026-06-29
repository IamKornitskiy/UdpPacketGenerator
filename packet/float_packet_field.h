#ifndef FLOAT_PACKET_FIELD_H
#define FLOAT_PACKET_FIELD_H

#include "base_packet_field.h"

class FloatPacketField : public BasePacketField
{
public:
    explicit FloatPacketField(const QString &name,
                              const QString &type,
                              quint32 size,
                              float min,
                              float max,
                              float initialValue = 0,
                              quint8 decimals = 2,
                              FieldSource source = FieldSource::Constant,
                              QDataStream::ByteOrder byteOrder = QDataStream::LittleEndian);

    QByteArray bytes() const override; // return value in bytes
    void incrementCounter() override {};

    // static function for checking the validity of JSON object data, and pre-constructor
    static std::unique_ptr<FloatPacketField> fromJson(const QJsonObject &obj,
                                                      QString *outError = nullptr);

    float min() const;
    float max() const;
    quint8 decimals() const;

    float value() const;
    void setValue(float newValue);

private:
    float m_max; //  maximum value
    float m_min; // minimum value
    quint8 m_decimals = 2;

    float m_value = 0;
};

#endif // FLOAT_PACKET_FIELD_H
