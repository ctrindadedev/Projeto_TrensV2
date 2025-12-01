#-------------------------------------------------
# Project created by QtCreator
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Trem
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp \
           trem.cpp

HEADERS  += mainwindow.h \
            trem.h

FORMS    += mainwindow.ui

# Necessário para semáforos no Linux
LIBS += -lpthread