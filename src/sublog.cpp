#include <algorithm>
#include <QProgressDialog>
#include <QEventLoop>
#include <QFile>
#include <QProgressDialog>
#include <QFileDialog>
#include <QtConcurrent>
#include <memory>
#include <QMessageBox>
#include "sublog.h"
#include "ilog.h"

using namespace std;

SubLog::SubLog(shared_ptr<ILog> parent, SearchArg arg)
    :ILog("sub")
{
    mSearchArg = arg;
    mParent = parent;
}

SubLog::~SubLog()
{
//    qDebug()<<"~SubLog";
}

QString SubLog::readLines(int from, int to)
{
     QString buffer;

     from -= mFromLineNum;
     to -= mFromLineNum;

     //尝试拼接连续的行
     auto contBegin = mLineInParent[from];
     auto contEnd = contBegin;
     for (auto i = from + 1; i <= to; i++) {
         auto line = mLineInParent[i];
         if (line - 1 == contEnd) {
             contEnd = line;
             continue;
         } else {
             buffer.append(mParent->readLines(contBegin, contEnd));
             contBegin = line;
             contEnd = line;
         }
     }
     buffer.append(mParent->readLines(contBegin, contEnd));

     return buffer;
}

QByteArray SubLog::readRawLines(int from, int to)
{
    QByteArray buffer;

    from -= mFromLineNum;
    to -= mFromLineNum;

    //尝试拼接连续的行
    auto contBegin = mLineInParent[from];
    auto contEnd = contBegin;
    for (auto i = from + 1; i <= to; i++) {
        auto line = mLineInParent[i];
        if (line - 1 == contEnd) {
            contEnd = line;
            continue;
        } else {
            buffer.append(mParent->readRawLines(contBegin, contEnd));
            contBegin = line;
            contEnd = line;
        }
    }
    buffer.append(mParent->readRawLines(contBegin, contEnd));

    return buffer;
}

Range SubLog::availRange()
{
    return {mFromLineNum, mFromLineNum+mLineInParent.size()-1};
}

int SubLog::fromParentLine(int line)
{
    auto begin = mLineInParent.begin();
    auto end = mLineInParent.end();

    auto it = std::lower_bound(begin, end, line);
    if (it == end)
        return -1;
    if (*it != line)
        return -1;

    return it - begin + mFromLineNum;
}

int SubLog::toParentLine(int line)
{
    //由于事件机制的原因，可能在发出emphasize事件到我们准备查找parent行时，mLineInParent已经发生改变
    //所以我们这里再检查一遍index的合法性
    auto index = line - mFromLineNum;
    if (index < 0 || index >= mLineInParent.size()) {
        return -1;
    }
    return mLineInParent[index];
}

SearchArg SubLog::getSearchArg()
{
    return mSearchArg;
}


