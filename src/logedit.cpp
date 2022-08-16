#include <QScrollBar>
#include <QTextBlock>
#include <QPainter>
#include "logedit.h"

static const int PRELOAD_COUNT = 2560;

//static void fillBackground(QPainter *p, const QRectF &rect, QBrush brush, const QRectF &gradientRect = QRectF())
//{
//    p->save();
//    if (brush.style() >= Qt::LinearGradientPattern && brush.style() <= Qt::ConicalGradientPattern) {
//        if (!gradientRect.isNull()) {
//            QTransform m = QTransform::fromTranslate(gradientRect.left(), gradientRect.top());
//            m.scale(gradientRect.width(), gradientRect.height());
//            brush.setTransform(m);
//            const_cast<QGradient *>(brush.gradient())->setCoordinateMode(QGradient::LogicalMode);
//        }
//    } else {
//        p->setBrushOrigin(rect.topLeft());
//    }
//    p->fillRect(rect, brush);
//    p->restore();
//}


//void LogEdit::paintEvent(QPaintEvent *e)
//{
//    //Q_D(QPlainTextEdit);
//    QPainter painter(viewport());
//    Q_ASSERT(qobject_cast<QPlainTextDocumentLayout*>(document()->documentLayout()));

//    QPointF offset(contentOffset());

//    QRect er = e->rect();
//    QRect viewportRect = viewport()->rect();

//    bool editable = !isReadOnly();

//    QTextBlock block = firstVisibleBlock();
//    qreal maximumWidth = document()->documentLayout()->documentSize().width();

//    // Set a brush origin so that the WaveUnderline knows where the wave started
//    painter.setBrushOrigin(offset);

//    // keep right margin clean from full-width selection
//    int maxX = offset.x() + qMax((qreal)viewportRect.width(), maximumWidth)
//               - document()->documentMargin();
//    er.setRight(qMin(er.right(), maxX));
//    painter.setClipRect(er);

////    if (d->placeholderVisible) {
////        const QColor col = d->control->palette().placeholderText().color();
////        painter.setPen(col);
////        painter.setClipRect(e->rect());
////        const int margin = int(document()->documentMargin());
////        QRectF textRect = viewportRect.adjusted(margin, margin, 0, 0);
////        painter.drawText(textRect, Qt::AlignTop | Qt::TextWordWrap, placeholderText());
////    }

//    QAbstractTextDocumentLayout::PaintContext context = getPaintContext();
//    painter.setPen(context.palette.text().color());

//    while (block.isValid()) {

//        QRectF r = blockBoundingRect(block).translated(offset);
//        QTextLayout *layout = block.layout();

//        if (!block.isVisible()) {
//            offset.ry() += r.height();
//            block = block.next();
//            continue;
//        }

//        if (r.bottom() >= er.top() && r.top() <= er.bottom()) {

//            QTextBlockFormat blockFormat = block.blockFormat();

//            QBrush bg = blockFormat.background();
//            if (bg != Qt::NoBrush) {
//                QRectF contentsRect = r;
//                contentsRect.setWidth(qMax(r.width(), maximumWidth));
//                fillBackground(&painter, contentsRect, bg);
//            }


//            QVector<QTextLayout::FormatRange> selections;
//            int blpos = block.position();
//            int bllen = block.length();
//            for (int i = 0; i < context.selections.size(); ++i) {
//                const QAbstractTextDocumentLayout::Selection &range = context.selections.at(i);
//                const int selStart = range.cursor.selectionStart() - blpos;
//                const int selEnd = range.cursor.selectionEnd() - blpos;
//                if (selStart < bllen && selEnd > 0
//                    && selEnd > selStart) {
//                    QTextLayout::FormatRange o;
//                    o.start = selStart;
//                    o.length = selEnd - selStart;
//                    o.format = range.cursor.blockCharFormat();
//                    o.format.setBackground(QColor(Qt::gray).lighter());
//                    selections.append(o);
//                } else if (!range.cursor.hasSelection() && range.format.hasProperty(QTextFormat::FullWidthSelection)
//                           && block.contains(range.cursor.position())) {
//                    // for full width selections we don't require an actual selection, just
//                    // a position to specify the line. that's more convenience in usage.
//                    QTextLayout::FormatRange o;
//                    QTextLine l = layout->lineForTextPosition(range.cursor.position() - blpos);
//                    o.start = l.textStart();
//                    o.length = l.textLength();
//                    if (o.start + o.length == bllen - 1)
//                        ++o.length; // include newline
//                    o.format = range.cursor.blockCharFormat();
//                    o.format.setBackground(QColor(Qt::gray).lighter());
//                    selections.append(o);
//                }
//            }

//            bool drawCursor = ((editable || (textInteractionFlags() & Qt::TextSelectableByKeyboard))
//                               && context.cursorPosition >= blpos
//                               && context.cursorPosition < blpos + bllen);

