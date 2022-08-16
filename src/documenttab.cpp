#include <QDebug>
#include <memory>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QtConcurrent>
#include <Qlabel>
#include <QSpinBox>
#include <QDir>
#include <QMenu>
#include "sublog.h"
#include "documenttab.h"
#include "controller.h"
#include "highlight.h"

using namespace std;

DocumentTab::DocumentTab()
{
    qDebug()<<"docTab constructing";
    //mFocusManager = new FocusManager;
    auto hsplitter = new QSplitter();

    mLogSplitter = new QSplitter();
    mLogSplitter->setOrientation(Qt::Vertical);
    
    connect(&mFocusManager, &FocusManager::focusChanged, [this](QWidget* old, QWidget* now){
            if (old) {
                old->repaint();
                // Repaints the widget directly by calling paintEvent() immediately, unless updates are disabled or the widget is hidden
            }
            if (!now->hasFocus()) {
                now->setFocus();
            }
            qDebug()<< "doc focus changed";

            loadCurrentTaglist(currentEdit());;
//            loadTagListWithCurrentLogEdit(currentLogEdit());
//            // 操作过后更新操作历史<前进>\<后退>
//            updateHistoryBtn(currentLogEdit());

        });
    mLogViewer = new LogViewer(&mFocusManager);
    mFocusManager.add(mLogViewer->getEdit());
    mLogSplitter->addWidget(mLogViewer);
 
    mSubLogTabWidget = new QTabWidget();
    mSubLogTabWidget->setTabsClosable(true);
    mLogSplitter->addWidget(mSubLogTabWidget);
    mSubLogTabWidget->hide();
    connect(mSubLogTabWidget, &QTabWidget::tabCloseRequested, this, &DocumentTab::closeSubLog);
    
//    auto tab = new LogViewer(&mFocusManager);
//    auto index = mSubLogTabWidget->addTab(tab,"sub test");
//    mSubLogTabWidget->setCurrentIndex(index);

    mLogSplitter->setStretchFactor(0, 2);
    mLogSplitter->setStretchFactor(1, 1);


    hsplitter->addWidget(mLogSplitter);
    mTimeLine = new TimeLine;
    mTimeLine->hide();
    hsplitter->addWidget(mTimeLine);
     connect(mTimeLine, &TimeLine::nodeSelected, [this](TimeNode* node){
        auto rootLog = mLogViewer->getEdit()->getLog();
        qApp->postEvent(this, new EmphasizeLineEvent(rootLog, node->lineNumber(), false));
    });

    hsplitter->setStretchFactor(0, 2);
    hsplitter->setStretchFactor(1, 1);

    auto box = new QHBoxLayout;
    box->addWidget(hsplitter);
    box->setMargin(0);
    setLayout(box);
    //bindController();

}

