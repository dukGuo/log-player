#include <QThreadPool>
#include "ilog.h"
#include <QRegExp>
#include <thread>
#include <QDebug>
#include <QtConcurrent>

#include "event.h"
#include "ilogsource.h"
#include "sublog.h"

using namespace std;

ILog::ILog(QString type)
    :mType(type)
{
}

QString ILog::readLines(int from, int to)
{
    return mSource->getCodec()->toUnicode(readRawLines(from, to));
}

int ILog::toRootLine(int line)
{
    auto cur = shared_from_this();
    while (cur->parent()) {
        line = cur->toParentLine(line);
        cur = cur->parent();
    }
    return line;
}

shared_ptr<ILog> ILog::rootLog()
{
    return mSource->getLog();
}

shared_ptr<ILog> ILog::parent()
{
    return mParent;
}

list<shared_ptr<ILog> > ILog::children()
{
    list<shared_ptr<ILog>> ret;
    for (auto& wp : mChildren) {
        auto child = wp.lock();
        if (child)
            ret.push_back(child);
    }
    return ret;
}

int ILog::find(SearchArg arg, Location loc, bool forward)
{
    auto range = availRange();
    if (forward) {
        range.from = loc.line;
    } else {
        range.to = loc.line;
    }


    QThreadPool::globalInstance()->start([=]{

        auto patternLen = arg.pattern.length();
        auto doSearch = [=](int line, int pos) {
            auto text = readLine(line);
            auto caseSense = arg.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
            SearchResult ret;
            ret.line = line;
            ret.offset = -1;

            if (arg.regex) {
                QRegExp pattern(arg.pattern, caseSense);
                ret.offset = forward ? text.indexOf(pattern, pos) : text.lastIndexOf(pattern, pos);
                if (ret.offset >= 0) {
                    ret.matchedText = pattern.cap();
                }
            } else {
                ret.offset = forward ? text.indexOf(arg.pattern, pos, caseSense) : text.lastIndexOf(arg.pattern, pos, caseSense);
                if (ret.offset >= 0) {
                    ret.matchedText = text.mid(ret.offset, patternLen);
                }
            }

            return ret;
        };

        //第一行特殊处理
        auto ret = doSearch(loc.line, loc.offset);
        if (ret.offset >= 0)
            goto __return;

        //其他行
        if (forward) {
            for (auto i = range.from+1; i <= range.to; i++) {
                ret = doSearch(i, 0);
                if (ret.offset >= 0)
                    goto __return;
            }
        } else {
            for (auto i = range.to - 1; i >= range.from /*&& !op->isCanceled()*/; i--) {
                ret = doSearch(i, -1);
                if (ret.offset >= 0)
                    goto __return;
            }
        }

__return:
        post(new FindDoneEvent(shared_from_this(), ret));
    });

    return 0;
}

int ILog::createChild(SearchArg arg, QString id)
{

    auto range = availRange();

    auto chunkSize = 200000;//20w行一个块
    QVector<Range> taskRanges;
    int offset = range.from;
    while (offset + chunkSize <= range.to) {
        taskRanges.push_back({offset, offset+chunkSize-1});
        offset+=chunkSize;
    }
    if (offset+chunkSize > range.to) {
        taskRanges.push_back({offset, range.to});
    }

    QtConcurrent::run([this, arg, taskRanges, range, id]{


        shared_ptr<SubLog> sub(new SubLog(shared_from_this(), arg));
        sub->mSource = mSource;
        sub->setId(id);
        mChildren.push_back(sub);

        function<QVector<int>(const Range&)> mapFunc = [this, arg](const Range& r){


            auto revert = arg.revert;
            auto caseSense = arg.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
            QRegExp pattern(arg.pattern, caseSense);
            QVector<int> ret;
            ret.reserve(r.size());
            QString line;

            for (int i = r.from; i <= r.to ; i++) {
                line.clear();
                line.append(readLine(i));

                auto match = arg.regex ? line.contains(pattern) : line.contains(arg.pattern, caseSense);
                if (match ^ revert) {
                    //match 0 revert 1 || match 1 revert 0
                    ret.push_back(i);
                }
            }

            return ret;
        };

        auto rets = QtConcurrent::mapped(taskRanges, mapFunc);

        sub->mLineInParent.reserve(range.size());
        for (auto i = 0; i < taskRanges.size() ; i++) {
            auto ret = rets.resultAt(i);
            sub->mLineInParent.append(ret);
        }
        sub->mLineInParent.shrink_to_fit();
        post(new SubLogCreatedEvent(sub));
        
    });

    return 0;
}

void ILog::post(BaseEvent *ev)
{
    if (mSource)
        mSource->post(ev);
}

QString ILog::type()
{
    return mType;
}

shared_ptr<ILog> ILog::findLogById(QString id)
{
    return deepFindLogById(id);
}

shared_ptr<ILog> ILog::deepFindLogById(QString id)
{
    if (id == mId)
        return shared_from_this();

    for (auto&& child : children()) {
        auto ret = child->deepFindLogById(id);
        if (ret)
            return ret;
    }

    return shared_ptr<ILog>();
}
