#ifndef TEXT_FIELD_EDITOR_DIALOG_H
#define TEXT_FIELD_EDITOR_DIALOG_H

#include <QColor>
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

private slots:
    void on_plainTextEdit_textChanged();

    void on_applyButton_clicked();

private:
    Ui::TextFieldEditorDialog *ui;
    QString m_type;

signals:
    void sendNewText(const QString &text);
};

#endif // TEXT_FIELD_EDITOR_DIALOG_H
