#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QLabel;
class FindDialog;
class ReplaceDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 文件
    void newFile();
    void openFile();
    bool saveFile();
    bool saveFileAs();
    // 状态
    void setModifiedTitle(bool modified);
    void updateStatusBar();
    void updatePasteAction();
    void updateEditActions();
    // 查找 / 替换
    void showFindDialog();
    void showReplaceDialog();
    void findNext(const QString &text, bool caseSensitive, bool backward);
    void replaceOne(const QString &text, const QString &with,
                    bool caseSensitive, bool backward);
    void replaceAllInDoc(const QString &text, const QString &with, bool caseSensitive);
    // 格式 / 视图
    void chooseFont();
    void chooseFontColor();
    void chooseEditorBackground();
    void toggleLineNumber(bool checked);
    void toggleWrap(bool checked);
    // 帮助
    void showAbout();

private:
    bool maybeSave();
    bool readFile(const QString &path, QString *text);
    bool writeFile(const QString &path, const QString &text);
    void setCurrentFile(const QString &path);
    QString displayName() const;
    bool findFrom(const QString &needle, bool caseSensitive, bool backward, bool wrap);

    void wireUp();
    void applyIcons();
    void createStatusBar();

    Ui::MainWindow *ui;
    QString m_filePath;          // 空字符串表示“未命名”
    QLabel *m_lblInfo = nullptr; // length / lines
    QLabel *m_lblPos = nullptr;  // Ln / Col
    FindDialog *m_findDlg = nullptr;
    ReplaceDialog *m_replaceDlg = nullptr;
};

#endif // MAINWINDOW_H