void DocumentTab::bindEditor(LogEdit* edit,bool isSub)
{
    connect(edit->highlighter(), &Highlighter::onPatternAdded, [](HighlightPattern p){

        auto tagList = Controller::instance().tagList();
        tagList->addTag(p.key,p.color);
    });
    // connect(edit, &LogEdit::addToTimeLineRequested, [this, edit](int lineNum){
    //     auto log = edit->getLog();

    //     auto sourceLine = log->toRootLine(lineNum);
    //     auto text = log->readLine(sourceLine);
    //     auto time = searchTime(sourceLine,edit);
    //     qDebug()<<"time:"<< time;
    //     if(!time.isEmpty()){
    //         auto len = text.size();
    //         text = text.right(len-25);
    //     }
    //     mTimeLine->addNode(sourceLine, text,time);
    // });
    connect(edit, &LogEdit::menuRequested,
            [this, edit,isSub](QPoint point, const QString& cursorWord, int lineNum){
        auto menu = new QMenu;
        auto a = menu->addAction("Add to Timeline");
        connect(a, &QAction::triggered, [this, edit, lineNum]{
            auto log = edit->getLog();

            auto sourceLine = log->toRootLine(lineNum);
            auto text = log->rootLog()->readLine(sourceLine);
            auto time = searchTime(sourceLine,edit);

            qDebug()<<"time:"<< time;

            auto contentRx =  Controller::instance().getContent();
            //auto len = text.size();
            //text = text.right(len-25);
            text.indexOf(contentRx);
            if(contentRx.capturedTexts()[0].isEmpty()){
               text = contentRx.capturedTexts()[0];
            }
            mTimeLine->addNode(sourceLine, text,time);
        });

        if (!cursorWord.isEmpty()) {
            menu->addSeparator();

            auto showWord=cursorWord;
            if (cursorWord.length() > 15)
                showWord = cursorWord.mid(0, 12)+"...";

            a = menu->addAction("Highligt "+showWord);
            connect(a, &QAction::triggered, [edit, cursorWord]{
                edit->highlighter()->quickHighlight(cursorWord);
            });

            a = menu->addAction("Search "+showWord);
            connect(a, &QAction::triggered, [cursorWord]{
                auto searchBar = Controller::instance().searchBar();
                searchBar->fill(cursorWord);
                searchBar->buildSearchRequest();
            });

            a = menu->addAction("Filter "+showWord);
            connect(a, &QAction::triggered, [this, cursorWord]{
                SearchArg arg;
                arg.pattern = cursorWord;
                arg.caseSensitive = true;
                doFilter(arg);
            });

            a = menu->addAction("Revert Filter "+showWord);
            connect(a, &QAction::triggered, [this, cursorWord]{
                SearchArg arg;
                arg.pattern = cursorWord;
                arg.revert = true;
                doFilter(arg);
            });
        }

        a = menu->addAction("Remove Search Highlight");//更好的交互方式
        connect(a, &QAction::triggered, edit->highlighter(), &Highlighter::clearSearchHighlight);

        menu->addSeparator();
        a = menu->addAction("Copy");
        connect(a, &QAction::triggered, edit, &LogEdit::copy);
        if (isSub){
            menu->addSeparator();
            a = menu->addAction("Export File...");
            connect(a,&QAction::triggered,[this,edit]{
                auto log = edit->getLog();
                auto savepath = QFileDialog::getSaveFileName(this, "Export Filter", QString(), "*.log");
                if (!savepath.isEmpty()){

                    auto hint = "[this file is Filtered from: " + mName;
                    auto range = log->availRange();
                    QtConcurrent::run([this,range,log,hint,savepath]{
                        QFile filter;
                        filter.setFileName(savepath);
                        if(!filter.open(QIODevice::ReadWrite|QIODevice::Text)){
                            QMessageBox::warning(this, "ERROR", "Failed To Open File", QMessageBox::Ok);
                            return ;
                        }
                        filter.write(hint.toStdString().c_str());
                        for(int line = range.from;line <= range.to; line++){
                            auto text = log->readLine(line);
                            filter.write(text.toStdString().c_str());
                        }
                        filter.close();
                    });
                }

            });
        }
        menu->exec(point);
    }
    );

}

