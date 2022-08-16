
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QDebug>
#include <memory>
#include <QMessageBox>
#include <QFileDialog>
#include "mainwindow.h"
#include "searchbar.h"
#include "taglistwidget.h"
#include "controller.h"
#include "filesource.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, ui(new Ui::MainWindow)
{


    buildTab();
    mCenterWidget = new QStackedWidget;
    mWelcomePage = new WelcomePage(this);
    mCenterWidget->addWidget(mWelcomePage);
    mCenterWidget->addWidget(mTabWidget);
    setCentralWidget(mCenterWidget);
    mCenterWidget->setCurrentWidget(mWelcomePage);
    
    //mCenterWidget->setCurrentWidget(mTabWidget);
    buildMenuBar();
    buildToolBar();
    bindController();
    this->resize( QSize( 300, 75 ));
    mToolBar->hide();
    DisableDocActions();
//    auto tab = new DocumentTab();
//    auto index = mTabWidget->addTab(tab, "QTest");
//    mTabWidget->setCurrentIndex(index);

//    if (mTabWidget->count() == 1) {
//        mCenterWidget->setCurrentWidget(mTabWidget);
//    }
}


MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::buildTab()
{
    mTabWidget = new QTabWidget();
    mTabWidget->setTabsClosable(true);
    connect(mTabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeDocumentTab);
    connect(mTabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeDocumentTab);
       // 切换tab时切换文档
       connect(mTabWidget, &QTabWidget::currentChanged, [this](int index){
           qDebug()<<"DocTab Index"<<index;
           // -1为没有tab
            if (mLastDocument) {
                // 没有tab时 断开所有连接
                mLastDocument->unBindController();
                mLastDocument = nullptr;
            }

            EnableDocActions();
            // 获取当前doc的tab 连接信号
            auto doc = currentDoc();
            if (doc) {
                doc->bindController();
                mLastDocument = doc;
            }
       });
}
void MainWindow::buildMenuBar()
{
    auto Bar = new QMenuBar;
    //Bar->setContextMenuPolicy(Qt::NoContextMenu);
    Bar->addMenu(buildFileMenu());
    Bar->addMenu(buildSearchMenu());
    Bar->addMenu(buildHighlightMenu());
    Bar->addMenu(buildTimelineMenu());
    setMenuBar(Bar);
}

void MainWindow::buildToolBar()
{
    auto Bar = new QToolBar("Tool");
    Bar->setAllowedAreas(Qt::TopToolBarArea|Qt::BottomToolBarArea);
    Bar->setFloatable(false);
    //Bar->setContextMenuPolicy(Qt::NoContextMenu);

    auto searchBar = new SearchBar;
    Bar->addWidget(searchBar);
    Controller::instance().setSearchBar(searchBar);

    auto taglist = new TagListWidget;
    taglist->setMinimumHeight(28);
    taglist->setMaximumHeight(28);
    taglist->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    Bar->addWidget(taglist);
    Controller::instance().setTagList(taglist);
    mToolBar = Bar;
    addToolBar(Bar);
}

QMenu*   MainWindow::buildFileMenu()
{
    auto menu = new QMenu("File");
    auto action = Controller::instance();
    //menu->addMenu("Open...");
    menu->addAction(action.actionFor(Controller::OpenFile));
    auto recentFileMenu = menu->addMenu("Recent File");
    mRecentFile.mount(recentFileMenu, bind(&MainWindow::doOpenFile, this, placeholders::_1));
    menu->addAction(action.actionFor(Controller::CloseTab));
    //menu->addMenu("Settings"); // TODO  设置对话窗口
    menu->addSeparator();
    //menu->addMenu("Split File..."); // TODO  文件分割
    menu->addAction(action.actionFor(Controller::SplitFile));
    menu->addSeparator();
    menu->addAction(action.actionFor(Controller::Shortcut));
    menu->addSeparator();
    menu->addAction("Exit", this, &QMainWindow::close);
    return menu;
}
QMenu*   MainWindow::buildSearchMenu()
{
    auto menu = new QMenu("Search");
    auto action = Controller::instance();
    //menu->addMenu("Find");
    menu->addAction(action.actionFor(Controller::Find));
    //menu->addMenu("Next");
    menu->addAction(action.actionFor(Controller::FindNext));
    //menu->addMenu("Previous");
    menu->addAction(action.actionFor(Controller::FindPrevious));
    menu->addSeparator();
    //menu->addMenu("Jump To");
    menu->addAction(action.actionFor(Controller::LocateLine));
    return menu;

}
QMenu*   MainWindow::buildHighlightMenu()
{
    auto menu = new QMenu("Highlight");
    // menu->addMenu("Add Rules"); 
    auto action = Controller::instance();
    menu->addAction(action.actionFor(Controller::Highlight));
    menu->addSeparator();
    // menu->addMenu("Filter");
    menu->addAction(action.actionFor(Controller::Filter));
    //menu->addMenu("RFilter");
    menu->addAction(action.actionFor(Controller::RevertFilter));

    // TODO 对话窗口
    return menu;
}
QMenu*   MainWindow::buildTimelineMenu()
{
    auto menu = new QMenu("Timeline");
    auto action = Controller::instance();
    menu->addAction(action.actionFor(Controller::SetPattern));
    //menu->addMenu("Export Timeline");
    menu->addAction(action.actionFor(Controller::SaveTimeLine));
    //menu->addMenu("Clear Timeline");
    menu->addAction(action.actionFor(Controller::ClearTimeLine));
    return menu;
}

