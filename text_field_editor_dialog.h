#ifndef TEXT_FIELD_EDITOR_DIALOG_H
#define TEXT_FIELD_EDITOR_DIALOG_H

#include <QDialog>

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

private slots:
    void on_plainTextEdit_textChanged();

private:
    Ui::TextFieldEditorDialog *ui;
    QString m_type;
};

#endif // TEXT_FIELD_EDITOR_DIALOG_H
