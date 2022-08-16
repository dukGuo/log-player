#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <QAction>
#include <QMap>
#include "searchbar.h"
#include "taglistwidget.h"

class Controller
{

public:
    enum ActionType{
        GoBackward,
        GoForward,
        Filter,
        RevertFilter,
        OpenFile,
        SplitFile,    
        CloseTab,
        LocateLine,
        SetPattern,   
        SaveTimeLine,
        ClearTimeLine,
        Find,
        RevertFind,     
        FindNext,
        FindPrevious,
        Highlight,
        Usage,          // unused
        Shortcut,       
//        PauseSource,    // unused
        ExportSubLog,   // future
//        SaveProject,    // unused
    };
private:
    Controller();
    void setAction(ActionType Type,QString hint,QString shortcut,bool isDoc = false,QString icon = "");

public:

    static Controller& instance();
    QString getShortcutHint();

    QAction* actionFor(ActionType type) {
           return mActions[type];
       }

       SearchBar* searchBar() {
           return mSearchBar;
       }

       void setSearchBar(SearchBar* bar) {
           mSearchBar = bar;
       }

       TagListWidget* tagList() {
           return mTagList;
       }

       void setTagList(TagListWidget* taglist) {
           mTagList = taglist;
       }

       QVector<QAction*> actionsForDoc(){
           return mDocActions;
       }
       QRegExp getTime(){
           return timeRx;
       }
       QRegExp getContent(){
           return contentRx;
       }
       bool setTimeRx(QString pattern);
       bool setContentRx(QString pattern);

private:
    QMap<ActionType, QAction*> mActions;
    QVector<QAction*> mDocActions;
    TagListWidget* mTagList;
    SearchBar* mSearchBar;
    QRegExp timeRx;
    QRegExp contentRx;
};

#endif // CONTROLLER_H
