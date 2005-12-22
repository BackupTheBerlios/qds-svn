TEMPLATE  = lib
CONFIG   += qt warn_on thread

HEADERS  += dbusplugin.h \
            dbusplugin_p.h \
            dbuslauncherimpl.h

SOURCES  += dbusplugin.cpp \
            dbuslauncherimpl.cpp

LANGUAGE  = C++

TARGET    = qds-dbus
VERSION   = 0.1.0

INCLUDEPATH += ../../include ../../

DBUSDIR = $$(DBUSDIR)
isEmpty(DBUSDIR) {
    error( Please set DBUSDIR to your D-BUS installation prefix )
}

INCLUDEPATH += $(DBUSDIR)/include/dbus-1.0/qt3 $(DBUSDIR)/include/dbus-1.0

LIBS += -L$(DBUSDIR)/lib -ldbus-qt3-1 -ldbus-1