void DocumentTab::bindController()
{
    auto action = Controller::instance();

    mConnections.push_back(
                connect(
                    action.searchBar(),
                    &SearchBar::searchRequest,
                    this,
                    &DocumentTab::find
                    )
                );
    mConnections.push_back(
                connect(
                    action.actionFor(Controller::SplitFile),
                    &QAction::triggered,
                    this,
                    &DocumentTab::splitFile
                    )
                );
    mConnections.push_back(
                connect(
                    action.actionFor(Controller::LocateLine),
                    &QAction::triggered,
                    this,
                    &DocumentTab::locateLine
                    )
                );
    mConnections.push_back(
                connect(
                    action.actionFor(Controller::Highlight),
                    &QAction::triggered,
                    this,
                    &DocumentTab::addHighlight
                    )
                );
    mConnections.push_back(
                connect(
                    action.actionFor(Controller::FindNext),
                    &QAction::triggered,
                    action.searchBar(),
                    &SearchBar::forwardBtnClicked
                    )
                );
    mConnections.push_back(
                connect(
                    action.actionFor(Controller::FindPrevious),
                    &QAction::triggered,
                    action.searchBar(),
                    &SearchBar::backwardBtnClicked
                    )
                );
    mConnections.push_back(
                connect(
                    action.actionFor(Controller::Find),
                    &QAction::triggered,
                    this,
                    &DocumentTab::findKeyword
                    )
                );
    mConnections.push_back(
                connect(
                    action.actionFor(Controller::Filter),
                    &QAction::triggered,
                    this,
                    &DocumentTab::filter
                    )
                );
    mConnections.push_back(
                connect(
                    action.actionFor(Controller::RevertFilter),
                    &QAction::triggered,
                    this,
                    &DocumentTab::revertFilter
                    )
                );
    mConnections.push_back(
                connect(
                    action.actionFor(Controller::SaveTimeLine), 
                    &QAction::triggered,
                    this,
                    &DocumentTab::exportTimeLineToImage
                    )
                );
    mConnections.push_back(
                connect(
                    action.actionFor(Controller::ClearTimeLine), 
                    &QAction::triggered, 
                    mTimeLine, 
                    &TimeLine::clear
                    )
                );
    mConnections.push_back(
                connect(
                    action.actionFor(Controller::SetPattern), 
                    &QAction::triggered, 
                    this, 
                    &DocumentTab::setPattern
                    )
                );
    

    bindTaglist();
}

void DocumentTab::unBindController()
{
    for (auto& conn : mConnections)
        disconnect(conn);
    mConnections.clear();
}

void DocumentTab::bindTaglist()
{
    //taglist
    auto taglist = Controller::instance().tagList();
    mConnections.push_back(connect(taglist, &TagListWidget::onTagDeleted, [this](const QString& text){
        currentEdit()->highlighter()->clearQuickHighlight(text);
    }));
//    mConnections.push_back(connect(taglist, &TagListWidget::requestSearchTag, [](const QString& keyword){
//        auto searchBar = Controller::instance().searchBar();
//        qDebug()<< "Seach Tag"<< keyword;
//        searchBar->fill(keyword);
//        searchBar->buildSearchRequest();
//    }));
    mConnections.push_back(connect(taglist, &TagListWidget::onTagColorChanged, [this](const QString& keyword, QColor color){
        //sqDebug()<< "change color"<< keyword;
        currentEdit()->highlighter()->quickHighlight(keyword, color);
    }));
    mConnections.push_back(connect(taglist, &TagListWidget::requestFilterTag, this, &DocumentTab::doFilter));
    loadCurrentTaglist(currentEdit());
}

void DocumentTab::loadCurrentTaglist(LogEdit* editor)
{
    auto taglist = Controller::instance().tagList();
    taglist->clear();
    for (auto& p : editor->highlighter()->allQuickHighlights()) {
          taglist->addTag(p.key, p.color);
      }

}

void DocumentTab::addHighlight()
{
    QCoreApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
    // 关闭  "?"  按钮
    bool ok = false;
    QString text = QString("");
    while(true){
            text =QInputDialog::getText(this, tr("Highlight"),
                    tr("Key Word:"), QLineEdit::Normal,
                   "", &ok);
            if ((!ok )||(ok && !text.isEmpty()))
            {
                break;
            }
    }
    if(text.isEmpty()){
        return;
    }
    currentEdit()->highlighter()->quickHighlight(text);

}

