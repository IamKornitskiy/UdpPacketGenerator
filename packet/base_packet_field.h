#ifndef PACKET_FIELD_H
#define PACKET_FIELD_H

#include <QByteArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QString>
#include "field_generator.h"

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

    // virtual bool parse(const QJsonObject &fieldObj) = 0;
    // virtual void accept(FieldVisitor &visitor) = 0;
    // virtual std::unique_ptr<FieldGenerator> buildGenerator() const = 0;

    QString name() const { return m_name; }
    quint32 size() const { return m_size; }
    FieldSource source() const { return m_source; }

    static std::optional<QString> isValid(const QJsonObject &obj);

protected:
    explicit BasePacketField(const QJsonObject &obj);

signals:
    void sendError(const QString &error);
};

#endif // PACKET_FIELD_H
