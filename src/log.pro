QT      += core gui
QT      += concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    FocusManager.cpp \
    colormenu.cpp \
    controller.cpp \
    documenttab.cpp \
    filesource.cpp \
    highlight.cpp \
    ilog.cpp \
    ilogsource.cpp \
    logedit.cpp \
    logviewer.cpp \
    main.cpp \
    mainwindow.cpp \
    memlog.cpp \
    recentfile.cpp \
    searchbar.cpp \
    sublog.cpp \
    taglistwidget.cpp \
    timeline.cpp \
    timenode.cpp \
    timetrace.cpp \
    welcomepage.cpp

HEADERS += \
    FocusManager.h \
    colormenu.h \
    controller.h \
    documenttab.h \
    event.h \
    filesource.h \
    highlight.h \
    ilog.h \
    ilogsource.h \
    logedit.h \
    logviewer.h \
    mainwindow.h \
    memlog.h \
    recentfile.h \
    searchbar.h \
    sublog.h \
    taglistwidget.h \
    timeline.h \
    timenode.h \
    timetrace.h \
    utils.h \
    welcomepage.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
