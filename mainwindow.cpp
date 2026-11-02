#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "codeeditor.h"
#include "finddialog.h"
#include "replacedialog.h"

#include <QAction>
#include <QAbstractButton>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QColorDialog>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QPixmap>
#include <QSaveFile>
#include <QStatusBar>
#include <QStyle>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QToolBar>

namespace {

// 没有图标素材时，用“单字”绘制 24x24 图标：
// 统一风格、不依赖任何 png 资源，后续换成真实图标只需替换这一个函数。
QIcon glyphIcon(const QString &glyph)
{
    const int side = 24;
    const qreal dpr = 2.0;
    QPixmap pm(qRound(side * dpr), qRound(side * dpr));
    pm.setDevicePixelRatio(dpr);
    pm.fill(Qt::transparent);

    QPainter painter(&pm);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    QFont font = painter.font();
    font.setPointSizeF(11.5);
    painter.setFont(font);
    painter.setPen(QApplication::palette().color(QPalette::WindowText));
    painter.drawText(QRectF(0, 0, side, side), Qt::AlignCenter, glyph);
    return QIcon(pm);
}

// 单文件大小上限，防止误开超大文件把界面卡死
const qint64 kMaxFileSize = 20LL * 1024 * 1024;

// 作者信息（“关于”对话框显示）
const QString kAuthorName = QStringLiteral("林福佳");
const QString kAuthorId = QStringLiteral("2024414300110");
const QString kAuthorEmail = QStringLiteral("2749849788@qq.com");

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 正文留白：QPlainTextEdit 的“layoutMargins”实际由 documentMargin 控制
    ui->plainTextEdit->document()->setDocumentMargin(4);
    QFont editorFont = ui->plainTextEdit->font();
    editorFont.setPointSize(12);
    ui->plainTextEdit->setFont(editorFont);
    ui->plainTextEdit->document()->setDefaultFont(editorFont);

    applyIcons();
    createStatusBar();
    wireUp();

    m_findDlg = new FindDialog(this);
    m_replaceDlg = new ReplaceDialog(this);
    connect(m_findDlg, &FindDialog::findRequested, this, &MainWindow::findNext);
    connect(m_replaceDlg, &ReplaceDialog::findRequested, this, &MainWindow::findNext);
    connect(m_replaceDlg, &ReplaceDialog::replaceRequested, this, &MainWindow::replaceOne);
    connect(m_replaceDlg, &ReplaceDialog::replaceAllRequested, this, &MainWindow::replaceAllInDoc);

    setCurrentFile(QString());
    updateEditActions();
    updateStatusBar();
    statusBar()->showMessage(tr("就绪"), 2000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ---------------------------------------------------------------- 工具栏图标

void MainWindow::applyIcons()
{
    ui->actionNew->setIcon(glyphIcon(QStringLiteral("新")));
    ui->actionOpen->setIcon(glyphIcon(QStringLiteral("开")));
    ui->actionSave->setIcon(glyphIcon(QStringLiteral("存")));
    ui->actionSaveAs->setIcon(glyphIcon(QStringLiteral("另")));
    ui->actionExit->setIcon(glyphIcon(QStringLiteral("退")));

    ui->actionUndo->setIcon(glyphIcon(QStringLiteral("撤")));
    ui->actionRedo->setIcon(glyphIcon(QStringLiteral("恢")));
    ui->actionCut->setIcon(glyphIcon(QStringLiteral("剪")));
    ui->actionCopy->setIcon(glyphIcon(QStringLiteral("复")));
    ui->actionPaste->setIcon(glyphIcon(QStringLiteral("粘")));

    ui->actionFind->setIcon(glyphIcon(QStringLiteral("查")));
    ui->actionReplace->setIcon(glyphIcon(QStringLiteral("替")));

    ui->actionWrap->setIcon(glyphIcon(QStringLiteral("换")));
    ui->actionLineNumber->setIcon(glyphIcon(QStringLiteral("号")));

    ui->actionFont->setIcon(glyphIcon(QStringLiteral("字")));
    ui->actionFontColor->setIcon(glyphIcon(QStringLiteral("色")));
    ui->actionEditorBg->setIcon(glyphIcon(QStringLiteral("底")));

    ui->actionAbout->setIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));
}

