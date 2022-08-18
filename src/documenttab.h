#ifndef DOCUMENTTAB_H
#define DOCUMENTTAB_H

#include <QTabWidget>
#include <QSplitter>
#include <QWidget>
#include <memory>
#include <QDialog>
#include "utils.h"
#include "logviewer.h"
#include "timeline.h"
#include "FocusManager.h"
#include "ilogsource.h"
#include "controller.h"

class DocumentTab
    : public QWidget
{
    Q_OBJECT

public:
    DocumentTab();
    void init(std::shared_ptr<ILogSource> log);
    void unInit();
    void setName(QString name) {mName = name;}
    QString getName() {return mName;}
    void bindController();
    void unBindController();
    LogEdit* currentEdit();
    void bindEditor(LogEdit* edit,bool isSub = false);
    

private:
    void doFilter(SearchArg arg);
    void appendSubLog(LogViewer* logViewer, const QString& hint);
    void closeSubLog(int index);
    void onLogChange(std::shared_ptr<ILog> who, EventAction update);
    LogEdit* findLogEditByLog(const std::shared_ptr<ILog>& log);
    void deepHandleLogChange(const std::shared_ptr<ILog>&log, Range before, Range after);
    void deepEmphasizeLine(const std::shared_ptr<ILog>& log, int line, const std::shared_ptr<ILog>& source);
    void onSubLogCreated(std::shared_ptr<SubLog> subLog);
    void onFindDone(std::shared_ptr<ILog> who, SearchResult ret);
    void onLineEmphasized(std::shared_ptr<ILog> who, int line, bool isSource);
    void bindTaglist();
    void loadCurrentTaglist(LogEdit* editor);
    QString searchTime(int sourceLine,LogEdit* edit);


protected:
    void customEvent(QEvent* ev) override;

signals:
    void fileSplitted(QString path);
public slots:
    void find(SearchArg arg, bool forward);
    void filter();
    void revertFilter();
    void exportTimeLineToImage();
    void setPattern();

private slots:
    void locateLine();
    void splitFile();
    void findKeyword();
    void addHighlight();
private:
    // layout
    // ------+-----
    // | log |     |
    // |     |time |
    // |-----+     |
    // | sub |line |
    // |_____|_____|

    QTabWidget* mSubLogTabWidget;
    QSplitter* mLogSplitter;
    QString mName;
    LogViewer* mLogViewer;
    TimeLine* mTimeLine;
    FocusManager mFocusManager;
    std::shared_ptr<ILogSource> mLogSource;
    QVector<QMetaObject::Connection> mConnections;

};

class FilterDialog : public QDialog
{
public:
    FilterDialog();

    SearchArg getArg(){return mArg;}

private:
    SearchArg mArg;
};

#endif // DOCUMENTTAB_H
