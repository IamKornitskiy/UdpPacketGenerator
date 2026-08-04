#ifndef TEXT_FIELD_EDITOR_DIALOG_H
#define TEXT_FIELD_EDITOR_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QSyntaxHighlighter>

namespace Ui {
class TextFieldEditorDialog;
}

class TextFieldEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextFieldEditorDialog(const QString &name,
                                   const QString &type,
                                   const QString &text,
                                   QWidget *parent = nullptr);
    ~TextFieldEditorDialog();

    QString plainText() const;
    void setValidator(std::function<bool(const QString &, QString *)> validator);
    void setHighlighter(QSyntaxHighlighter *highlighter);

private:
    Ui::TextFieldEditorDialog *ui;
    QString m_type;
    bool m_updating = false;

    std::function<bool(const QString &, QString *)> m_validator;
    QSyntaxHighlighter *m_highlighter = nullptr;
    void onTextChanged();
};

#endif // TEXT_FIELD_EDITOR_DIALOG_H