void DocumentTab::locateLine()
{
    auto edit = currentEdit();
    auto log = edit->getLog();
    auto range = log->availRange();
    QCoreApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
    // 关闭  "?"  按钮
    bool ok = false;
    auto lineNum = QInputDialog::getInt(this,
            "Jump",
            QString("Range:%1 - %2").arg(range.from).arg(range.to),
            range.from,
            range.from, range.to, 1,
            &ok);

    if (ok) {
        qDebug()<< "Locate :"<< lineNum;
        edit->scrollToLine(lineNum);
    }
}

void DocumentTab::setPattern()
{
    QCoreApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
    // 关闭  "?"  按钮
    //bool ok = false;
    QDialog dialog(this);
    QFormLayout form(&dialog);
    form.addRow(new QLabel("Do Not Forget Escape Characters In C-String!"));
    // Value1
    QString time = QString("Time Reg: ");
    QLineEdit *timeInput =new QLineEdit(&dialog);
    form.addRow(time, timeInput);
    // Value2
    QString content = QString("Content Rex : ");
    QLineEdit *contentInput = new QLineEdit(&dialog);

    form.addRow(content, contentInput);
    // Add Cancel and OK button
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }
    if(!timeInput->text().isEmpty())
    {
        qDebug()<<"regExp" << timeInput->text();
        auto isValid = Controller::instance().setTimeRx(timeInput->text());
        if (!isValid){
            QMessageBox::warning(this, "ERROR", "Time Pattern Is Not Valid", QMessageBox::Ok);
        }
    }
    if(!contentInput->text().isEmpty())
    {
        qDebug()<<"regExp" << contentInput->text();
        auto isValid = Controller::instance().setContentRx(contentInput->text());
        if (!isValid){
            QMessageBox::warning(this, "ERROR", "Content Pattern Is Not Valid", QMessageBox::Ok);
        }
    }


}

void DocumentTab::splitFile()
{
    auto edit = currentEdit();
    auto log = edit->getLog();
    auto range = log->availRange();
    QCoreApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
    // 关闭  "?"  按钮
    //bool ok = false;
    QDialog dialog(this);
    QFormLayout form(&dialog);
    //form.addRow(new QLabel("Split Range:"));
    // Value1
    QString start = QString("Start Line: ");
    QSpinBox *startInput = new QSpinBox(&dialog);
    startInput->setMaximum(range.to);
    startInput->setMinimum(range.from);
    form.addRow(start, startInput);
    // Value2
    QString end = QString("End line: ");
    QSpinBox *endInput = new QSpinBox(&dialog);
    endInput->setMaximum(range.to);
    endInput->setMinimum(range.from);
    form.addRow(end, endInput);
    // Add Cancel and OK button
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    while (true) {
        if (dialog.exec() == QDialog::Rejected)
        {
            return;
        }

        else if (startInput->value() >= endInput->value()){
            QMessageBox::warning(this, "Illegal input", "Start Should Smaller Than End", QMessageBox::Ok);
        }
        else{
            qDebug()<<"Split Range ok" ;
            break;
        }
        qDebug()<<"input range" <<startInput->value()<<" "<<endInput->value();
    }
//    if (ok) {
//        qDebug()<< "Locate :"<< lineNum;
//        edit->scrollToLine(lineNum);
//    }
    auto savepath = QFileDialog::getSaveFileName(this, "Spilt file", QString(), "*.log");
    if (savepath.isEmpty())
        return;
    int from = startInput->value();
    int to = endInput->value();
    QtConcurrent::run([this,from,to,savepath,log]{
        auto hint = "[this file is splitted from: "+mName +": from "+QString::number(from)+" to "+QString::number(to)+"\n";
        QFile splitted;
        splitted.setFileName(savepath);
        if (!splitted.open(QIODevice::ReadWrite|QIODevice::Text)){
            QMessageBox::warning(this, "ERROR", "Failed To Open File", QMessageBox::Ok);
            return;
        }
        //splitted.open(QIODevice::ReadWrite|QIODevice::Text);
        //不存在的情况下，打开包含了新建文件的操作
        splitted.write(hint.toStdString().c_str());
        for (int line = from; line <= to; line++){
            auto text = log->readLine(line);
            splitted.write(text.toStdString().c_str());
        }
    //    splitted.write("[this file is splitted from: ");
    //    splitted.write(mName.toStdString().c_str());

        splitted.close();
        emit fileSplitted(savepath);
    });
    
}


