TEMPLATE = app

TARGET = qds-daemon

CONFIG += qt thread warn_on debug

SOURCES = main.cpp daemon.cpp \
          factoryservice.cpp \
          launcherservice.cpp \
          servicefactoryimpl.cpp

HEADERS = daemon.h factoryservice.h launcherservice.h \

LIBS += -L../lib -lqds

INCLUDEPATH += ../include ../lib ../

QDBUS_CXXFLAGS = $$system(pkg-config --cflags dbus-1-qt3)
QMAKE_CXXFLAGS_DEBUG += $$QDBUS_CXXFLAGS
QMAKE_CXXFLAGS_RELEASE += $$QDBUS_CXXFLAGS
LIBS += $$system(pkg-config --libs dbus-1-qt3)

OBJECTS_DIR = .obj
MOC_DIR     = .moc
