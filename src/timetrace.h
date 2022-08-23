#ifndef TIMETRACE_H
#define TIMETRACE_H

#include <QGraphicsView>
#include <QObject>
#include <QWidget>
#include <QGraphicsItem>
#include <QList>
#include <QSplitter>
#include <functional>
#include <QTimeLine>
#include "timeline.h"


class TimeTrace: public QWidget
{
    Q_OBJECT
public:
    TimeTrace();
    void addTrace(QString);
    void addNode(int index,int lineNum, const QString &text, const QString time);
    void clear();
    bool isEmpty(){
        return mTraces.isEmpty();
    }
    int size(){
        return mTraces.size();
    }
    QString getName(int index){
        return mNames.at(index);
    }

private:
    QList<TimeLine*> mTraces;
    QList<QString> mNames;
    QSplitter* mSplitter;
};
#endif // TIMETRACE_H
