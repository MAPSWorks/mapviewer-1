#-------------------------------------------------
#
# Project created by QtCreator 2012-12-05T11:11:48
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mapview
TEMPLATE = app

MOC_DIR = .moc
OBJECTS_DIR = .obj
SOURCES += main.cpp\
        widget.cpp \
#    mapitem.cpp \
    maptile.cpp \
    quadtree.cpp \
    tilegenerator.cpp \
    controllers.cpp

HEADERS  += widget.h \
#    mapitem.h \
    maptile.h \
    quadtree.h \
    tilegenerator.h \
    misc.h \
    controllers.h \
    layerinterface.h \
    mutexlocker.h
