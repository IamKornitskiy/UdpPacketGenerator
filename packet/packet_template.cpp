#include "packet_template.h"
#include <QJsonArray>
#include <QJsonValue>

static const QMap<QString, int> kTypeSizes = {{"uint8", 1},
                                              {"uint16", 2},
                                              {"uint32", 4},
                                              {"uint64", 8},
                                              {"int8", 1},
                                              {"int16", 2},
                                              {"int32", 4},
                                              {"int64", 8},
                                              {"float32", 4},
                                              {"float64", 8}};

static const QMap<QString, FieldInput> kTypeInputs = {{"uint8", FieldInput::SpinBox},
                                                      {"uint16", FieldInput::SpinBox},
                                                      {"uint32", FieldInput::SpinBox},
                                                      {"uint64", FieldInput::None},
                                                      {"int8", FieldInput::SpinBox},
                                                      {"int16", FieldInput::SpinBox},
                                                      {"int32", FieldInput::SpinBox},
                                                      {"int64", FieldInput::None},
                                                      {"float32", FieldInput::DoubleSpinBox},
                                                      {"float64", FieldInput::DoubleSpinBox}};
// ToDo: для 64 разрядов использовать DoubleSpinBox без десятых с шагом 1

PacketTemplate::PacketTemplate() {}

QString PacketTemplate::loadFromJson(const QByteArray &jsonData)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError)
        return QString("JSON parse error: %1").arg(parseError.errorString());

    QJsonObject root = doc.object();
    QJsonArray fieldsArray = root["fields"].toArray();
    if (fieldsArray.isEmpty())
        return "No 'fields' array in JSON";

    m_fields.clear();
    m_totalSize = 0;
    for (const QJsonValue &val : fieldsArray) {
        QJsonObject obj = val.toObject();
        PacketField field;
        QString error;
        if (!parseField(obj, field, error))
            return error;
        m_fields.append(field);
        m_totalSize += field.size;
    }

    return validate();
}

QString PacketTemplate::validate() const
{
    // Проверка на уникальность имён
    QSet<QString> names;
    for (const auto &f : m_fields) {
        if (names.contains(f.name))
            return QString("Duplicate field name: %1").arg(f.name);
        names.insert(f.name);
    }
    return {}; // OK
}

bool PacketTemplate::parseField(const QJsonObject &obj, PacketField &field, QString &error)
{
    if (!obj.contains("name") || !obj["name"].isString()) {
        error = "Field missing 'name'";
        return false;
    }
    field.name = obj["name"].toString();

    if (!obj.contains("type") || !obj["type"].isString()) {
        error = QString("Field '%1' missing 'type'").arg(field.name);
        return false;
    }
    field.type = obj["type"].toString();

    field.size = typeSize(field.type);
    if (field.size == 0 && field.type != "bytes") {
        error = QString("Field '%1': unknown type '%2' and no 'size' provided")
                    .arg(field.name, field.type);
        return false;
    }

    if (field.type == "bytes") {
        if (!obj.contains("size")) {
            error = QString("Field '%1': missing 'size'").arg(field.name);
            return false;
        }

        field.size = obj["size"].toInt();
    }

    QString sourceStr = obj["value_source"].toString("constant");
    if (sourceStr == "input") {
        field.source = FieldSource::Input;
    } else if (sourceStr == "constant") {
        field.source = FieldSource::Constant;
    } else if (sourceStr == "counter") {
        field.source = FieldSource::Counter;
    } else if (sourceStr == "reserved") {
        field.source = FieldSource::Reserved;
    } else {
        error = QString("Field '%1': unknown value_source '%2'").arg(field.name, sourceStr);
        return false;
    }

    if (field.source != FieldSource::Input) {
        field.input = FieldInput::None;
    } else {
        field.input = typeInput(field.type);

        if (field.input == FieldInput::SpinBox) {
            if (obj.contains("maxInt")) {
                field.maxInt = obj["maxInt"].toInt();
            } else {
                error = QString("Field '%1': missing 'maxInt'").arg(field.name);
            }
            if (obj.contains("minInt")) {
                field.minInt = obj["minInt"].toInt();
            } else {
                error = QString("Field '%1': missing 'minInt'").arg(field.name);
            }
        }

        if (field.input == FieldInput::DoubleSpinBox) {
            if (obj.contains("maxDouble")) {
                field.maxDouble = obj["maxDouble"].toDouble();
            } else {
                error = QString("Field '%1': missing 'maxDouble'").arg(field.name);
            }
            if (obj.contains("minDouble")) {
                field.minDouble = obj["minDouble"].toDouble();
            } else {
                error = QString("Field '%1': missing 'minDouble'").arg(field.name);
            }
        }
    }

    if (field.source == FieldSource::Counter) {
        if (obj.contains("start_value"))
            field.counterStart = static_cast<quint64>(obj["start_value"].toDouble());
    }

    // default_value_hex для constant и reserved
    if (field.source == FieldSource::Constant || field.source == FieldSource::Reserved) {
        QString hex = obj["default_value_hex"].toString("");
        // Валидация hex строки
        static QRegularExpression hexRe("^[0-9A-Fa-f]*$");
        if (!hexRe.match(hex).hasMatch()) {
            error = QString("Field '%1': invalid hex in default_value_hex").arg(field.name);
            return false;
        }
        // Преобразуем hex в QByteArray (дополним нулями до размера, если нужно)
        QByteArray bytes = QByteArray::fromHex(hex.toUtf8());
        if (bytes.size() > field.size) {
            error = QString("Field '%1': default_value_hex longer than size %2")
                        .arg(field.name)
                        .arg(field.size);
            return false;
        }
        bytes.resize(field.size); // дополнит нулями справа
        field.defaultValue = bytes;
    }

    return true;
}
