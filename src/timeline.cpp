
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsDropShadowEffect>
#include <QImage>
#include <QtGlobal>
#include <QApplication>
#include <QClipboard>
#include <QPropertyAnimation>
#include <QJsonArray>
#include <QKeyEvent>
#include <QMessageBox>
#include "timeline.h"
#include "timenode.h"
TimeLine::TimeLine(QWidget* parent)
    :QGraphicsView(parent)
{
    setFrameStyle(QFrame::NoFrame);
    QGraphicsScene* scene = new QGraphicsScene;
    mWidth = 460;
    mHeight = 400;
    scene->setSceneRect(0,0,mWidth,mHeight);
    setScene(scene);
    
    mLineX = LINE_X+MARGIN_LEFT;
    mLineY = 8;
    const double d = 4;
    mLineHead = scene->addEllipse({(double)mLineX-d/2, 4, d, d}, Qt::NoPen, QColor(180,180,180));

    mLine = new QGraphicsLineItem();
    mLine->setLine(mLineX, mLineY, mLineX, mHeight);
    mLine->setPen(QPen(QColor(180,180,180)));
    scene->addItem(mLine);

    mNodeTop = 20;
    mCurrentNodeIdx = -1;
}


void TimeLine::keyPressEvent(QKeyEvent *event)
{
    auto key = event->key();
    switch (key){
        case Qt::Key_Up:{
            auto old = mCurrentNodeIdx;
 
            if (mCurrentNodeIdx > 0){
                mCurrentNodeIdx--;
//                mNodes.at(old)->updateHighlight(false);
//                mNodes.at(mCurrentNodeIdx)->updateHighlight(true);
                mNodes.at(mCurrentNodeIdx)->emitSelect();
                highlightItem(mNodes.at(mCurrentNodeIdx));
            }
            qDebug()<< "key: up_push"<<old<<"->"<<mCurrentNodeIdx;
            break;
        }
        case Qt::Key_Down:{
            
            auto old = mCurrentNodeIdx;
            if (mCurrentNodeIdx < mNodes.size()-1){
                mCurrentNodeIdx++;
//                mNodes.at(old)->updateHighlight(false);
//                mNodes.at(mCurrentNodeIdx)->updateHighlight(true);
                mNodes.at(mCurrentNodeIdx)->emitSelect();
                highlightItem(mNodes.at(mCurrentNodeIdx));

            }
            qDebug()<< "key: down_push"<<old<<"->"<<mCurrentNodeIdx;
            break;
        }
        default:
            qDebug()<< "key: some key"<< mCurrentNodeIdx;
    }
}


void TimeLine::addNode(int lineNum, const QString &text, const QString time) {
    //找到插入位置
    int pos = mNodes.size();
    for (int i = 0; i < mNodes.size(); i++) {
        auto order = mNodes.at(i)->lineNumber();
        if (lineNum < order) {
            pos = i;
            break;
        } else if (lineNum == order) {
            return;
        }
    }

    mCurrentNodeIdx = pos;
    //大于该位置的节点后移
    for (int i = pos; i < mNodes.size(); i++) {
        mNodes.at(i)->setY(calNodeY(i+1));
    }

    //插入新的节点
    auto node = new TimeNode(lineNum, "No "+QString("%1").arg(lineNum)+time, text);
    mNodes.insert(pos, node);

    fitLine();

    node->setX(MARGIN_LEFT);
    node->setY(calNodeY(pos));

    scene()->addItem(node);
    node->ensureVisible(0,0,10,10);

    connect(node, SIGNAL(requestDel(TimeNode*)), this, SLOT(deleteNode(TimeNode*)));
    connect(node, SIGNAL(selected(TimeNode*)), this, SIGNAL(nodeSelected(TimeNode*)));
    connect(node, SIGNAL(selected(TimeNode*)),this,SLOT(updateCurrentNode(TimeNode*)));

    
    highlightItem(node);

    show();
}

void TimeLine::exportToImage(const QString& path)
{
    withExportedImage([&path](QImage& img){
        img.save(path);
    });
}


void TimeLine::clear()
{
    for (auto* node : mNodes) {
        scene()->removeItem(node);
    }
    mNodes.clear();

    hide();
    emit lineEmpty(this);
}

void TimeLine::deleteNode(TimeNode *node)
{
    scene()->removeItem(node);
    mNodes.removeOne(node);

    for (int i = 0; i < mNodes.size(); i++) {
        mNodes.at(i)->setY(calNodeY(i));
    }

    fitLine();

    if (mNodes.isEmpty()){
        hide();
        emit lineEmpty(this);
    }
}
void TimeLine::updateCurrentNode(TimeNode *node)
{
    if(node != nullptr){
        for (int i = 0; i < mNodes.size(); i++) {
            if (node == mNodes.at(i)){
                mCurrentNodeIdx = i;
            }
        }
    }
}


int TimeLine::calNodeY(int index)
{
    return index * mNodeStep + mNodeTop;
}

void TimeLine::fitLine()
{
    auto minHeight = calNodeY(mNodes.size())+30;
    if (minHeight < 100)
        minHeight = 100;

    mHeight = minHeight;
    scene()->setSceneRect(0,0, mWidth, mHeight);
    mLine->setLine(mLineX, mLineY, mLineX, mHeight);
}

void TimeLine::withExportedImage(std::function<void (QImage &)> handler)
{
    auto rect = sceneRect();
#ifdef Q_OS_MAC
    int borderMargin = 10;
    //避免图片发虚
    QImage img((int)(rect.width()*2) + 2*borderMargin, (int)(rect.height()*2) + 2*borderMargin, QImage::Format_RGB32);
#else
    int borderMargin = 5;
    QImage img((int)(rect.width()) + 2*borderMargin, (int)(rect.height()) + 2*borderMargin, QImage::Format_RGB32);
#endif

    QPainter painter(&img);
    painter.fillRect(img.rect(), QColor(250,250,250));
    auto borderRect = img.rect().adjusted(borderMargin,borderMargin, -borderMargin, -borderMargin);
    scene()->render(&painter, borderRect);

    painter.setPen(Qt::gray);
    painter.drawRect(borderRect);


    handler(img);
}

void TimeLine::highlightItem(QGraphicsObject *item)
{
    auto anim = new QPropertyAnimation(item, "scale");
    anim->setDuration(500);
    anim->setKeyValueAt(0, 1.0);
    anim->setKeyValueAt(0.5, 1.2);
    anim->setKeyValueAt(1, 1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    scene()->clearSelection();
    item->setSelected(true);
}
