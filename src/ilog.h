#ifndef ILOG_H
#define ILOG_H

#include <memory>
#include <QString>
#include <list>
#include <QFile>
#include <QProgressDialog>
#include "utils.h"

class ILogSource;
class BaseEvent;

class ILog: public std::enable_shared_from_this<ILog>
{
public:
    ILog(QString type);
    virtual ~ILog(){}
    int find(SearchArg arg, Location loc, bool forward);
    int createChild(SearchArg arg,QString id = "");
public:
    virtual QString readLines(int from, int to);
    virtual QByteArray readRawLines(int from, int to) = 0;
    virtual Range availRange() = 0;

    virtual int fromParentLine(int){return -1;}
    virtual int toParentLine(int){return -1;}
    virtual bool onParentRangeChanged(Range, Range){return false;};

    int toRootLine(int line);
    std::shared_ptr<ILog> rootLog();

    QString readLine(int line){
        return readLines(line,line);
    }

    std::shared_ptr<ILog> parent();
    std::list<std::shared_ptr<ILog>> children();

    void setSource(const std::shared_ptr<ILogSource>& source) {
        mSource = source;
    }

    void post(BaseEvent* ev);

    QString type();

    virtual bool saveTo(QFile&) {return false;};

public:
    QString getType(){return mType;}
    void setId(QString id){mId = id;}
    QString getId() {return mId;}
    std::shared_ptr<ILog> findLogById(QString id);

private:
    std::shared_ptr<ILog> deepFindLogById(QString id);

protected:
    std::shared_ptr<ILog> mParent;
    std::list<std::weak_ptr<ILog>> mChildren;
    std::shared_ptr<ILogSource> mSource;
    QString mType;
    QString mId;
};

#endif // ILOG_H
