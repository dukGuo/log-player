#ifndef SUBLOG_H
#define SUBLOG_H
#include <memory>
#include "ilog.h"

class SubLog: public ILog
{
public:
    SubLog(std::shared_ptr<ILog> parent, SearchArg arg);
    ~SubLog();

public:
    QString readLines(int from, int to) override;
    QByteArray readRawLines(int from, int to) override;
    Range availRange() override;

    int fromParentLine(int line) override;
    int toParentLine(int line) override;

    SearchArg getSearchArg();

    //bool onParentRangeChanged(Range before, Range after) override;

    //bool saveTo(QFile& file) override;

private:
    SearchArg mSearchArg;
    friend class ILog;
    QVector<int> mLineInParent;
    int mFromLineNum{1};

};

#endif // SUBLOG_H
