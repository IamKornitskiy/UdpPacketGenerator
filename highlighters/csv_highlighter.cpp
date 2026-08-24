#include "csv_highlighter.h"

CsvHighlighter::CsvHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    QTextCharFormat quotedFormat;
    quotedFormat.setForeground(QColor("#ce9178")); // orange

    QTextCharFormat delimiterFormat;
    delimiterFormat.setForeground(QColor("#808080")); // gray

    QTextCharFormat fieldFormat;
    fieldFormat.setForeground(QColor("#9cdcfe")); // light blue

    m_rules = {
        {QRegularExpression(R"("[^"]*") quotedFormat},
        {QRegularExpression(R"(,)"), delimiterFormat},
        {QRegularExpression(R"((?:^|,)([^",]+))"), fieldFormat}
    };

    m_errorFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    m_errorFormat.setUnderlineColor(Qt::red);
    m_errorFormat.setBackground(QColor(255, 0, 0, 40));
}

void CsvHighlighter::setError(int line, const QString &message)
{
    m_errorLine = line;
    m_errorMessage = message;
    rehighlight();
}

void CsvHighlighter::highlightBlock(const QString &text)
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