// ------------------------------------------------------------------ 状态栏

void MainWindow::createStatusBar()
{
    m_lblInfo = new QLabel(this);
    m_lblPos = new QLabel(this);
    m_lblInfo->setMinimumWidth(200);
    m_lblPos->setMinimumWidth(150);

    statusBar()->addPermanentWidget(m_lblInfo);
    statusBar()->addPermanentWidget(m_lblPos);
}

// ------------------------------------------------------------------ 信号连接

void MainWindow::wireUp()
{
    // 文件
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave, &QAction::triggered, this, [this] { saveFile(); });
    connect(ui->actionSaveAs, &QAction::triggered, this, [this] { saveFileAs(); });
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    // 编辑：直接连到控件内建槽，保持 Action 与控件状态唯一同步
    connect(ui->actionUndo, &QAction::triggered, ui->plainTextEdit, &QPlainTextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered, ui->plainTextEdit, &QPlainTextEdit::redo);
    connect(ui->actionCut, &QAction::triggered, ui->plainTextEdit, &QPlainTextEdit::cut);
    connect(ui->actionCopy, &QAction::triggered, ui->plainTextEdit, &QPlainTextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered, ui->plainTextEdit, &QPlainTextEdit::paste);
    connect(ui->actionSelectAll, &QAction::triggered, ui->plainTextEdit, &QPlainTextEdit::selectAll);

    // 使能状态随编辑状态变化
    connect(ui->plainTextEdit, &QPlainTextEdit::copyAvailable,
            this, &MainWindow::updateEditActions);
    connect(ui->plainTextEdit, &QPlainTextEdit::undoAvailable,
            ui->actionUndo, &QAction::setEnabled);
    connect(ui->plainTextEdit, &QPlainTextEdit::redoAvailable,
            ui->actionRedo, &QAction::setEnabled);
    connect(QApplication::clipboard(), &QClipboard::dataChanged,
            this, &MainWindow::updatePasteAction);

    // 文档状态 -> 窗口标题
    connect(ui->plainTextEdit->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::setModifiedTitle);
    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged,
            this, &MainWindow::updateStatusBar);
    connect(ui->plainTextEdit, &QPlainTextEdit::cursorPositionChanged,
            this, &MainWindow::updateStatusBar);

    // 查找 / 替换
    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::showFindDialog);
    connect(ui->actionReplace, &QAction::triggered, this, &MainWindow::showReplaceDialog);

    // 格式 / 视图
    connect(ui->actionWrap, &QAction::toggled, this, &MainWindow::toggleWrap);
    connect(ui->actionLineNumber, &QAction::toggled, this, &MainWindow::toggleLineNumber);
    connect(ui->actionFont, &QAction::triggered, this, &MainWindow::chooseFont);
    connect(ui->actionFontColor, &QAction::triggered, this, &MainWindow::chooseFontColor);
    connect(ui->actionEditorBg, &QAction::triggered, this, &MainWindow::chooseEditorBackground);

    // 工具栏 / 状态栏显隐：菜单项与 toggleViewAction 双向同步，避免出现两套状态
    ui->actionToolBar->setCheckable(true);
    ui->actionToolBar->setChecked(true);
    connect(ui->actionToolBar, &QAction::toggled, ui->mainToolBar, &QWidget::setVisible);
    connect(ui->mainToolBar->toggleViewAction(), &QAction::toggled,
            ui->actionToolBar, &QAction::setChecked);

    // 注意：QStatusBar 没有 toggleViewAction()（只有 QToolBar / QDockWidget 有），
    // 所以这里直接连可见性，菜单项自身就是唯一入口，不会出现两套状态。
    ui->actionStatusBar->setCheckable(true);
    ui->actionStatusBar->setChecked(true);
    connect(ui->actionStatusBar, &QAction::toggled, statusBar(), &QWidget::setVisible);

    // 帮助
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAbout);
}

