#include "finddialog.h"
#include "ui_finddialog.h"

#include <QLineEdit>
#include <QPushButton>

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindDialog)
{
    ui->setupUi(this);

    setModal(false);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    ui->rbDown->setChecked(true);
    ui->btnFind->setDefault(true);

    connect(ui->btnFind, &QPushButton::clicked, this, &FindDialog::doFind);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::hide);
    connect(ui->editFind, &QLineEdit::returnPressed, this, &FindDialog::doFind);
}

FindDialog::~FindDialog()
{
    delete ui;
}

void FindDialog::setSearchText(const QString &text)
{
    ui->editFind->setText(text);
    ui->editFind->selectAll();
}

void FindDialog::setFocusOnEdit()
{
    ui->editFind->setFocus(Qt::OtherFocusReason);
    ui->editFind->selectAll();
}

void FindDialog::doFind()
{
    const QString text = ui->editFind->text();
    if (text.isEmpty()) {
        ui->editFind->setFocus(Qt::OtherFocusReason);
        return;
    }
    emit findRequested(text, ui->chkCase->isChecked(), ui->rbUp->isChecked());
}
