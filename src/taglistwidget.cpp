
#include "taglistwidget.h"
#include "colormenu.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QDebug>
#include <QContextMenuEvent>
//#include <QJsonArray>

TagListWidget::TagListWidget()
{
    setFlow(QListView::LeftToRight);
    setWrapping(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setResizeMode(QListView::Adjust);
    setSpacing(2);
    setStyleSheet("background-color: transparent");

    setContextMenuPolicy(Qt::DefaultContextMenu);
}

void TagListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    auto item = itemAt(event->x(), event->y());
    //qDebug()<< event->x() <<","<<event->y();
    if (!item)
        return;

    auto keyword = item->data(Qt::DisplayRole).toString();

    QMenu menu;
    connect(menu.addAction("Remove"), &QAction::triggered, [&]{
        qDebug()<<"Remove Tag:"<< keyword;
        takeItem(row(item));
        emit onTagDeleted(keyword);
    });

    auto chooseColorMenu = new ColorMenu({{"Red",Qt::red},{"Green",Qt::green},{"Blue",Qt::blue},{"Yellow",Qt::darkYellow}});
    connect(chooseColorMenu, &ColorMenu::chooseColor, [&](QColor color){
        auto oldColor = item->data(Qt::DecorationRole).value<QColor>();
        qDebug()<< "change" <<oldColor<<"to"<<color;
        if (color != oldColor){
            item->setData(Qt::DecorationRole, color);
            emit onTagColorChanged(keyword, color);
        }

    });
    menu.addMenu(chooseColorMenu);




    menu.exec(event->globalPos());
}

void TagListWidget::addTag(QString keyword, QColor color)
{
    int index = 0;
    qDebug()<<count();
    while(index < count()){

        auto oldKey = item(index)->data(Qt::DisplayRole).toString();
        if (keyword == oldKey){
            item(index)->setData(Qt::DecorationRole, color);
            return;
        }
        index++;
    }
    auto item = new QListWidgetItem();
    addItem(item);
    item->setData(Qt::DisplayRole,keyword);
    item->setData(Qt::DecorationRole, color);
}
