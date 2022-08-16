#ifndef RECENTFILE_H
#define RECENTFILE_H


#include <QString>
#include <QMenu>
#include <functional>

class RecentFile
{
public:
    RecentFile(QString key, int maxCount);

    void add(QString path);

    void mount(QMenu* menu, std::function<void(QString path)> func);

private:
    void createActions();

private:
    QString mKey;
    int mMaxCount;
    QMenu* mMenu;
    std::function<void(QString path)> mFunc;
};


#endif // RECENTFILE_H
