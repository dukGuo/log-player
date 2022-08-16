#include <QSettings>
#include <QDebug>
#include "recentfile.h"

RecentFile::RecentFile(QString key, int maxCount)
    :mKey(key), mMaxCount(maxCount)
{

}

void RecentFile::add(QString path)
{
    QSettings config;
    auto lst = config.value(mKey).toList();
    qDebug()<<"recent setting key"<< config.allKeys();
    qDebug()<<"recent 1"<<lst;
    if (lst.contains(path)) {
        lst.removeOne(path);
    }

    lst.push_front(path);
    qDebug()<<"recent 2"<<lst;
    if (lst.size() > mMaxCount) {
        lst.pop_back();
    }

    config.setValue(mKey, lst);
    config.sync();
    qDebug()<<"recent setting"<< config.allKeys();

    createActions();
}

void RecentFile::mount(QMenu *menu, std::function<void (QString)> func)
{
    mMenu = menu;
    mFunc = func;
    createActions();
}

void RecentFile::createActions()
{
    QSettings config;
    auto lst = config.value(mKey).toList();

    mMenu->clear();

    for (auto& i : lst) {
        auto path = i.toString();
        qDebug()<<"recent c"<< path;
        mMenu->addAction(path, [this, path]{
            mFunc(path);
        });
    }

    mMenu->addAction("清除", [this]{
        QSettings config;
        config.setValue(mKey, QStringList{});
        config.sync();
        createActions();
    });
}
