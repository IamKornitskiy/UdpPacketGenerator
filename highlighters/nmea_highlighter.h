#pragma once
#include <QRegularExpression>
#include <QSyntaxHighlighter>

class NmeaHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit NmeaHighlighter(QTextDocument *parent = nullptr);
    void setError(int line, const QString &message);

protected:
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
