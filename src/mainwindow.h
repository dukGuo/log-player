#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "welcomepage.h"
#include "documenttab.h"
#include "recentfile.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    DocumentTab* currentDoc();
protected:
    void closeEvent(QCloseEvent* ev) override;
private:
    void buildMenuBar();
    void buildToolBar();
    void buildTab();
    void bindController();
    QMenu*  buildFileMenu();
    QMenu*  buildSearchMenu();
    QMenu*  buildHighlightMenu();
    QMenu*  buildTimelineMenu();
    int appendDocumentTab(DocumentTab *tab);
    void DisableDocActions();
    void EnableDocActions();

public slots:
    void doOpenFile(const QString& path);
    void openFile();
    void closeDocumentTab(int index);
    void doCloseDocumentTab(int index);

private:
    //Ui::MainWindow *ui;
    QToolBar* mToolBar;
    QStackedWidget* mCenterWidget;
    WelcomePage* mWelcomePage;
    DocumentTab* mLastDocument{nullptr};
    QTabWidget* mTabWidget;
    RecentFile mRecentFile{"recentFile", 5};
    // QMenu* mCodeMenu;

};
#endif // MAINWINDOW_H
