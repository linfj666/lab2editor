#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class FindDialog;
}
QT_END_NAMESPACE

// 查找对话框：非模态，只负责收集输入并通过信号请求主窗口执行查找。
class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog() override;

    void setSearchText(const QString &text);
    void setFocusOnEdit();

signals:
    void findRequested(const QString &text, bool caseSensitive, bool backward);

private slots:
    void doFind();

private:
    Ui::FindDialog *ui;
};

#endif // FINDDIALOG_H
