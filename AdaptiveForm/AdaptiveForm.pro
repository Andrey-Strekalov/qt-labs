QT += widgets

CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = AdaptiveForm
TEMPLATE = app

SOURCES += \
    adaptivefield.cpp \
    main.cpp \
    formwindow.cpp

HEADERS += \
    adaptivefield.h \
    formwindow.h