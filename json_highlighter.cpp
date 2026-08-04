#include "json_highlighter.h"
#include <QJsonDocument>
#include <QJsonParseError>

JsonHighlighter::JsonHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    QTextCharFormat keyFormat;
    keyFormat.setForeground(QColor("#9cdcfe")); // light-blue
    keyFormat.setFontWeight(QFont::Bold);

    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor("#ce9178")); // orange

    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor("#b5cea8")); // light-green

    QTextCharFormat boolNullFormat;
    boolNullFormat.setForeground(QColor("#569cd6")); // blue
    boolNullFormat.setFontWeight(QFont::Bold);

    QTextCharFormat punctuationFormat;
    punctuationFormat.setForeground(QColor("#d4d4d4")); // gray

    // RULES !!! ORDER IS IMPORTANT
    m_rules = {{QRegularExpression(R"("(?:[^"\\]|\\.)*"\s*:)"), keyFormat},
               {QRegularExpression(R"("(?:[^"\\]|\\.)*")"), stringFormat},
               {QRegularExpression(R"(\b\d+(?:\.\d+)?(?:[eE][+-]?\d+)?)"), numberFormat},
               {QRegularExpression(R"(\b(?:true|false|null)\b)"), boolNullFormat},
               {QRegularExpression(R"([{}\[\],:])"), punctuationFormat}};

    m_errorFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    m_errorFormat.setUnderlineColor(Qt::red);
    m_errorFormat.setBackground(QColor(255, 0, 0, 30)); // semi-opacity red
}

void JsonHighlighter::setError(int line, const QString &message)
{
    m_errorLine = line;
    m_errorMessage = message;
    rehighlight();
}

void JsonHighlighter::highlightBlock(const QString &text)
{
    if (currentBlock().blockNumber() + 1 == m_errorLine) {
        QTextCharFormat errorFmt;
        errorFmt.setUnderlineStyle(QTextCharFormat::WaveUnderline);
        errorFmt.setUnderlineColor(Qt::red);
        errorFmt.setBackground(QColor(255, 0, 0, 40)); // semi-opacity red
        setFormat(0, text.length(), errorFmt);
        return;
    }

    for (const auto &rule : m_rules) {
        auto it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            auto match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
