TEMPLATE = app

TARGET = qds-daemon

CONFIG += qt thread warn_on debug

DBUSDIR = $$(DBUSDIR)
isEmpty(DBUSDIR) {
    error( Please set DBUSDIR to your D-BUS installation prefix )
}

SOURCES = main.cpp daemon.cpp \
          factoryservice.cpp \
          launcherservice.cpp \
          servicefactoryimpl.cpp

HEADERS = daemon.h factoryservice.h launcherservice.h \

LIBS += -L../lib -lqds
LIBS += -L$(DBUSDIR)/lib -ldbus-qt3-1 -ldbus-1

INCLUDEPATH += ../include ../lib ../
INCLUDEPATH += $(DBUSDIR)/include/dbus-1.0/qt3

OBJECTS_DIR = .obj
MOC_DIR     = .moc
