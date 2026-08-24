#include "csv_packet_field.h"
#include <memory>
#include <QRegularExpression>
#include <QStringList>

CsvPacketField::CsvPacketField(const QString &name,
                               const QString &type,
                               quint32 size,
                               FieldSource source,
                               const QString &initialValue)
    : StringPacketField(name, type, static_cast<int>(size), source, initialValue)
{}

CsvValidationError CsvPacketField::isValid(const QString &text)
{
    CsvValidationError result;

    // Check for empty or whitespace-only strings
    if (text.trimmed().isEmpty()) {
        result.offset = 0;
        result.message = "CSV string is empty or contains only whitespace";
        result.line = 1;
        result.column = 1;
        return result;
    }

    bool insideQuotes = false;
    int quoteStartLine = 1;
    int quoteStartColumn = 1;
    int currentLine = 1;
    int currentColumn = 1;

    for (int i = 0; i < text.length(); ++i) {
        QChar ch = text.at(i);

        if (ch == '\n') {
            if (insideQuotes) {
            }
            ++currentLine;
            currentColumn = 1;
        } else if (ch == '"') {
            if (i + 1 < text.length() && text.at(i + 1) == '"') {
                ++i;
                currentColumn += 2;
                continue;
            }
            if (!insideQuotes) {
                insideQuotes = true;
                quoteStartLine = currentLine;
                quoteStartColumn = currentColumn;
            } else {
                insideQuotes = false;
            }
        } else if (ch == ',' && !insideQuotes) {
        }
        ++currentColumn;
    }

    if (insideQuotes) {
        result.offset = text.length() - 1;
        result.message = "Unclosed quote";
        result.line = quoteStartLine;
        result.column = quoteStartColumn;
        return result;
    }

    return result; // offset == -1
}

bool CsvPacketField::setValue(const QString &v, QString *errorMsg)
{
    auto error = isValid(v);
    if (error) {
        if (errorMsg)
            *errorMsg = error.message;
        return false;
    }
    StringPacketField::setValue(v);
    return true;
}

std::unique_ptr<CsvPacketField> CsvPacketField::fromJson(const QJsonObject &obj, QString *outError)
{
    auto validationError = BasePacketField::jsonIsValid(obj);
    if (validationError) {
        if (outError)
            *outError = validationError.value();
        return nullptr;
    }

    auto name = obj["name"].toString();
    auto type = obj["type"].toString();
    auto source = FieldSource::Constant;
    auto size = 0;

    if (obj.contains("source")) {
        QString sourceName = obj["source"].toString();
        if (kSourceMap.contains(sourceName))
            source = kSourceMap[sourceName];
    }

    if (obj.contains("size")) {
        size = obj["size"].toInt();
    }

    QString value{};
    if (obj.contains("value")) {
        auto error = isValid(obj["value"].toString());
        if (error) {
            if (outError)
                *outError = error.value();
        } else {
            value = obj["value"].toString();
        }
    }

    auto field = std::make_unique<CsvPacketField>(name, type, size, source, value);
    return field;
}
