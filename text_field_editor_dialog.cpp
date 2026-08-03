#include "text_field_editor_dialog.h"
#include <QStyle>
#include "json_highlighter.h"
#include "json_packet_field.h"
#include "nmea_highlighter.h"
#include "nmea_packet_field.h"
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

    ui->errorLabel->setStyleSheet("color: #ff4444; font-weight: bold;");
    ui->errorLabel->setVisible(false);

    QObject::connect(ui->applyButton, &QPushButton::clicked, this, &TextFieldEditorDialog::accept);
    QObject::connect(ui->closeButton, &QPushButton::clicked, this, &TextFieldEditorDialog::reject);
    QObject::connect(ui->plainTextEdit,
                     &QPlainTextEdit::textChanged,
                     this,
                     &TextFieldEditorDialog::onTextChanged);
}

TextFieldEditorDialog::~TextFieldEditorDialog()
{
    delete ui;
}

QString TextFieldEditorDialog::plainText() const
{
    return ui->plainTextEdit->toPlainText();
}

void TextFieldEditorDialog::setValidator(std::function<bool(const QString &, QString *)> validator)
{
    m_validator = std::move(validator);
}

void TextFieldEditorDialog::setHighlighter(QSyntaxHighlighter *highlighter)
{
    delete m_highlighter;
    m_highlighter = highlighter;
    if (m_highlighter) {
        m_highlighter->setDocument(ui->plainTextEdit->document());
    }
}

void TextFieldEditorDialog::onTextChanged()
{
    if (m_updating || !m_validator || !m_highlighter)
        return;
    m_updating = true;

    QSignalBlocker blocker(ui->plainTextEdit);
    QString text = ui->plainTextEdit->toPlainText();
    QString errorMsg;
    bool valid = m_validator(text, &errorMsg);

    ui->plainTextEdit->setProperty("error", !valid);
    ui->plainTextEdit->style()->unpolish(ui->plainTextEdit);
    ui->plainTextEdit->style()->polish(ui->plainTextEdit);

    if (auto *nmeaHL = dynamic_cast<NmeaHighlighter *>(m_highlighter)) {
        auto err = NmeaPacketField::isValid(text);
        if (err) {
            nmeaHL->setError(err.line, err.message);
            ui->errorLabel->setText(
                QString("⚠ Line %1, Col %2: %3").arg(err.line).arg(err.column).arg(err.message));
            ui->errorLabel->setVisible(true);
        } else {
            nmeaHL->setError(-1, QString());
            ui->errorLabel->setVisible(false);
        }
    }

    if (auto *jsonHL = dynamic_cast<JsonHighlighter *>(m_highlighter)) {
        if (!valid) {
            auto validationError = JsonPacketField::isValid(text);
            if (validationError.offset != -1) {
                jsonHL->setError(validationError.line, validationError.message);
                ui->errorLabel->setText(QString("Line %1, Col %2: %3")
                                            .arg(validationError.line)
                                            .arg(validationError.column)
                                            .arg(validationError.message));
                ui->errorLabel->setVisible(true);
            }
        } else {
            jsonHL->setError(-1, QString());
            ui->errorLabel->clear();
            ui->errorLabel->setVisible(false);
        }
    }

    m_updating = false;
}
