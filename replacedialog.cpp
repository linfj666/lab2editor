#include "replacedialog.h"
#include "ui_replacedialog.h"

#include <QLineEdit>
#include <QPushButton>

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);

    setModal(false);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    ui->rbDown->setChecked(true);
    ui->btnFind->setDefault(true);

    connect(ui->btnFind, &QPushButton::clicked, this, &ReplaceDialog::doFind);
    connect(ui->btnReplace, &QPushButton::clicked, this, &ReplaceDialog::doReplace);
    connect(ui->btnReplaceAll, &QPushButton::clicked, this, &ReplaceDialog::doReplaceAll);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::hide);
    connect(ui->editFind, &QLineEdit::returnPressed, this, &ReplaceDialog::doFind);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

void ReplaceDialog::setSearchText(const QString &text)
{
    ui->editFind->setText(text);
    ui->editFind->selectAll();
}

void ReplaceDialog::setFocusOnEdit()
{
    ui->editFind->setFocus(Qt::OtherFocusReason);
    ui->editFind->selectAll();
}

QString ReplaceDialog::searchText() const
{
    return ui->editFind->text();
}

bool ReplaceDialog::caseSensitive() const
{
    return ui->chkCase->isChecked();
}

bool ReplaceDialog::backward() const
{
    return ui->rbUp->isChecked();
}

void ReplaceDialog::doFind()
{
    if (searchText().isEmpty()) {
        ui->editFind->setFocus(Qt::OtherFocusReason);
        return;
    }
    emit findRequested(searchText(), caseSensitive(), backward());
}

void ReplaceDialog::doReplace()
{
    if (searchText().isEmpty()) {
        ui->editFind->setFocus(Qt::OtherFocusReason);
        return;
    }
    emit replaceRequested(searchText(), ui->editReplace->text(),
                          caseSensitive(), backward());
}

void ReplaceDialog::doReplaceAll()
{
    if (searchText().isEmpty()) {
        ui->editFind->setFocus(Qt::OtherFocusReason);
        return;
    }
    emit replaceAllRequested(searchText(), ui->editReplace->text(), caseSensitive());
}
