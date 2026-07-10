#include "text_field_editor_dialog.h"
#include <QStyle>
#include "json_packet_field.h"
#include "ui_text_field_editor_dialog.h"

TextFieldEditorDialog::TextFieldEditorDialog(const QString &name,
                                             const QString &type,
                                             const QString &text,
                                             QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TextFieldEditorDialog)
    , m_type(type)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    ui->head->setText(QString("Name: %1   |   Type: %2").arg(name, type));
    ui->plainTextEdit->setPlainText(text);
}

TextFieldEditorDialog::~TextFieldEditorDialog()
{
    delete ui;
}

void TextFieldEditorDialog::on_plainTextEdit_textChanged()
{
    if (m_type == "json") {
        auto err = JsonPacketField::isValid(ui->plainTextEdit->toPlainText());
        ui->plainTextEdit->setProperty("error", err.has_value());
        ui->plainTextEdit->style()->unpolish(ui->plainTextEdit);
        ui->plainTextEdit->style()->polish(ui->plainTextEdit);
    }
}

void TextFieldEditorDialog::on_applyButton_clicked()
{
    emit sendNewText(ui->plainTextEdit->toPlainText());
}
