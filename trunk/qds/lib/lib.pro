TEMPLATE  = lib
CONFIG   += qt warn_on thread staticlib

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
               unix/dbusplugin.h \
               unix/mailcap.h \
               unix/mailcaplauncherimpl.h \
               unix/mimetypes.h 

    SOURCES += unix/launcher.cpp \
               unix/mailcap.cpp \
               unix/mailcaplauncherimpl.cpp \
               unix/mimetypes.cpp \ 
               unix/servicefactoryimpl.cpp

    INCLUDEPATH += ..
}

win32 {
    MOC_DIR     = moc
    OBJECTS_DIR = obj

    SOURCES += win32/launcher.cpp \
               win32/servicefactoryimpl.cpp
}

macx {
    MOC_DIR     = .moc
    OBJECTS_DIR = .obj

    SOURCES += macx/launcher.cpp \
               macx/servicefactoryimpl.cpp
}
