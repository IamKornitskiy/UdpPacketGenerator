#ifndef PACKET_FIELD_H
#define PACKET_FIELD_H

#include <QByteArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QString>

enum class FieldSource { Input, Constant, Counter, Reserved };
enum class FieldInput { None, SpinBox, DoubleSpinBox };

class BasePacketField
{
public:
    virtual ~BasePacketField() = default;

    QString m_name;
    FieldInput m_type;
    quint32 m_size = 0; // in bytes
    FieldSource m_source = FieldSource::Constant;

    quint64 m_counterStart = 0;

    QByteArray m_defaultValue;

    virtual FieldInput inputType() const = 0;
    virtual bool parseExtra(const QJsonObject &fieldObj, QString &error) = 0;

    quint32 size() const;

protected:
    BasePacketField(quint32 size, FieldInput inputType);
};

#endif // PACKET_FIELD_H
