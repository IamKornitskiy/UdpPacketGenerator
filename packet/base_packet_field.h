#ifndef PACKET_FIELD_H
#define PACKET_FIELD_H

#include <QByteArray>
#include <QJsonObject>
#include <QMutex>
#include <QMutexLocker>
#include <QRegularExpression>
#include <QString>

enum class FieldSource { Input, Constant, Counter };

class BasePacketField
{
public:
    virtual ~BasePacketField() = default;
    virtual QByteArray bytes() const = 0; // return value in bytes
    virtual void incrementCounter() = 0;

    QString m_name;
    QString m_type;
    quint32 m_size = 0; // in bytes
    FieldSource m_source = FieldSource::Constant;

    QString name() const { return m_name; }
    quint32 size() const { return m_size; }
    FieldSource source() const { return m_source; }

    static std::optional<QString> isValid(const QJsonObject &obj);

private:
    QMap<QString, FieldSource> m_sourceMap
        = {{"input", FieldSource::Input},       // for changing values
           {"constant", FieldSource::Constant}, // for constant or reserved fields
           {"counter", FieldSource::Counter}};  // for counters

protected:
    explicit BasePacketField(const QJsonObject &obj);
    QMutexLocker<QMutex> lock() const { return QMutexLocker<QMutex>(&m_mutex); }

    mutable QMutex m_mutex;

signals:
    void sendError(const QString &error);
};

#endif // PACKET_FIELD_H
