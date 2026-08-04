#include "nmea_packet_field.h"
#include <QRegularExpression>

NmeaPacketField::NmeaPacketField(const QString &name,
                                 const QString &type,
                                 quint32 size,
                                 FieldSource source,
                                 const QString &initialValue)
    : StringPacketField(name, type, static_cast<int>(size), source, initialValue)
{}

NmeaValidationError NmeaPacketField::isValid(const QString &text)
{
    NmeaValidationError result;

    if (text.isEmpty()) {
        result.offset = 0;
        result.message = "Empty sentence";
        result.line = 1;
        result.column = 1;
        return result;
    }

    if (!text.startsWith('$')) {
        result.offset = 0;
        result.message = "Must start with '$'";
        result.line = 1;
        result.column = 1;
        return result;
    }

    if (!text.endsWith("\r\n") && !text.endsWith('\n')) {
        result.offset = text.length() - 1;
        result.message = "Must end with CR+LF or LF";

        int line = 1, col = 1;
        for (int i = 0; i < result.offset; ++i) {
            if (text.at(i) == '\n') {
                ++line;
                col = 1;
            } else {
                ++col;
            }
        }
        result.line = line;
        result.column = col;
        return result;
    }

    int starPos = text.indexOf('*');
    if (starPos == -1) {
        result.offset = text.length() - 1;
        result.message = "Missing '*' before checksum";

        int line = 1, col = 1;
        for (int i = 0; i < result.offset; ++i) {
            if (text.at(i) == '\n') {
                ++line;
                col = 1;
            } else {
                ++col;
            }
        }
        result.line = line;
        result.column = col;
        return result;
    }

    QString afterStar = text.mid(starPos + 1);
    if (afterStar.endsWith("\r\n"))
        afterStar.chop(2);
    else if (afterStar.endsWith('\n'))
        afterStar.chop(1);

    if (afterStar.length() != 2 || !afterStar[0].isLetterOrNumber()
        || !afterStar[1].isLetterOrNumber()) {
        result.offset = starPos + 1; // after *
        result.message = "Checksum must be exactly two hex digits";
        int line = 1, col = 1;
        for (int i = 0; i < result.offset; ++i) {
            if (text.at(i) == '\n') {
                ++line;
                col = 1;
            } else {
                ++col;
            }
        }
        result.line = line;
        result.column = col;
        return result;
    }

    bool ok;
    int givenChecksum = afterStar.toInt(&ok, 16);
    if (!ok) {
        result.offset = starPos + 1;
        result.message = "Invalid checksum hex";

        int line = 1, col = 1;
        for (int i = 0; i < result.offset; ++i) {
            if (text.at(i) == '\n') {
                ++line;
                col = 1;
            } else {
                ++col;
            }
        }
        result.line = line;
        result.column = col;
        return result;
    }

    QByteArray data = text.mid(1, starPos - 1).toLatin1();
    int calculatedChecksum = 0;
    for (char ch : data) {
        calculatedChecksum ^= static_cast<unsigned char>(ch);
    }

    if (calculatedChecksum != givenChecksum) {
        result.offset = starPos + 1;
        result.message = QString("Checksum mismatch: calculated %1, given %2")
                             .arg(calculatedChecksum, 2, 16, QChar('0'))
                             .arg(givenChecksum, 2, 16, QChar('0'));
        int line = 1, col = 1;
        for (int i = 0; i < result.offset; ++i) {
            if (text.at(i) == '\n') {
                ++line;
                col = 1;
            } else {
                ++col;
            }
        }
        result.line = line;
        result.column = col;
        return result;
    }

    return result; // offset == -1
}

bool NmeaPacketField::setValue(const QString &v, QString *errorMsg)
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

std::unique_ptr<NmeaPacketField> NmeaPacketField::fromJson(const QJsonObject &obj, QString *outError)
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

    auto field = std::make_unique<NmeaPacketField>(name, type, size, source, value);
    return field;
}