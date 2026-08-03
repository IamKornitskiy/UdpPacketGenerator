#pragma once

#include <QRegularExpression>
#include <QSyntaxHighlighter>

class JsonHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit JsonHighlighter(QTextDocument *parent = nullptr);
    void setError(int line, const QString &message);

protected:
    void extracted(const QString &text);
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> m_rules;
    QTextCharFormat m_errorFormat;

    int m_errorLine = -1;
    QString m_errorMessage;
};
