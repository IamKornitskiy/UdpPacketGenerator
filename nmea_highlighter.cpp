#include "nmea_highlighter.h"

NmeaHighlighter::NmeaHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    QTextCharFormat talkerFormat;
    talkerFormat.setForeground(QColor("#4ec9b0"));
    talkerFormat.setFontWeight(QFont::Bold);

    QTextCharFormat checksumFormat;
    checksumFormat.setForeground(QColor("#569cd6"));

    QTextCharFormat delimiterFormat;
    delimiterFormat.setForeground(QColor("#d4d4d4"));

    QTextCharFormat fieldFormat;
    fieldFormat.setForeground(QColor("#9cdcfe"));

    m_rules = {{QRegularExpression(R"(\$\w{2,5})"), talkerFormat},
               {QRegularExpression(R"(\*\w{2})"), checksumFormat},
               {QRegularExpression(R"(,)"), delimiterFormat},
               {QRegularExpression(R"([^,\*\$\r\n]+)"), fieldFormat}};

    m_errorFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    m_errorFormat.setUnderlineColor(Qt::red);
    m_errorFormat.setBackground(QColor(255, 0, 0, 40));
}

void NmeaHighlighter::setError(int line, const QString &message)
{
    m_errorLine = line;
    m_errorMessage = message;
    rehighlight();
}

void NmeaHighlighter::highlightBlock(const QString &text)
{
    if (currentBlock().blockNumber() + 1 == m_errorLine) {
        setFormat(0, text.length(), m_errorFormat);
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