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
     //mTitleStyle = "style=\"color:#303030\"";
    //mWidget = new QWidget;

}
void WelcomePage::showEvent(QShowEvent *)
{
    //mWidget.set


    auto old = layout();
    if (old)
        QWidget().setLayout(old);

    auto box = new QGridLayout;
    box->setMargin(30);
    box->addWidget(createLabel("Open Local File..."), 1,0);
 //   box->addWidget(createLabel("Open Recent File"), 1, 0);
//    box->addWidget(createLabel("test3"), 0, 2);
//    box->addWidget(createLabel("test4"), 2, 2);

    box->setRowMinimumHeight(1, 50);
    setLayout(box);
}

QPushButton* WelcomePage::createLabel(QString text)
{
    auto btn = new QPushButton;
    if (text == "Open Local File..."){
        connect(btn,&QPushButton::clicked,[](){
                   // auto type = (Controller::ActionType)(Str.toInt());
                    Controller::instance().actionFor(Controller::OpenFile)->trigger();
        });
    }
//    else if (text == "Exit")
//    {

//    }
    else{


    }

    btn->setText(text);
    btn->setFixedSize(200,30);

    return btn;
}

//QWidget *WelcomePage::ActionsArea(const QString &name, const QVector<int> &actions)
//{
//    QStringList actionStrList;
//    for (auto&& type : actions) {
//        auto action = Controller::instance().actionFor((Controller::ActionType)(type));
//        if (action)
//            actionStrList.append(QString("<p><a href=%1>%2</a></p>").arg(type).arg(action->text()));
//    }

//    auto label = createLabel("te");

//    label->setText(QString("<h1 %1>%2</h1> %3").arg(mTitleStyle).arg(name).arg(actionStrList.join("")));
//    qDebug()<< label->text();
//    connect(label, &QLabel::linkActivated, [](const QString& intentStr){
//        auto type = (Controller::ActionType)(intentStr.toInt());
//        Controller::instance().actionFor(type)->trigger();
//    });

//    return label;
//}