void DocumentTab::findKeyword()
{
    auto searchBar = Controller::instance().searchBar();
    QCoreApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
    // 关闭  "?"  按钮
    bool ok = false;
    QString keyword = QString("");
    while(true){
            keyword =QInputDialog::getText(this, tr("Find"),
                    tr("Key Word:"), QLineEdit::Normal,
                   "", &ok);
            if ((!ok )||(ok && !keyword.isEmpty()))
            {
                break;
            }
    }
    qDebug()<<"keyword:"<< keyword;
    if(keyword.isEmpty()){
        return;
    }
    searchBar->setSearchForward(true);
    searchBar->setCombo(keyword);
    searchBar->strongFocus();
    searchBar->buildSearchRequest();

}

void DocumentTab::exportTimeLineToImage()
{
    auto savepath = QFileDialog::getSaveFileName(this, "Export Timeline", QString(), "*.png");
    if (savepath.isEmpty())
        return;
    if (!savepath.endsWith(".png"))
        savepath+=".png";

    mTimeLine->exportToImage(savepath);
}

LogEdit* DocumentTab::currentEdit()
{
    qDebug() <<  (LogEdit*)mFocusManager.current()<< "==" << mLogViewer->getEdit() ;
    return (LogEdit*)mFocusManager.current();
}
void DocumentTab::init(shared_ptr<ILogSource> log)
{
    mLogSource = log;
    mLogViewer->getEdit()->setLog(mLogSource->getLog());
    bindEditor(mLogViewer->getEdit());
}

void DocumentTab::unInit()
{
    if (mLogSource)
            mLogSource->close();
}

void DocumentTab::onLogChange(shared_ptr<ILog> who, EventAction update)
{
    auto before = who->availRange();
    update();
    auto after = who->availRange();

    auto edit = findLogEditByLog(who);
    if (edit)
        edit->onLogRangeChanged();

    for (auto&& child: who->children()) {
        deepHandleLogChange(child, before, after);
    }
}

void DocumentTab::deepHandleLogChange(const shared_ptr<ILog> &log, Range before, Range after)
{
    auto myBefore = log->availRange();
    if (!log->onParentRangeChanged(before, after))
        return;
    auto myAfter = log->availRange();

    auto edit = findLogEditByLog(log);
    if (edit) {
        edit->onLogRangeChanged();
    }

    for (auto&& child : log->children()) {
        deepHandleLogChange(child, myBefore, myAfter);
    }
}

void DocumentTab::onSubLogCreated(shared_ptr<SubLog> subLog)
{
    if (!subLog->availRange().isValid()) {
        if (QMessageBox::Ok
                != QMessageBox::warning(this, "Search Result", "No Matched Text", QMessageBox::Ok, QMessageBox::Cancel)) {
            return;
        }
    }

    auto viewer = new LogViewer(&mFocusManager);

    auto subEdit = viewer->getEdit();

    bindEditor(subEdit,true);
    QString revert = subLog->getSearchArg().revert? "_revert" : "";

    appendSubLog(viewer, subLog->getSearchArg().pattern+revert);

    subEdit->setLog(subLog);
}


void DocumentTab::onFindDone(shared_ptr<ILog> who, SearchResult ret)
{

    qDebug()<< who.get();
    if (ret.matchedText.isEmpty()) {
        if (ret.line <= 1) {
            QMessageBox::warning(this, "Search", "Reach The Top,No Matched Result");
        } else {
            QMessageBox::warning(this, "Search", "Reach The Bottom,No Matched Result");
        }
        return;
    }
    auto edit = findLogEditByLog(who);
    if (edit) {
        edit->onLogFindOne(ret);
    }
}

