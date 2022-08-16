#include <QScrollBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QGridLayout>
#include <QToolBar>
#include <QAction>
#include "logviewer.h"

LogViewer::LogViewer(FocusManager* focusManager)
{

    auto vbar = new QScrollBar;
    mLogEdit = new LogEdit(focusManager);
    mLogEdit->setExternalVScrollbar(vbar);
    //mLogEdit->setPlainText(QString("test in LogViewer"));
    auto layout = new QHBoxLayout;
    layout->addWidget(mLogEdit);
//    auto box = new QGridLayout();
//    box->addWidget(vbar,0,0,Qt::AlignHCenter);
//    //box->addWidget(buildSwitchModeButton(),1,0,Qt::AlignCenter);
//    box->setRowStretch(0,1);
    layout->addWidget(vbar);

    layout->setSpacing(0);
    layout->setMargin(0);

    setLayout(layout);

}
