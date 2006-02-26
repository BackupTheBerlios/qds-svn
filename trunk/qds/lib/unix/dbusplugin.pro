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

QDBUS_CXXFLAGS = $$system(pkg-config --cflags dbus-1-qt3)
QMAKE_CXXFLAGS_DEBUG += $$QDBUS_CXXFLAGS
QMAKE_CXXFLAGS_RELEASE += $$QDBUS_CXXFLAGS
LIBS += $$system(pkg-config --libs dbus-1-qt3)