LogEdit *DocumentTab::findLogEditByLog(const shared_ptr<ILog> &log)
{
    if (mLogViewer->getEdit()->getLog() == log) {
        return mLogViewer->getEdit();
    }

    auto cnt = mSubLogTabWidget->count();
    for (auto i = 0; i < cnt; i++) {
        auto viewer = (LogViewer*)mSubLogTabWidget->widget(i);
        if (viewer->getEdit()->getLog() == log) {
            return viewer->getEdit();
        }
    }

    return nullptr;
}

void DocumentTab::customEvent(QEvent *ev)
{
    auto type = (EventsType)ev->type();
    qDebug()<< type;
    switch (type) {
    case evLogChange: {
        auto e = (LogChangeEvent*)ev;
        onLogChange(e->who, e->updateLog);
        ev->accept();
        break;
    }
    case evSubLogCreated:{
        qDebug() << "recive Sub Create :" << ev;
        auto e = (SubLogCreatedEvent*)ev;
        onSubLogCreated(e->log);
        ev->accept();
        break;
    }
    case evEmphasizeLine:{
        auto e = (EmphasizeLineEvent*)ev;
        onLineEmphasized(e->who, e->line, e->isSource);
        ev->accept();
        break;
    }
    case evFindDone:{
        auto e = (FindDoneEvent*)ev;
        onFindDone(e->who, e->ret);
        ev->accept();
        break;
    }
    case evError:{
        auto e = (ErrorEvent*)ev;
        e->cleanup();
        ev->accept();
        break;
    }
    // case evSourceReady: {
    //     ev->accept();
    //     break;
    // }
    // case evSourceFinish:{
    //     //Controller::instance().actionFor(Controller::PauseSource)->setEnabled(false);
    //     ev->accept();
    //     break;
    // }
    default:
        ev->ignore();
        break;
    }

//    if (mLoadingPharse) {
//        if (!mLoadingPharse->process(ev)) {
//            mLoadingPharse.reset();
//        }
//    }
}

void DocumentTab::find(SearchArg arg, bool forward)
{
    auto* logEdit = currentEdit();
    
    qDebug()<<"find:"<< arg.pattern<<" "<< logEdit->documentTitle();
    logEdit->find(arg, forward);
    
    //modalLongOp(op, "查找中……可能需要较长时间");
}

void DocumentTab::appendSubLog(LogViewer* logViewer, const QString& hint)
{
    mFocusManager.add(logViewer->getEdit());
        mSubLogTabWidget->addTab(logViewer, hint);
        mSubLogTabWidget->setCurrentWidget(logViewer);
        if (mSubLogTabWidget->count() == 1) {
            mSubLogTabWidget->show();
        }
}

void DocumentTab::closeSubLog(int index)
{
    auto subViewer = (LogViewer*)mSubLogTabWidget->widget(index);
    mFocusManager.remove(subViewer->getEdit());
    mSubLogTabWidget->removeTab(index);
    delete subViewer;

    if (mSubLogTabWidget->count() == 0) {
        mSubLogTabWidget->hide();
    }

}

void DocumentTab::doFilter(SearchArg arg)
{
    qDebug()<<"filter"<<arg.pattern;
    auto log = currentEdit()->getLog();
    log->createChild(arg);
}

void DocumentTab::revertFilter()
{
    qDebug()<< "revert filter trigger";
    FilterDialog dlg;
    dlg.setWindowTitle("Revert Filter");
    while(true){
        if (dlg.exec() == QDialog::Rejected)
        {
            return;
        }

        if (!dlg.getArg().pattern.isEmpty()){
            break;
        }
    }
    auto arg = dlg.getArg();
    arg.revert = true;
    doFilter(arg);

}

