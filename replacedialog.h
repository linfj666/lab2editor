#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class ReplaceDialog;
}
QT_END_NAMESPACE

// 替换对话框：非模态，只发信号，具体查找/替换算法由主窗口实现。
class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);
    ~ReplaceDialog() override;

    void setSearchText(const QString &text);
    void setFocusOnEdit();

signals:
    void findRequested(const QString &text, bool caseSensitive, bool backward);
    void replaceRequested(const QString &text, const QString &with,
                          bool caseSensitive, bool backward);
    void replaceAllRequested(const QString &text, const QString &with, bool caseSensitive);

private slots:
    void doFind();
    void doReplace();
    void doReplaceAll();

private:
    QString searchText() const;
    bool caseSensitive() const;
    bool backward() const;

    Ui::ReplaceDialog *ui;
};

#endif // REPLACEDIALOG_H