// -------------------------------------------------------------------- 文件

void MainWindow::newFile()
{
    if (!maybeSave())
        return;

    ui->plainTextEdit->clear();
    ui->plainTextEdit->document()->clearUndoRedoStacks();
    setCurrentFile(QString());
    ui->plainTextEdit->document()->setModified(false);
    updateEditActions();
    statusBar()->showMessage(tr("已新建文档"), 2000);
}

void MainWindow::openFile()
{
    if (!maybeSave())
        return;

    const QString path = QFileDialog::getOpenFileName(
        this, tr("打开文件"), QString(),
        tr("文本文件 (*.txt);;C++ 源文件 (*.cpp *.h);;所有文件 (*)"));
    if (path.isEmpty())
        return;

    QString text;
    if (!readFile(path, &text)) {
        // 关键：打开失败时编辑区内容保持不变
        statusBar()->showMessage(tr("打开失败：%1").arg(QDir::toNativeSeparators(path)), 4000);
        return;
    }

    ui->plainTextEdit->setPlainText(text);
    ui->plainTextEdit->document()->clearUndoRedoStacks();
    setCurrentFile(path);
    ui->plainTextEdit->document()->setModified(false);
    updateEditActions();
    statusBar()->showMessage(tr("已打开：%1").arg(QDir::toNativeSeparators(path)), 3000);
}

bool MainWindow::saveFile()
{
    if (m_filePath.isEmpty())
        return saveFileAs();
    return writeFile(m_filePath, ui->plainTextEdit->toPlainText());
}

bool MainWindow::saveFileAs()
{
    const QString defaultPath = m_filePath.isEmpty()
                                    ? QDir::homePath() + QLatin1Char('/') + displayName() + QStringLiteral(".txt")
                                    : m_filePath;
    const QString path = QFileDialog::getSaveFileName(
        this, tr("另存为"), defaultPath,
        tr("文本文件 (*.txt);;所有文件 (*)"));
    if (path.isEmpty())
        return false; // 用户取消：不改任何状态

    if (!writeFile(path, ui->plainTextEdit->toPlainText()))
        return false; // 写失败：保持 modified 标记

    setCurrentFile(path);
    return true;
}

bool MainWindow::readFile(const QString &path, QString *text)
{
    const QFileInfo info(path);
    if (!info.exists() || !info.isFile()) {
        QMessageBox::warning(this, tr("打开失败"),
                             tr("文件不存在：\n%1").arg(QDir::toNativeSeparators(path)));
        return false;
    }
    if (info.size() > kMaxFileSize) {
        QMessageBox::warning(this, tr("打开失败"),
                             tr("文件过大（%1 MB），本编辑器仅支持 %2 MB 以内的文本文件。")
                                 .arg(QString::number(double(info.size()) / (1024.0 * 1024.0), 'f', 1))
                                 .arg(kMaxFileSize / (1024 * 1024)));
        return false;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("打开失败"),
                             tr("无法读取文件：\n%1\n\n%2")
                                 .arg(QDir::toNativeSeparators(path), file.errorString()));
        return false;
    }
    const QByteArray raw = file.readAll();
    file.close();

    if (raw.contains('\0')) {
        QMessageBox::warning(this, tr("打开失败"),
                             tr("该文件疑似二进制文件，无法作为文本打开。"));
        return false;
    }

    QString content = QString::fromUtf8(raw);
    if (content.startsWith(QChar(0xFEFF)))
        content.remove(0, 1);      // 去掉 UTF-8 BOM
    content.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
    content.replace(QLatin1Char('\r'), QLatin1Char('\n'));

    *text = content;
    return true;
}

