#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <list>
#include <QEvent>

#include <memory>
#include <QJsonValue>
#include <QJsonObject>


struct Location {
    int line{0};
    int offset{0};
    Location(){}
    Location(int line, int offset) {
        this->line = line;
        this->offset = offset;
    }
};

struct Range {
    int from{1};
    int to{0};

    bool contains(int value)
    {
        return from <= value && value <= to;
    }
    Range(){}
    Range(int from, int to){
        this->from = from;
        this->to = to;
    }

    bool isValid() {
        return to >= from;
    }

    Range intersect(Range& r)
    {
        Range newRange;
        newRange.from = qMax(from, r.from);
        newRange.to = qMin(to, r.to);
        return newRange;
    }

    bool operator == (const Range& r) {
        return from == r.from && to == r.to;
    }

    int size() const{
        return to - from + 1;
    }
};

struct SearchArg {
    bool regex{false};
    bool caseSensitive{false};
    QString pattern;
    bool revert{false};

};

struct SearchResult : public Location {
    SearchArg arg;
    bool forward;

    QString matchedText{""};
    bool isValid() const{
        return offset >= 0;
    }
};



#endif // UTILS_H