//            bool drawCursorAsBlock = drawCursor && overwriteMode() ;

//            if (drawCursorAsBlock) {
//                if (context.cursorPosition == blpos + bllen - 1) {
//                    drawCursorAsBlock = false;
//                } else {
//                    QTextLayout::FormatRange o;
//                    o.start = context.cursorPosition - blpos;
//                    o.length = 1;
//                    o.format.setForeground(palette().base());
//                    o.format.setBackground(palette().text());
//                    selections.append(o);
//                }
//            }

//            layout->draw(&painter, offset, selections, er);

//            if ((drawCursor && !drawCursorAsBlock)
//                || (editable && context.cursorPosition < -1
//                    && !layout->preeditAreaText().isEmpty())) {
//                int cpos = context.cursorPosition;
//                if (cpos < -1)
//                    cpos = layout->preeditAreaPosition() - (cpos + 2);
//                else
//                    cpos -= blpos;
//                layout->drawCursor(&painter, offset, cpos, cursorWidth());
//            }
//        }

//        offset.ry() += r.height();
//        if (offset.y() > viewportRect.height())
//            break;
//        block = block.next();
//    }

//    if (backgroundVisible() && !block.isValid() && offset.y() <= er.bottom()
//        && (centerOnScroll() || verticalScrollBar()->maximum() == verticalScrollBar()->minimum())) {
//        painter.fillRect(QRect(QPoint((int)er.left(), (int)offset.y()), er.bottomRight()), palette().window());
//    }
//}
LogEdit::LogEdit(FocusManager* focusManager)
{
    mFocusManager = focusManager;
    mLineNum = new LineNum(this);
    mHighlighter.reset(new Highlighter(document()));
    connect(this, &QPlainTextEdit::textChanged, this, &QPlainTextEdit::undo);//模拟readonly
    connect(this, &QPlainTextEdit::blockCountChanged, this, &LogEdit::updateLineNumWidth);// 按下回车后
    connect(this, &QPlainTextEdit::updateRequest, this, &LogEdit::updateLineNum);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &LogEdit::highlightCurrentLine);
    bindInternalScroll();

    updateLineNumWidth();
    setLineWrapMode(QPlainTextEdit::NoWrap);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


}

void LogEdit::contextMenuEvent(QContextMenuEvent *e)
{
    QTextCursor cursor;

    cursor = textCursor();


    QString cursorWord = cursor.selectedText();
    int lineNum = blockNumberToPresenter(cursor.blockNumber());

    emit menuRequested(e->globalPos(), cursorWord, lineNum);
}
void LogEdit::setExternalVScrollbar(QScrollBar *bar)
{
    mExternalBar = bar;
    bindExternalScroll();
}
void LogEdit::setLog(std::shared_ptr<ILog> presenter)
{
    //mHistory.clear();

    mLog = presenter;
    mExternalBar->setEnabled(false);

    if (mLog) {
        auto range = presenter->availRange();
        qDebug()<<"update external range to "<<range.from<<","<<range.to;
        if (!range.isValid())//还没有数据
            return;


        updateExternalScrollRange(range);
//        if (mMode == LogEdit::WatchMode) {
           mExternalBar->setValue(mExternalBar->maximum());
//        } else {
 //          load(1);
//        }
    } else {
        clear();
    }

}
void LogEdit::onLogEmphasized(int lineNum)
{
    scrollToLine(lineNum);
}

void LogEdit::onLogRangeChanged()
{


    auto availRange = mLog->availRange();
    auto firstVisibleLineNumber = blockNumberToPresenter(firstVisibleBlock().blockNumber());


          if (!mBufferRange.isValid()) {
              load(1);
              unBindExternalScroll();
              updateExternalScrollRange(availRange);
              mExternalBar->setValue(firstVisibleLineNumber);
              bindExternalScroll();
              return ;
          }

          //取buffer range和avail range交集，
          auto intersectRange = availRange.intersect(mBufferRange);

          if (intersectRange.isValid()) {
              //如果intersectRange等于buffer range，但buffer range不够大，也load(intersectRange.from)
              if (intersectRange == mBufferRange) {
                  if (mBufferRange.to - firstVisibleLineNumber < 2 * mViewPortMaxLineCnt) {
                      load(firstVisibleLineNumber);
                  }
              }
              //如果intersectRange不等于buffer range，则load(intersectRange.from)
              else {
                  load(intersectRange.from, firstVisibleLineNumber);
              }
          } else {
              //如果完全没有交集，则load(avai range. from)
              firstVisibleLineNumber = availRange.from;
              if (availRange.isValid()) {
                  load(availRange.from);
              } else {
                  clear();
              }
          }
          unBindExternalScroll();
          updateExternalScrollRange(availRange);
          mExternalBar->setValue(firstVisibleLineNumber);
          bindExternalScroll();



//        if (availRange.isValid()) {
//            auto to = availRange.to;
//            auto from = to - 2 * mViewPortMaxLineCnt;
//            if (from < availRange.from)
//                from = availRange.from;

//            mBufferRange.from = from;
//            mBufferRange.to = to;

//            unBindInternalScroll();

//           auto horiValue = horizontalScrollBar()->value();
//            qDebug()<<mLog->type()<<",set plain text:"<<from<<","<<to;
//            setPlainText(mLog->readLines(from, to));
//           horizontalScrollBar()->setValue(horiValue);

//            verticalScrollBar()->setValue(verticalScrollBar()->maximum());
//            firstVisibleLineNumber = blockNumberToPresenter(firstVisibleBlock().blockNumber());

//            bindInternalScroll();
//        } else {
//            qDebug()<<mLog->type()<<",clear";
//            clear();
//        }


    unBindExternalScroll();
    updateExternalScrollRange(availRange);
    mExternalBar->setValue(firstVisibleLineNumber);
    bindExternalScroll();
}

