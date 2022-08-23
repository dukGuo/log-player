
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
#include <QHBoxLayout>
#include "timetrace.h"
#include "timeline.h"


TimeTrace::TimeTrace()
{

   mSplitter = new QSplitter;
   auto box = new QHBoxLayout;
   box->addWidget(mSplitter);
   box->setMargin(0);
   setLayout(box);

}

void TimeTrace::addTrace(QString name){
    mTraces << new TimeLine;

    if(name.isEmpty()){
        mNames << "Trace";
    }
    else{
        mNames << name;
    }
    mTraces[mTraces.size()-1]->addNode(0,"\n\n\t"+name,"");
    mSplitter->addWidget(mTraces[mTraces.size()-1]);

    connect(mTraces[mTraces.size()-1],&TimeLine::lineEmpty,[this](TimeLine* line){
        for(int i = 0; i < mTraces.size();i++){
            if (mTraces[i] == line){
                mTraces.removeAt(i);
                mNames.removeAt(i);
                break;
            }
        }
        if(mTraces.isEmpty()){
            hide();
        }
    });
    show();

}
void TimeTrace::addNode(int index,int lineNum, const QString &text, const QString time)
{
    if (index < mTraces.size() && index >=0){
        auto trace = mTraces.at(index);
        trace->addNode(lineNum,text,time);
    }
}

void TimeTrace::clear(){
    for(auto trace: mTraces){
        trace->clear();
    }
    mTraces.clear();
    mNames.clear();
    hide();
}

