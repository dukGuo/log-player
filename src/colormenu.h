#ifndef COLORMENU_H
#define COLORMENU_H

#include <QMenu>
#include <QList>
#include <QColor>
#include <utility>

class ColorMenu : public QMenu
{
    Q_OBJECT

public:
    ColorMenu(const QList<std::pair<QString,QColor>>& preset);

signals:
    void chooseColor(QColor color);
};

#endif // COLORMENU_H