void LogEdit::load(int preloadCenterLine, int firstVisibleLineNumber)
{
    qDebug()<<"load" ;
    auto availRange = mLog->availRange();
    if (!availRange.isValid())
        return;

    auto from = preloadCenterLine - PRELOAD_COUNT/2;
    if (from < availRange.from)
        from = availRange.from;

    auto to = preloadCenterLine + PRELOAD_COUNT/2;
    if (to > availRange.to)
        to = availRange.to;

    mBufferRange.from = from;
    mBufferRange.to = to;

    unBindInternalScroll();

    auto horiValue = horizontalScrollBar()->value();
    qDebug()<<mLog->type()<<",set plain text:"<<from<<","<<to;

    setPlainText(mLog->readLines(from, to));
    horizontalScrollBar()->setValue(horiValue);
    if (firstVisibleLineNumber < 0)
        firstVisibleLineNumber = preloadCenterLine;
    auto firstBlockNum = presenterNumberToBlock(firstVisibleLineNumber);
    if (firstBlockNum < 0)
        firstBlockNum = 0;
    setVertialScrollbarByBlockNum(firstBlockNum);

    bindInternalScroll();
}
void LogEdit::setVertialScrollbarByBlockNum(int blockNumber)
{
    //scrollbar的范围是按wrap后的行数计算，所以这里不能直接传入blockNumber,而是取block.firstLineNumber
    verticalScrollBar()->setValue(document()->findBlockByNumber(blockNumber).firstLineNumber());
}

void LogEdit::bindInternalScroll()
{
    if (mInternalVBarHandler)
        return;
    mInternalVBarHandler = connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &LogEdit::handleInternalScroll);
}

void LogEdit::unBindInternalScroll()
{
    if (mInternalVBarHandler)
        disconnect(mInternalVBarHandler);
}

void LogEdit::bindExternalScroll()
{
    if (mExternalVBarHandler)
        return;
    mExternalVBarHandler = connect(mExternalBar, &QScrollBar::valueChanged, this, &LogEdit::handleExternalScroll);
}

void LogEdit::unBindExternalScroll()
{
    if (mExternalVBarHandler)
        disconnect(mExternalVBarHandler);
}
void LogEdit::updateExternalScrollRange(Range range)
{
    auto min = range.from;
    auto max = range.to;// - mViewPortMaxLineCnt + 1;
    if (max < min)//数据还不够显示一屏
        max = min;

    mExternalBar->setRange(min, max);
    mExternalBar->setEnabled(true);
}

int LogEdit::presenterNumberToBlock(int lineNumber)
{
    return lineNumber - mBufferRange.from;
}

int LogEdit::blockNumberToPresenter(int blockNumber)
{
    return blockNumber + mBufferRange.from;
}

void LogEdit::handleInternalScroll(int)
{
    if (!mLog)
        return;

    if (mResizing)
        return;
    
    auto firstVisibleLineNum = blockNumberToPresenter(firstVisibleBlock().blockNumber());
    unBindExternalScroll();
    mExternalBar->setValue(firstVisibleLineNum);
    bindExternalScroll();

    auto windowTop = firstVisibleLineNum-1;
    auto windowBottom = firstVisibleLineNum + mViewPortMaxLineCnt+1;
//    qDebug()<<"["<<windowTop<<","<<windowBottom<<"] vs "<<mBufferRange;
    if (windowTop < mBufferRange.from || windowBottom > mBufferRange.to) {
        load(firstVisibleLineNum);
    }
}

