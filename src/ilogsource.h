#ifndef ILOGSOURCE_H
#define ILOGSOURCE_H
#include <QObject>
#include <QApplication>
#include <QMutex>
#include <QWaitCondition>
#include <QMutexLocker>
#include <QJsonObject>
#include <QTextCodec>
#include <memory>
#include "event.h"




class ILog;
class ILogSource: public std::enable_shared_from_this<ILogSource>
{
public:
    ILogSource(QObject* eventHandler, QString type)
        :mEventHandler(eventHandler), mType(type){
        setCodec("UTF-8");
    }

    virtual ~ILogSource(){}

    virtual bool open() = 0;
   
    virtual void close() = 0;

    virtual std::shared_ptr<ILog> getLog() = 0;


    virtual bool setCodec(QString name) {
        mCodec = QTextCodec::codecForName(name.toStdString().c_str());
        return true;
    }

    QTextCodec* getCodec() {
        return mCodec;
    }


     virtual QString getSimpleDesc() = 0;


public:
    void post(BaseEvent* ev) {
        if ( (EventsType)ev->type() == evLogChange) {
            qDebug()<<"wait";
        }

        qApp->postEvent(mEventHandler, ev);
    }


protected:
    QTextCodec* mCodec;

private:
    QObject* mEventHandler;
    QString mType;
};

#endif // ILOGSOURCE_H
