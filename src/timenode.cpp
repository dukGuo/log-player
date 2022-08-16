#include <QFontMetrics>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsBlurEffect>
#include <QDebug>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QTextOption>
#include <QPainter>
#include <QInputDialog>
#include <QGraphicsTextItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsDropShadowEffect>
#include <QTextDocument>
#include "timeline.h"
#include "timenode.h"

#define MARGIN (2)
#define COLOR_RECT_WIDTH (2)
#define MEMO_HEIGHT (40)
#define CONTENT_WIDTH (TIME_NODE_BODY_WIDTH-MARGIN*2-COLOR_RECT_WIDTH)
#define CONTENT_HEIGHT (TIME_NODE_HEIGHT-MEMO_HEIGHT-2*MARGIN)

TimeNode::TimeNode(int lineNum, const QString &locateText, const QString &detailText, const QString& memo)
    :mLineNum(lineNum) {
    setFlag(GraphicsItemFlag::ItemIsSelectable);
   // auto test = QString("text in timeline");
    auto head = new TimeNodeHead(this, locateText);
    head->setX(0);
    head->setY(0);
    head->setParentItem(this);

    mWidth = LINE_X + TIME_NODE_DOT_R;

    mBody = new TimeNodeBody(this, detailText, memo);
    mBody->setX(mWidth + 8);
    mBody->setY(0);
    mBody->setParentItem(this);

    mWidth += (8 + TIME_NODE_BODY_WIDTH);

    mWidth += 2;

    mHlRect = new QGraphicsRectItem(-6,-6,mWidth+12, TIME_NODE_HEIGHT+12, this);
    QPen pen(Qt::red);
    pen.setWidth(2);
    mHlRect->setPen(pen);
    mHlRect->setVisible(false);
    auto blur = new QGraphicsBlurEffect(this);
    blur->setBlurRadius(4);
    mHlRect->setGraphicsEffect(blur);

//    mProjectData["lineNum"] = lineNum;
//    mProjectData["detail"] = detailText;
}

//TimeNode::TimeNode(const QJsonValue &jo)
//    :TimeNode(jo["lineNum"].toInt(),
//      QString("%1").arg(jo["lineNum"].toInt()),
//      jo["detail"].toString(),
//      jo["memo"].toString())
//{
//    setColor(jo["color"].toString());
//}

//QJsonValue TimeNode::saveToJson()
//{
//    mProjectData["memo"] = mBody->getMemo();
//    mProjectData["color"] = mColor.name();
//    return mProjectData;
//}

QRectF TimeNode::boundingRect() const
{
    return QRectF(0,0, mWidth, TIME_NODE_HEIGHT);
}

void TimeNode::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{
}

void TimeNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    setSelected(true);
    auto menu = new QMenu();

    auto action = menu->addAction("Remove");
    connect(action, SIGNAL(triggered()), this, SLOT(handleDelActionTriggered()));
    menu->exec(event->screenPos());
    delete menu;
}

void TimeNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit selected(this);
    QGraphicsItem::mousePressEvent(event);
}

QVariant TimeNode::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged) {
        auto selected = value.toBool();
        mHlRect->setVisible(selected);
    }
    
    return value;
}

void TimeNode::handleDelActionTriggered()
{
    emit requestDel(this);
}




TimeNodeHead::TimeNodeHead(TimeNode *node, const QString &text)
    :mText(text), mNode(node){
}

QRectF TimeNodeHead::boundingRect() const {
    return QRectF(0, 0, LINE_X + TIME_NODE_DOT_R, TIME_NODE_HEIGHT);
}

void TimeNodeHead::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *){
    QRectF textRect(0, 0, LINE_X, TIME_NODE_HEIGHT);
    painter->setPen(mNode->color());
    painter->drawText(textRect, mText, QTextOption(Qt::AlignCenter));

    painter->setPen(Qt::NoPen);
    painter->setBrush(mNode->color());
    painter->drawEllipse(QPointF(LINE_X, TIME_NODE_HEIGHT/2), TIME_NODE_DOT_R, TIME_NODE_DOT_R);
}



class TextItem: public QGraphicsItem
{
private:
    QString mText;
public:
    TextItem(const QString& text, QGraphicsItem* parent):QGraphicsItem(parent),mText(text){}
protected:
    QRectF boundingRect() const override {
        return QRectF(0,0, CONTENT_WIDTH, CONTENT_HEIGHT);
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {
            QTextOption op(Qt::AlignLeft);
            op.setWrapMode(QTextOption::WrapAnywhere);
            QFont font = painter->font() ;
            font.setPointSize(8);
            painter->setFont(font);

            painter->setPen(QColor(100,100,100));
            painter->drawText(boundingRect(), mText, op);
    }
};

TimeNodeBody::TimeNodeBody(TimeNode* node, const QString &text, const QString& memo)
    :mNode(node){
    auto rect = new QGraphicsRectItem(0,0,TIME_NODE_BODY_WIDTH, TIME_NODE_HEIGHT, this);
    rect->setBrush(Qt::white);
    rect->setPen(Qt::NoPen);
    auto shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(2,2);
    shadow->setBlurRadius(10);
    rect->setGraphicsEffect(shadow);

    mColorRect = new QGraphicsRectItem(0,0,COLOR_RECT_WIDTH,TIME_NODE_HEIGHT, this);
    mColorRect->setBrush(mNode->color());
    mColorRect->setPen(Qt::NoPen);

    auto content = new TextItem(text, this);
    content->setX(MARGIN+COLOR_RECT_WIDTH);
    content->setY(MARGIN);

    auto line = new QGraphicsLineItem(this);
    line->setLine(MARGIN+COLOR_RECT_WIDTH+10, MARGIN+CONTENT_HEIGHT,
                  MARGIN+COLOR_RECT_WIDTH+CONTENT_WIDTH-20, MARGIN+CONTENT_HEIGHT);
    line->setPen(QPen(Qt::DotLine));

    mMemo = new QGraphicsTextItem(memo, this);
    mMemo->setTextInteractionFlags(Qt::TextEditorInteraction);
    mMemo->setTextWidth(CONTENT_WIDTH);
    mMemo->setX(MARGIN+COLOR_RECT_WIDTH);
    mMemo->setDefaultTextColor(mNode->color());
    QFont font = mMemo->font() ;
    font.setPointSize(9);
    mMemo->setFont(font);
    QTextOption op;
    op.setWrapMode(QTextOption::WrapAnywhere);
    mMemo->document()->setDefaultTextOption(op);
    mMemo->setY(MARGIN+CONTENT_HEIGHT);
}

QString TimeNodeBody::getMemo()
{
    return mMemo->toPlainText();
}

QRectF TimeNodeBody::boundingRect() const {
    return QRectF(0, 0, TIME_NODE_BODY_WIDTH, TIME_NODE_HEIGHT);
}

void TimeNodeBody::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) {
    mColorRect->setBrush(mNode->color());
    mMemo->setDefaultTextColor(mNode->color());
}

