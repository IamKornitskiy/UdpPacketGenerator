#ifndef PACKET_TEMPLATE_H
#define PACKET_TEMPLATE_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QVector>

enum class FieldSource { Input, Constant, Counter, Reserved };
enum class FieldInput { None, SpinBox, DoubleSpinBox };

struct PacketField
{
    QString name; // Имя поля
    QString type; // "uint8", "uint16", "uint32", "uint64", "int8", "int16", "int32", "int64", "float32", "float64", "bytes"
    int size = 0; // Размер поля данных
    FieldSource source = FieldSource::Constant;
    FieldInput input = FieldInput::None;

    int maxInt; // Максимальное значение для input целого
    int minInt; // Минимальное значение для input целого

    double maxDouble; // Максимальное значение для input с плавающей точкой
    double minDouble; // Минимальное значение для input с плавающей точкой

    // для source == Counter
    quint64 counterStart = 0;

    // для source == Constant или Reserved
    QByteArray defaultValue; // Если значение не надо менять
};

class PacketTemplate
{
public:
    PacketTemplate();

    QVector<PacketField> m_fields;
    int m_totalSize = 0;

    QString loadFromJson(const QByteArray &jsonData);
    QString validate() const;

private:
    bool parseField(const QJsonObject &obj, PacketField &field, QString &error);
    static int typeSize(const QString &type);
    static FieldInput typeInput(const QString &type);
};

#endif // PACKET_TEMPLATE_H
