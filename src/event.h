#ifndef EVENT_H
#define EVENT_H
#include <memory>
#include <functional>
#include <QEvent>
#include "ilog.h"

enum EventsType{
    evError = 1001,
    evLogChange,
    evSubLogCreated,
    evEmphasizeLine,
    evFindDone,
    // evSourceReady,//ready后可以执行filter动作
    // evSourceFinish,//finish后表示不会再产生新的日志，也就不能再暂停日志了
};

typedef std::function<void()> EventAction;
class ILogSource;
class ILog;
class SubLog;

class BaseEvent: public QEvent {
public:
    BaseEvent(Type t)
        :QEvent(t){}
};

class LogChangeEvent: public BaseEvent
{
public:
    LogChangeEvent(const std::shared_ptr<ILog>& in_who, const EventAction& in_updateLog)
        :BaseEvent(Type(evLogChange)), who(in_who), updateLog(in_updateLog)
    {

    }
    std::shared_ptr<ILog> who;
    EventAction updateLog;
};

class SubLogCreatedEvent : public BaseEvent
{
public:
    SubLogCreatedEvent(const std::shared_ptr<SubLog>& in_log) :
        BaseEvent(Type(evSubLogCreated)), log(in_log)
    {

    }
    std::shared_ptr<SubLog> log;
};

class EmphasizeLineEvent : public BaseEvent
{
public:
    EmphasizeLineEvent(const std::shared_ptr<ILog>& in_who, const int& in_line, const bool& in_isSource)
        :BaseEvent(Type(evEmphasizeLine)), who(in_who), line(in_line), isSource(in_isSource)
    {

    }
    std::shared_ptr<ILog> who;
    int line;
    bool isSource;
};

class FindDoneEvent : public BaseEvent
{
public:
    FindDoneEvent(const std::shared_ptr<ILog>& in_who, const SearchResult& in_ret)
        :BaseEvent(Type(evFindDone)), who(in_who), ret(in_ret)
    {

    }
    std::shared_ptr<ILog> who;
    SearchResult ret;
};

class ErrorEvent : public BaseEvent
{
public:
    ErrorEvent(const EventAction& in_cleanup)
        :BaseEvent(Type(evError)), cleanup(in_cleanup)
    {

    }
    EventAction cleanup;
};

//class SourceReadyEvent : public BaseEvent
//{
//public:
//    SourceReadyEvent() :
//        BaseEvent(Type(evSourceReady))
//    {

//    }
//};

// class SourceFinishEvent : public BaseEvent
// {
// public:
//     SourceFinishEvent()
//         :BaseEvent(Type(evSourceFinish))
//     {

//     }
// };
#endif // EVENT_H
