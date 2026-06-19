#ifndef PACKET_FIELD_H
#define PACKET_FIELD_H

#include <QByteArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QString>
#include "field__data_generator.h"

enum class FieldSource { Input, Constant, Counter };

class BasePacketField
{
public:
    virtual ~BasePacketField() = default;

    QString m_name;
    QString m_type;
    quint32 m_size = 0; // in bytes
    FieldSource m_source = FieldSource::Constant;

    QByteArray m_defaultValue;

    virtual std::unique_ptr<FieldDataGenerator> buildGenerator() const;

    QString name() const { return m_name; }
    quint32 size() const { return m_size; }
    FieldSource source() const { return m_source; }

    static std::optional<QString> isValid(const QJsonObject &obj);

private:
    QMap<QString, FieldSource> m_sourceMap
        = {{"input", FieldSource::Input},       // for changing values
           {"constant", FieldSource::Constant}, // for constant or reserved fields
           {"counter", FieldSource::Counter}};  //for counters

    std::unique_ptr<FieldDataGenerator> m_DataGenerator;

protected:
    explicit BasePacketField(const QJsonObject &obj);
    virtual std::unique_ptr<FieldDataGenerator> createCounterGenerator() const;
    virtual QByteArray valueToBytes() const = 0;

signals:
    void sendError(const QString &error);
};

#endif // PACKET_FIELD_H