void MainWindow::bindController()
{
    auto a = Controller::instance();
    connect(a.actionFor(Controller::OpenFile), &QAction::triggered, this, &MainWindow::openFile);
    connect(a.actionFor(Controller::CloseTab), &QAction::triggered, [this]{
        auto index = mTabWidget->currentIndex();
        this->closeDocumentTab(index);
    });

    connect(a.actionFor(Controller::Shortcut), &QAction::triggered, []{
        auto shortcuts = Controller::instance().getShortcutHint();
        QMessageBox::information(nullptr, "shortcuts", shortcuts);
    });

}



void MainWindow::closeDocumentTab(int index)
{
    if (index <0 || index >= mTabWidget->count()) {
            return;
        }

        //QSettings config;
        int btn = QMessageBox::Ok;
        //if (config.value("closeTabPrompt").toBool()) {
        auto tabText = mTabWidget->tabText(index);
        btn = QMessageBox::warning(this,
                                        "Close log", QString("Make Sure To Close %1").arg(tabText),
                                        QMessageBox::Ok,
                                        QMessageBox::Cancel);
        //}

        if (btn == QMessageBox::Ok) {
            doCloseDocumentTab(index);
        }

}
void MainWindow::doCloseDocumentTab(int index)
{
        qDebug()<<"Close DocTab"<<index;
        auto tab = (DocumentTab*)mTabWidget->widget(index);
        tab->unInit();
        mTabWidget->removeTab(index);
        disconnect(tab, &DocumentTab::fileSplitted, this, &MainWindow::doOpenFile);
        delete tab;

        if (mTabWidget->count() == 0) {
            mCenterWidget->setCurrentWidget(mWelcomePage);
            this->setGeometry(800,400,300,75);
            //this->resize( QSize( 300, 75 ));
            mToolBar->hide();
            DisableDocActions();
        }
}

void MainWindow::doOpenFile(const QString& path)
{
    auto tab = new DocumentTab();
    shared_ptr<FileSource> log(new FileSource(tab));
    log->setFileName(path);

    tab->setName(log->getSimpleDesc());

    if (!log->open()) {
        QMessageBox::critical(this, "Fail To Open", QString("Cannot Open %1").arg(path));
        return;
    }
    tab->init(log);
    appendDocumentTab(tab);
    mRecentFile.add(path);
}


void MainWindow::openFile()
{
    auto path = QFileDialog::getOpenFileName(this, "Open File In openFile");
    if (path.isEmpty())
        return;

    doOpenFile(path);
}

int MainWindow::appendDocumentTab(DocumentTab *tab)
{
    auto index = mTabWidget->addTab(tab, tab->getName());
    mTabWidget->setCurrentIndex(index);
    connect(tab, &DocumentTab::fileSplitted, this, &MainWindow::doOpenFile);
    if (mTabWidget->count() == 1) {
        mCenterWidget->setCurrentWidget(mTabWidget);
        this->setGeometry(400,200,1100,800);
        //this->resize( QSize( 1100, 800 ));
        mToolBar->show();
    }
    return index;
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    qDebug()<< "MainWindow closeEvent";
    auto cnt = mTabWidget->count();
    while(cnt--> 0) {
        doCloseDocumentTab(cnt);
    }
    ev->accept();
}

void MainWindow::DisableDocActions()
{
    auto actions = Controller::instance().actionsForDoc();
    for (auto action : actions)
    {
        action->setEnabled(false);
    }
}

void MainWindow::EnableDocActions()
{
    
    auto actions = Controller::instance().actionsForDoc();
    for (auto action : actions)
    {
        action->setEnabled(true);
    }
}

DocumentTab* MainWindow::currentDoc()
{
    return (DocumentTab*)mTabWidget->currentWidget();
}
