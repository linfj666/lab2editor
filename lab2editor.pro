QT += widgets

CONFIG += c++17

TEMPLATE = app
TARGET = lab2editor

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    codeeditor.cpp \
    finddialog.cpp \
    replacedialog.cpp

HEADERS += \
    mainwindow.h \
    codeeditor.h \
    finddialog.h \
    replacedialog.h

FORMS += \
    mainwindow.ui \
    finddialog.ui \
    replacedialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
