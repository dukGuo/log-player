#include "colormenu.h"
#include <QColorDialog>

ColorMenu::ColorMenu(const QList<std::pair<QString,QColor>>& preset)
{
       setTitle("Set Color");

       for (int i = 0; i < preset.size(); i++) {
           auto name = preset[i].first;
           auto color = preset[i].second;
           QPixmap pixmap(10,10);
           pixmap.fill(color);
           connect(addAction( name), &QAction::triggered, [this,color]{
              emit chooseColor(color);
           });
       }

       connect(addAction("More Color..."), &QAction::triggered, [this]{
           auto color = QColorDialog::getColor();
           if (color.isValid()) {
               emit chooseColor(color);
           }
       });
}