bool MainWindow::writeFile(const QString &path, const QString &text)
{
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("保存失败"),
                             tr("无法写入文件：\n%1\n\n%2")
                                 .arg(QDir::toNativeSeparators(path), file.errorString()));
        return false;
    }

    QByteArray data = text.toUtf8();
#ifdef Q_OS_WIN
    // Windows 下写成 CRLF，保证记事本等工具显示正常
    data.replace("\r\n", "\n");
    data.replace("\n", "\r\n");
#endif

    if (file.write(data) != data.size()) {
        QMessageBox::warning(this, tr("保存失败"),
                             tr("写入文件时出错：\n%1\n\n%2")
                                 .arg(QDir::toNativeSeparators(path), file.errorString()));
        file.cancelWriting();
        return false;
    }
    if (!file.commit()) {
        QMessageBox::warning(this, tr("保存失败"),
                             tr("提交文件时出错：\n%1\n\n%2")
                                 .arg(QDir::toNativeSeparators(path), file.errorString()));
        return false;
    }

    ui->plainTextEdit->document()->setModified(false);
    statusBar()->showMessage(tr("已保存：%1").arg(QDir::toNativeSeparators(path)), 3000);
    return true;
}

bool MainWindow::maybeSave()
{
    if (!ui->plainTextEdit->document()->isModified())
        return true;

    // 用标准按钮（便于测试与键盘操作），但把文字改成中文：
    // 不依赖 Qt 的 .qm 翻译包，任何机器上都显示“保存 / 不保存 / 取消”。
    QMessageBox box(QMessageBox::Warning, tr("提示"),
                    tr("“%1” 已被修改，是否保存？").arg(displayName()),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                    this);
    if (QAbstractButton *button = box.button(QMessageBox::Save))
        button->setText(tr("保存(&S)"));
    if (QAbstractButton *button = box.button(QMessageBox::Discard))
        button->setText(tr("不保存(&D)"));
    if (QAbstractButton *button = box.button(QMessageBox::Cancel))
        button->setText(tr("取消(&C)"));
    box.setDefaultButton(QMessageBox::Save);
    box.setEscapeButton(QMessageBox::Cancel);

    const int ret = box.exec();

    if (ret == QMessageBox::Save)
        return saveFile();      // 保存失败则不允许继续（不关闭 / 不覆盖）
    if (ret == QMessageBox::Discard)
        return true;
    return false;               // Cancel
}