void DocumentTab::filter()
{
    qDebug()<< "filter trigger";
    FilterDialog dlg;
    dlg.setWindowTitle("Filter");
    while (true) {
        if (dlg.exec() == QDialog::Rejected)
        {
            return;
        }
        if(!dlg.getArg().pattern.isEmpty()){
            break;
        }
    }
    doFilter(dlg.getArg());

}

void DocumentTab::onLineEmphasized(shared_ptr<ILog> who, int line, bool isSource)
{
    auto rootLine = who->toRootLine(line);
    auto root = who->rootLog();

    // if (!root->availRange().contains(rootLine) || rootLine <= 0) {
    //     QMessageBox::warning(this, "跳转错误", "无法跳转到对应行，可能日志已经被刷掉了");
    //     return ;
    // }

    //从上到下，触发emphasize
    deepEmphasizeLine(root, rootLine, isSource ? who : shared_ptr<ILog>());
}

void DocumentTab::deepEmphasizeLine(const shared_ptr<ILog> &log, int line, const shared_ptr<ILog> &source)
{
    //高亮自己
    if (log != source) {//只在非源的log上刷新。否则会出现双击某个文本框时，自己这个文本框的高亮行跳到了第一行位置
        auto edit = findLogEditByLog(log);
        if (edit)
            edit->onLogEmphasized(line);
    }

    //高亮子节点
    auto children = log->children();
    for (auto&& child : children) {
        deepEmphasizeLine(child, child->fromParentLine(line), source);
    }
}


QString DocumentTab::searchTime(int sourceLine,LogEdit* edit){



    auto timeRx =  Controller::instance().getTime();
    bool defaultRx = true;
    if (timeRx != QRegExp("\\[[0-9]{4}-[0-1][0-9]-[0-3][0-9]\\s[0-2][0-9]:[0-6][0-9]:[0-6][0-9].[0-9]{3}\\]")){
        defaultRx = false;
    }
    auto log = edit->getLog();
    int idx = -1;
    while(idx <0 && sourceLine > 0){
        auto context = log->rootLog()->readLine(sourceLine);
        idx = context.indexOf(timeRx);
        sourceLine --;
    }
    QString time = "";
    if (!timeRx.capturedTexts()[0].isEmpty()){
        time = timeRx.capturedTexts()[0];
        if (defaultRx){
            auto first = time.mid(1,10);
            auto second = time.mid(12,12);
            time = first+"\n"+second;
        }
        time = "\n"+time;
    }


    //time=QString( "\n2022-07-25\n15:59:33.018");
    return time;
}


FilterDialog::FilterDialog()
{
    QCoreApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);

    auto box = new QVBoxLayout;

    auto patternEdit = new QLineEdit;
    box->addWidget(patternEdit);
    auto checkbox = new QHBoxLayout;
    auto caseSensitiveCheckBox = new QCheckBox("Aa");
    caseSensitiveCheckBox->setToolTip("区分大小写");
    checkbox->addWidget(caseSensitiveCheckBox);

    auto useRegexCheckBox = new QCheckBox(".*");
    useRegexCheckBox->setToolTip("使用正则表达式");
    checkbox->addWidget(useRegexCheckBox);
    box->addLayout(checkbox);
    auto buttons = new QHBoxLayout;
    auto cancelBtn = new QPushButton("Cancel");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    buttons->addWidget(cancelBtn);

    auto okBtn = new QPushButton("Ok");
    connect(patternEdit, &QLineEdit::textChanged, [okBtn](const QString& text){
        okBtn->setEnabled(!text.isEmpty());
    });
    okBtn->setDefault(true);
    connect(okBtn, &QPushButton::clicked, [this, caseSensitiveCheckBox, useRegexCheckBox, patternEdit]{
        mArg.caseSensitive = caseSensitiveCheckBox->isChecked();
        mArg.regex = useRegexCheckBox->isChecked();
        mArg.pattern = patternEdit->text();
        accept();
    });
    buttons->addWidget(okBtn);

    box->addLayout(buttons);
    setLayout(box);
}


