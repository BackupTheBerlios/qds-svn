TEMPLATE = lib

CONFIG += qt thread debug warn_on

OBJECTS_DIR = .obj

TARGET = qds_gnome
VERSION = 0.1.0

HEADERS += gnomevfslauncherimpl.h

SOURCES += gnomeinit.cpp gnomevfslauncherimpl.cpp

INCLUDEPATH += ../../

QMAKE_CXXFLAGS_DEBUG += $$system(pkg-config --cflags gnome-vfs-2.0)
QMAKE_CXXFLAGS_RELEASE += $$system(pkg-config --cflags gnome-vfs-2.0)

LIBS += $$system(pkg-config --libs gnome-vfs-2.0)
              
QMAKE_CXXFLAGS_DEBUG += $$system(pkg-config --cflags gnome-vfs-module-2.0)
QMAKE_CXXFLAGS_RELEASE += $$system(pkg-config --cflags gnome-vfs-module-2.0)

LIBS += $$system(pkg-config --libs gnome-vfs-module-2.0)
              
DESTDIR = ../lib