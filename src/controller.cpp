#include <QDebug>
#include "controller.h"

Controller::Controller()
{
    qDebug()<< "Controller Constructing";
    setAction(Filter,QString("过滤"),QString("Ctrl+L"),true);
    setAction(RevertFilter,QString("反向过滤"),QString("Ctrl+Shift+L"),true);
    setAction(OpenFile,QString("打开文件..."),QString("Ctrl+O"));
    setAction(SplitFile,QString("分割文件..."),QString("Ctrl+T"),true);
    setAction(CloseTab,QString("关闭标签"),QString("Ctrl+W"),true);
    setAction(LocateLine,QString("跳转到"),QString("Ctrl+G"),true);
    setAction(SetPattern,QString("匹配模式"),QString("Ctrl+P"),true);
    setAction(SaveTimeLine,QString("导出时间线"),QString("Ctrl+Shift+S"),true);
    setAction(ClearTimeLine,QString("清除时间线"),QString("Ctrl+Shift+D"),true);
    setAction(Find,QString("查找"),QString("Ctrl+F"),true);
    setAction(RevertFind,QString("反向查找"),QString("Ctrl+Shift+F"),true);
    setAction(FindNext,QString("下一个"),QString("Ctrl+N"),true);
    setAction(FindPrevious,QString("上一个"),QString("Ctrl+M"),true);
    setAction(Highlight,QString("添加高亮"),QString("Ctrl+J"),true);
    setAction(Shortcut,QString("快捷键"),QString(""));
    timeRx = QRegExp("\\[[0-9]{4}-[0-1][0-9]-[0-3][0-9]\\s[0-2][0-9]:[0-6][0-9]:[0-6][0-9].[0-9]{3}\\]");
    //timeRx = QRegExp("\\[[0-9]{4}-[0-1][0-9]-[0-3][0-9]");
    contentRx= QRegExp(".*");
}

void Controller::setAction(ActionType Type,QString hint,QString shortcut,bool isDoc ,QString icon)
{
    QAction *action;
    if(icon.isEmpty()){
        action = new QAction(hint);
    }
    else {
        action = new QAction(QIcon(icon),hint);
    }
    if(!shortcut.isEmpty()){
        action ->setShortcut(shortcut);
    }
    if(isDoc){
        mDocActions.append(action);
    }
    mActions[Type] = action;
}

Controller& Controller::instance()
{
    static Controller Ctr;
    return Ctr;
}

QString Controller::getShortcutHint()
{
    QStringList seqs;

    for (auto&& action : mActions.values()) {
        if(action == nullptr || action->shortcut().isEmpty())
            continue;
        QString description;
        if (action->text().size() >= 5)
           description = action->text() + "\t" + action->shortcut().toString(QKeySequence::NativeText);
        else
           description = action->text() + "\t\t" + action->shortcut().toString(QKeySequence::NativeText);

        seqs.push_back(description);
    }


    return seqs.join('\n');
}

bool Controller::setTimeRx(QString pattern)
{

    QRegExp rx = QRegExp(pattern);
    if (rx.isValid()){
        qDebug()<<"set timeRx"<< pattern;
        timeRx = rx;
        return true;
    }

    return false;
}

bool Controller::setContentRx(QString pattern)
{
    QRegExp rx = QRegExp(pattern);
    if (rx.isValid()){
        qDebug()<<"set contentRx"<< pattern;
        contentRx = rx;
        return true;
    }

    return false;
}
