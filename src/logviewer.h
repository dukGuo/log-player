#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QObject>
#include <QWidget>
#include "logedit.h"
#include "FocusManager.h"
#include "utils.h"

class LogViewer : public QWidget
{

public:
    explicit LogViewer(FocusManager* focusManager);
    LogEdit* getEdit(){
        return mLogEdit;
    }
private:

    
    LogEdit* mLogEdit;

};

#endif // LOGVIEWER_H
