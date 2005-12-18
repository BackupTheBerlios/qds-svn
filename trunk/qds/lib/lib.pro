TEMPLATE  = lib
CONFIG   += qt warn_on thread

HEADERS  += ../include/qds/launcher.h \
            ../include/qds/qds.h \
            ../include/qds/servicefactory.h \
            servicefactoryimpl.h

SOURCES  += qds.cpp \
            servicefactory.cpp

LANGUAGE  = C++

TARGET    = qds
VERSION   = 0.5.0

INCLUDEPATH += ../include

unix {
    MOC_DIR     = .moc
    OBJECTS_DIR = .obj

    HEADERS += ../plugins/launcherimpl.h \
               unix/dbuslauncherimpl.h \
               unix/mailcap.h \
               unix/mailcaplauncherimpl.h \
               unix/mimetypes.h 

    SOURCES += unix/dbuslauncherimpl.cpp \
               unix/launcher.cpp \
               unix/mailcap.cpp \
               unix/mailcaplauncherimpl.cpp \
               unix/mimetypes.cpp \ 
               unix/servicefactoryimpl.cpp

    INCLUDEPATH += ..

    INCLUDEPATH += $(DBUSDIR)/include/dbus-1.0/qt3
    LIBS += -L$(DBUSDIR)/lib -ldbus-qt3-1 -ldbus-1
}

win32 {
    CONFIG += staticlib
    MOC_DIR     = moc
    OBJECTS_DIR = obj

    SOURCES += win32/launcher.cpp \
               win32/servicefactoryimpl.cpp
}

macx {
    CONFIG += staticlib
    MOC_DIR     = .moc
    OBJECTS_DIR = .obj

    SOURCES += macx/launcher.cpp \
               macx/servicefactoryimpl.cpp
}
