#ifndef LOGEDIT_H
#define LOGEDIT_H

#include <QObject>
#include <QWidget>
#include <QPlainTextEdit>
#include <QShortcut>
#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include "FocusManager.h"
#include "ilog.h"
#include "highlight.h"




class LineNum;

class LogEdit :  public QPlainTextEdit//, public AbstractNavPosReplayer
{
    Q_OBJECT
public:
    LogEdit(FocusManager* focusManager);
    void setExternalVScrollbar(QScrollBar *bar);
    void setLog(std::shared_ptr<ILog> log);
    std::shared_ptr<ILog> getLog(){return mLog;};
    void onLogRangeChanged();
    void onLogEmphasized(int lineNum);
    int find(SearchArg arg, bool forward);
    void scrollToLine(int line,int col=0);
    void onLogFindOne(SearchResult ret);
    Highlighter* highlighter()
    {
        return mHighlighter.get();
    }
protected:
    virtual bool event(QEvent *ev) override;
    void resizeEvent(QResizeEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    //void paintEvent(QPaintEvent *) override;

private:
    friend class LineNum;
    int lineNumWidth();
    void paintLineNum(QPaintEvent* event);
    void updateLineNum(const QRect &rect, int dy);
    void updateLineNumWidth(int newBlockCount = 0);
    void highlightCurrentLine();

signals:
    void menuRequested(QPoint point, const QString& cursorWord, int lineNum);
    void emphasizeLineRequested(int lineNum);
    void addToTimeLineRequested(int lineNum);

private:
    void load(int preloadCenterLine, int firstVisibleLineNumber=-1);
    void bindExternalScroll();
    void unBindExternalScroll();
    void bindInternalScroll();
    void unBindInternalScroll();
    void updateExternalScrollRange(Range range);
    void setVertialScrollbarByBlockNum(int blockNumber);
    int presenterNumberToBlock(int lineNumber);
    int blockNumberToPresenter(int blockNumber);
    void handleInternalScroll(int);
    void handleExternalScroll(int value);

private:
    bool mResize{false};
    //FocusManager* mfocusManager;
    int mViewPortMaxLineCnt{0};
    QScrollBar* mExternalBar{nullptr};

    QMetaObject::Connection mInternalVBarHandler;
    bool mResizing{false};
    QMetaObject::Connection mExternalVBarHandler;

    Range mBufferRange;

    LineNum* mLineNum{nullptr};
    std::shared_ptr<ILog> mLog{nullptr};
    FocusManager* mFocusManager;
    std::unique_ptr<Highlighter> mHighlighter;

};

class LineNum : public QWidget
{
public:
    LineNum(LogEdit *editor) : QWidget(editor), logEditor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(logEditor->lineNumWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        logEditor->paintLineNum(event);
    }

private:
    LogEdit *logEditor;
};

#endif // LOGEDIT_H
