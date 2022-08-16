#include "FocusManager.h"
#include <QDebug>
#include <QApplication>

FocusManager::FocusManager() {
    mConnectId = connect(qApp, &QApplication::focusChanged, [this](QWidget*, QWidget* now){
        if (mItems.contains(now)) {
            qDebug()<< " focus changed";
            auto old = mCurrent;
            mCurrent = now;
            emit focusChanged(old, mCurrent);
        }
    });
}

FocusManager::~FocusManager()
{
    disconnect(mConnectId);
}


void FocusManager::add(QWidget *edit) {
    mItems.insert(edit);
    qDebug()<< "add";
    auto old = mCurrent;
    mCurrent = edit;
    emit focusChanged(old, mCurrent);
}

void FocusManager::remove(QWidget *edit) {
    mItems.remove(edit);
    qDebug()<< "remove";
    if (mCurrent == edit && !mItems.isEmpty()) {
        auto old = mCurrent;
        mCurrent = *(mItems.begin());
        emit focusChanged(old, mCurrent);
    }
}

bool FocusManager::isInFocus(QWidget *edit) {
    qDebug()<< (int)mCurrent<<"edit"<<(int)edit;
    return mCurrent == edit;
}

QWidget* FocusManager::current() {
    return mCurrent;
}
