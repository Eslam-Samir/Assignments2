#-------------------------------------------------
#
# Project created by QtCreator 2016-12-07T19:51:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Parser
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Parser.cpp \
    Scanner.cpp \
    treewindow.cpp \
    treenode.cpp

HEADERS  += mainwindow.h \
    Scanner.h \
    Parser.h \
    treewindow.h \
    treenode.h

FORMS    += mainwindow.ui \
    treewindow.ui
