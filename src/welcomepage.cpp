#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QDebug>
#include <QString>
#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include "controller.h"
#include "welcomepage.h"

WelcomePage::WelcomePage(MainWindow *parent)
    :QWidget((QWidget*)parent), mMainWnd(parent)
{


}
void WelcomePage::showEvent(QShowEvent *)
{

    auto old = layout();
    if (old)
        QWidget().setLayout(old);

    auto box = new QGridLayout;
    box->setMargin(30);
    box->addWidget(createLabel("打开本地文件..."), 1,0);

    box->setRowMinimumHeight(1, 50);
    setLayout(box);
}

QPushButton* WelcomePage::createLabel(QString text)
{
    auto btn = new QPushButton;
    if (text == "打开本地文件...")
    {
        connect(btn,&QPushButton::clicked,[](){
                Controller::instance().actionFor(Controller::OpenFile)->trigger();
        });
    }
    else{
        assert(false);
    }
    btn->setText(text);
    btn->setFixedSize(200,30);
    return btn;
}