void LogEdit::handleExternalScroll(int value)
{
    if (mBufferRange.contains(value)) {
        auto blockNum = presenterNumberToBlock(value);
        if (blockNum == firstVisibleBlock().blockNumber())
            return;

        unBindInternalScroll();
        setVertialScrollbarByBlockNum(presenterNumberToBlock(value));
        bindInternalScroll();
    } else {
        load(value);
    }
}
void LogEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    QColor lineColor = QColor(Qt::gray).lighter();
    selection.format.setBackground(lineColor);
    selection.format.setTextOutline(QPen(Qt::black));
    selection.format.setProperty(QTextFormat::FullWidthSelection,true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    //selection.cursor.select(QTextCursor::BlockUnderCursor);
    extraSelections.append(selection);


    setExtraSelections(extraSelections);
}

bool LogEdit::event(QEvent *ev)
{
    qDebug()<< ev->type();
    return QPlainTextEdit::event(ev);
}

static inline int calDigitCount(int d) {
    int cnt = 1;
    while (d >= 10) {
        d/=10;
        ++cnt;
    }
    return cnt+1;
}


int LogEdit::lineNumWidth()
{
    auto maxLineDigitCnt = calDigitCount(mBufferRange.to);
    auto singleDigitWidth = fontMetrics().horizontalAdvance(QLatin1Char('9'));
    return 8 + singleDigitWidth*maxLineDigitCnt;
}

void LogEdit::paintLineNum(QPaintEvent* event)
{
    QPainter painter(mLineNum);

    QColor bgColor(230,230,230);
    QColor fgColor(Qt::gray);
   if (mFocusManager->isInFocus(this)) {
       bgColor.setRgb(105, 105, 105);
       fgColor.setRgb(255, 255, 255);
   }

    painter.fillRect(event->rect(), bgColor);

    if (!mLog)
        return;

    QTextBlock block = firstVisibleBlock();
    auto lineNumber = blockNumberToPresenter(block.blockNumber());
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    auto curLine = blockNumberToPresenter(textCursor().blockNumber());
    //qDebug()<<"top" << top<< "cur"<<curLine<< "bottom"<< bottom;

    painter.setPen(fgColor);
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            painter.save();
            if (lineNumber == curLine) {
                auto font = painter.font();
                font.setUnderline(true);
                font.setBold(true);
                painter.setFont(font);
            }
            painter.drawText(0, top, mLineNum->width(), fontMetrics().height(),
                             Qt::AlignCenter, QString::number(lineNumber));
            painter.restore();
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++lineNumber;
    }
}

void LogEdit::updateLineNum(const QRect &rect,int dy)
{
    if (dy)
        mLineNum->scroll(0, dy);
    else
        mLineNum->update(0, rect.y(), mLineNum->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumWidth();
}

void LogEdit::updateLineNumWidth(int newBlockCnt)
{
     setViewportMargins(lineNumWidth(), 0, 0, 0);
}

void LogEdit::resizeEvent(QResizeEvent *e)
{

    paintEvent((QPaintEvent*)e);
    mResizing = true;
    QPlainTextEdit::resizeEvent(e);
    mResizing = false;

    auto metrics = fontMetrics();
    mViewPortMaxLineCnt = height()/metrics.height();

    QRect cr = contentsRect();
    mLineNum->setGeometry(QRect(cr.left(), cr.top(), lineNumWidth(), cr.height()));
}


void LogEdit::scrollToLine(int line,int col)
{
    auto cursor = textCursor();

    handleExternalScroll(line);//将target滚动到第一行

    auto blockNum = presenterNumberToBlock(line);
    auto block = document()->findBlockByNumber(blockNum);
    auto pos = block.position() + col;

    cursor.setPosition(pos);
    setTextCursor(cursor);

    repaint();
}

int LogEdit::find(SearchArg arg, bool forward)
{
    auto cursor = textCursor();
        //清除选区以保证连续search的正确
        if (cursor.hasSelection()) {
            auto len = cursor.selectionEnd() - cursor.selectionStart();
            if (forward) {
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, len);
            } else {
                cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, len);
            }
        }
        setTextCursor(cursor);

        Location l;
        l.line = blockNumberToPresenter(cursor.blockNumber());
        l.offset = cursor.positionInBlock();
        //因为text cursor的position指向后一个字符（比如对于竖线光标，position指示竖线后一个字符）
        //所以，为了避免反向搜索时搜索到自己，需要-1
        if (!forward && l.offset > 0) {
            l.offset -= 1;
        }

        return mLog->find(arg, l, forward);

}

void LogEdit::onLogFindOne(SearchResult ret)
{
    scrollToLine(ret.line, ret.offset);

    mHighlighter->clearSearchHighlight();

    //利用select选择search词
    auto cursor = textCursor();
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, ret.matchedText.length());
    setTextCursor(cursor);

    mHighlighter->searchHighlight(ret.matchedText, ret.arg.caseSensitive);
    repaint();
}
