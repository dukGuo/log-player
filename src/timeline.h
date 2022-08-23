#ifndef TIMELINE_H
#define TIMELINE_H

#include <QGraphicsView>
#include <QObject>
#include <QWidget>
#include <QGraphicsItem>
#include <QList>
#include <functional>
#include <QTimeLine>
#include "ilog.h"
#include "timenode.h"

#define LINE_X 120
#define MARGIN_LEFT 10
#define TIME_NODE_DOT_R 4

class TimeLine: public QGraphicsView
{
    Q_OBJECT
public:
    TimeLine(QWidget* parent = nullptr);
    
protected:
    void keyPressEvent(QKeyEvent *event) override;


public slots:
    void addNode(int lineNum, const QString& text,const QString time= "");
    void exportToImage(const QString& path);

    void clear();

signals:
    void nodeSelected(TimeNode* node);
    void lineEmpty(TimeLine* node);

private slots:
    void deleteNode(TimeNode* node);
    void updateCurrentNode(TimeNode* node);

private:
    void addNode(TimeNode* node);
    int calNodeY(int index);
    void fitLine();
    void withExportedImage(std::function<void(QImage&)> handler);

    void highlightItem(QGraphicsObject *item);

private:
    QGraphicsLineItem* mLine;
    QGraphicsEllipseItem* mLineHead;
    int mWidth;
    int mHeight;
    int mNodeTop;
    int mNodeStep{TIME_NODE_HEIGHT+20};
    int mLineX;
    int mLineY;
    QList<TimeNode*> mNodes;
    int mCurrentNodeIdx;
};
#endif // TIMELINE_H
