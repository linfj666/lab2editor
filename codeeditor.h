#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class CodeEditor;

// 行号区：只负责把绘制工作转交给 CodeEditor，自身不保存任何状态。
class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    CodeEditor *m_editor;
};

// 从 QPlainTextEdit 派生的编辑器，在左侧绘制行号。
// 通过 Qt Designer 的“提升为(promote)”把 plainTextEdit 变成这个类。
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    // 行号区宽度（按当前最大行号的位数计算）
    int lineNumberAreaWidth() const;

    // 供 LineNumberArea 在 paintEvent 中调用
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    void setLineNumberVisible(bool visible);
    bool isLineNumberVisible() const { return m_showLineNumber; }

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *m_lineNumberArea = nullptr;
    bool m_showLineNumber = true;
};

#endif // CODEEDITOR_H
