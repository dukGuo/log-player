#ifndef TIMENODE_H
#define TIMENODE_H

#include <QGraphicsItem>
//#include <QJsonObject>

#define TIME_NODE_HEIGHT 140
#define TIME_NODE_DOT_R 4
#define TIME_NODE_BODY_WIDTH 260

class TimeNodeBody;
class TimeNode: public QGraphicsObject
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    TimeNode(int lineNum, const QString& locateText, const QString& detailText, const QString& memo = "Note:");
public:

    void updateHighlight(bool seleced){
        setSelected(seleced);
    }
    void emitSelect(){
        emit selected(this);
    }

public:
    QColor color() {return mColor;}
    int data() {return mLineNum;}
    int lineNumber() {return mLineNum;}
protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

signals:
    void requestDel(TimeNode* node);
    void selected(TimeNode* node);
private slots:
    void handleDelActionTriggered();


private:
    QGraphicsRectItem* mHlRect;
    double mWidth;
    int mLineNum;
    QColor mColor{Qt::blue};
    TimeNodeBody* mBody;
};


class TimeNodeHead: public QGraphicsItem {
private:
    QString mText;
    TimeNode* mNode;
public:
    TimeNodeHead(TimeNode* node, const QString& text);
protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
};



class TimeNodeBody: public QGraphicsObject
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
private:
    TimeNode* mNode;
    QGraphicsRectItem* mColorRect;
    QGraphicsTextItem* mMemo;
public:
    TimeNodeBody(TimeNode* node, const QString& text, const QString& memo);
    QString getMemo();
protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
};

#endif // TIMENODE_H
