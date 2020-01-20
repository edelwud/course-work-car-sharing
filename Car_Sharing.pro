#-------------------------------------------------
#
# Project created by QtCreator 2019-10-12T19:01:16
#
#-------------------------------------------------

QT += core gui widgets

TARGET = Car_Sharing
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    AuthController.cpp \
    addcarform.cpp \
        main.cpp \
        mainwindow.cpp \
    Database.cpp \
    Car.cpp \
    Order.cpp \
    User.cpp \
    EventManager.cpp \
    carwidget.cpp \
    loginform.cpp \
    registerform.cpp \
    careditform.cpp \
    usereditform.cpp \
    userform.cpp

HEADERS += \
    AuthController.h \
    Controller.h \
    Manager.h \
    addcarform.h \
        mainwindow.h \
    Database.h \
    Car.h \
    Event.h \
    Package.h \
    Order.h \
    User.h \
    EventManager.h \
    carwidget.h \
    loginform.h \
    registerform.h \
    careditform.h \
    usereditform.h \
    userform.h

FORMS += \
    addcarform.ui \
        mainwindow.ui \
    carwidget.ui \
    loginform.ui \
    registerform.ui \
    careditform.ui \
    usereditform.ui \
    userform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../../Рабочий стол/Images/19MerAcl180Sport5drGryHO2_800.jpg \
    ../../Рабочий стол/Images/19MerAcl180Sport5drGryFR1_800.jpg \
    class_design.qmodel

RESOURCES += \
    resources.qrc

STATECHARTS +=