void MainWindow::setCurrentFile(const QString &path)
{
    m_filePath = path;
    setModifiedTitle(ui->plainTextEdit->document()->isModified());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

QString MainWindow::displayName() const
{
    if (m_filePath.isEmpty())
        return tr("新建文本文件");
    return QFileInfo(m_filePath).fileName();
}

// ------------------------------------------------------------------ 标题 / 状态

void MainWindow::setModifiedTitle(bool modified)
{
    setWindowModified(modified);
    setWindowTitle(tr("%1[*] - 编辑器").arg(displayName()));
}

void MainWindow::updateStatusBar()
{
    const QTextCursor cursor = ui->plainTextEdit->textCursor();
    // 字符数：QTextDocument::characterCount 把每个块的分隔符也算进去了，减掉即可
    const int chars = ui->plainTextEdit->document()->characterCount()
                      - ui->plainTextEdit->blockCount();
    m_lblInfo->setText(tr("length: %1    lines: %2")
                           .arg(qMax(0, chars))
                           .arg(ui->plainTextEdit->blockCount()));
    m_lblPos->setText(tr("Ln: %1    Col: %2")
                          .arg(cursor.blockNumber() + 1)
                          .arg(cursor.positionInBlock() + 1));
}

void MainWindow::updatePasteAction()
{
    const QMimeData *mime = QApplication::clipboard()->mimeData();
    ui->actionPaste->setEnabled(mime && mime->hasText()
                                && !ui->plainTextEdit->isReadOnly());
}

void MainWindow::updateEditActions()
{
    const bool hasSelection = ui->plainTextEdit->textCursor().hasSelection();
    ui->actionCut->setEnabled(hasSelection);
    ui->actionCopy->setEnabled(hasSelection);
    ui->actionUndo->setEnabled(ui->plainTextEdit->document()->isUndoAvailable());
    ui->actionRedo->setEnabled(ui->plainTextEdit->document()->isRedoAvailable());
    ui->actionSelectAll->setEnabled(!ui->plainTextEdit->document()->isEmpty());
    updatePasteAction();
}

// ------------------------------------------------------------------ 查找 / 替换

void MainWindow::showFindDialog()
{
    m_findDlg->show();
    m_findDlg->raise();
    m_findDlg->activateWindow();
    m_findDlg->setFocusOnEdit();
    if (ui->plainTextEdit->textCursor().hasSelection()) {
        // selectedText() 用 U+2029 表示段落分隔，先统一成 \n 再判断是否单行
        QString selected = ui->plainTextEdit->textCursor().selectedText();
        selected.replace(QChar::ParagraphSeparator, QLatin1Char('\n'));
        if (!selected.contains(QLatin1Char('\n')) && selected.length() <= 200)
            m_findDlg->setSearchText(selected);
    }
}

void MainWindow::showReplaceDialog()
{
    m_replaceDlg->show();
    m_replaceDlg->raise();
    m_replaceDlg->activateWindow();
    m_replaceDlg->setFocusOnEdit();
    if (ui->plainTextEdit->textCursor().hasSelection()) {
        QString selected = ui->plainTextEdit->textCursor().selectedText();
        selected.replace(QChar::ParagraphSeparator, QLatin1Char('\n'));
        if (!selected.contains(QLatin1Char('\n')) && selected.length() <= 200)
            m_replaceDlg->setSearchText(selected);
    }
}

void MainWindow::findNext(const QString &text, bool caseSensitive, bool backward)
{
    if (text.isEmpty())
        return;
    findFrom(text, caseSensitive, backward, true);
}

bool MainWindow::findFrom(const QString &needle, bool caseSensitive,
                          bool backward, bool wrap)
{
    if (needle.isEmpty())
        return false;

    QTextDocument *doc = ui->plainTextEdit->document();
    QTextDocument::FindFlags flags;
    if (caseSensitive)
        flags |= QTextDocument::FindCaseSensitively;
    if (backward)
        flags |= QTextDocument::FindBackward;

    QTextCursor start = ui->plainTextEdit->textCursor();
    if (start.hasSelection()) {
        // 从选区边界开始，否则会在原地重复命中
        start.setPosition(backward ? start.selectionStart() : start.selectionEnd());
    }

    QTextCursor hit = doc->find(needle, start, flags);
    if (hit.isNull() && wrap) {
        // 回绕：从头（或尾）再找一次
        QTextCursor edge(doc);
        edge.movePosition(backward ? QTextCursor::End : QTextCursor::Start);
        hit = doc->find(needle, edge, flags);
        if (!hit.isNull()) {
            statusBar()->showMessage(backward ? tr("已从文末继续查找")
                                              : tr("已从文首继续查找"), 2000);
        }
    }

    if (hit.isNull()) {
        QMessageBox::information(this, tr("查找"), tr("未找到“%1”。").arg(needle));
        return false;
    }

    ui->plainTextEdit->setTextCursor(hit);
    ui->plainTextEdit->centerCursor();
    return true;
}

void MainWindow::replaceOne(const QString &text, const QString &with,
                            bool caseSensitive, bool backward)
{
    if (text.isEmpty())
        return;

    const Qt::CaseSensitivity cs = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    QTextCursor cursor = ui->plainTextEdit->textCursor();
    if (cursor.hasSelection() && cursor.selectedText().compare(text, cs) == 0) {
        cursor.insertText(with);
        ui->plainTextEdit->setTextCursor(cursor);
    }
    findFrom(text, caseSensitive, backward, true);
}

void MainWindow::replaceAllInDoc(const QString &text, const QString &with,
                                 bool caseSensitive)
{
    if (text.isEmpty())
        return;

    QTextDocument *doc = ui->plainTextEdit->document();
    QTextDocument::FindFlags flags;
    if (caseSensitive)
        flags |= QTextDocument::FindCaseSensitively;

    QTextCursor cursor(doc);
    int count = 0;
    cursor.beginEditBlock();          // 一次 Ctrl+Z 即可整体回退
    for (;;) {
        QTextCursor hit = doc->find(text, cursor, flags);
        if (hit.isNull())
            break;

        const int oldPos = hit.selectionStart();
        hit.insertText(with);
        ++count;

        // 替换为空串时光标不前进，会死循环，强制前移一个字符
        if (hit.position() <= oldPos)
            hit.movePosition(QTextCursor::NextCharacter);

        cursor = hit;
    }
    cursor.endEditBlock();

    if (count == 0) {
        QMessageBox::information(this, tr("替换"),
                                 tr("未找到“%1”。").arg(text));
    } else {
        QMessageBox::information(this, tr("替换"),
                                 tr("共替换 %1 处。").arg(count));
    }
    updateStatusBar();
}

// ------------------------------------------------------------------ 格式 / 视图

void MainWindow::chooseFont()
{
    bool ok = false;
    const QFont font = QFontDialog::getFont(&ok, ui->plainTextEdit->font(),
                                            this, tr("选择字体（字号也在其中）"));
    if (!ok)
        return;

    ui->plainTextEdit->setFont(font);
    ui->plainTextEdit->document()->setDefaultFont(font);
    statusBar()->showMessage(tr("字体：%1  %2pt").arg(font.family()).arg(font.pointSize()), 3000);
}

void MainWindow::chooseFontColor()
{
    // QPlainTextEdit 没有 textColor()（那是 QTextEdit），从当前字符格式里取
    QColor current = ui->plainTextEdit->currentCharFormat().foreground().color();
    if (!current.isValid())
        current = ui->plainTextEdit->palette().color(QPalette::Text);

    const QColor color = QColorDialog::getColor(current, this, tr("字体颜色"));
    if (!color.isValid())
        return;

    QTextCharFormat format;
    format.setForeground(color);
    ui->plainTextEdit->mergeCurrentCharFormat(format);
    statusBar()->showMessage(tr("字体颜色已应用"), 2000);
}

void MainWindow::chooseEditorBackground()
{
    const QColor color = QColorDialog::getColor(
        ui->plainTextEdit->palette().color(QPalette::Base), this, tr("编辑器背景色"));
    if (!color.isValid())
        return;

    QPalette pal = ui->plainTextEdit->palette();
    pal.setColor(QPalette::Base, color);
    ui->plainTextEdit->setPalette(pal);
    ui->plainTextEdit->viewport()->update();
    statusBar()->showMessage(tr("编辑器背景色已修改"), 2000);
}

void MainWindow::toggleLineNumber(bool checked)
{
    ui->plainTextEdit->setLineNumberVisible(checked);
}

void MainWindow::toggleWrap(bool checked)
{
    ui->plainTextEdit->setLineWrapMode(checked ? QPlainTextEdit::WidgetWidth
                                               : QPlainTextEdit::NoWrap);
}

// ---------------------------------------------------------------------- 帮助

void MainWindow::showAbout()
{
    QMessageBox::about(
        this, tr("关于"),
        tr("<h3>小牛文本编辑器</h3>"
           "<p>Qt 应用程序开发 &middot; 实验二</p>"
           "<p>开发者：%1<br/>学号：%2<br/>邮箱：%3</p>"
           "<p>版本：V1.0 build 20261026<br/>基于 Qt %4</p>")
            .arg(kAuthorName, kAuthorId, kAuthorEmail, QString::fromLatin1(qVersion())));
}
