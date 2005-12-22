TEMPLATE = app

TARGET = qdsexample

CONFIG += qt thread warn_on debug

SOURCES = main.cpp mainwindow.cpp

HEADERS = mainwindow.h

FORMS = mainwindowbase.ui

LIBS += -L../lib -lqds

INCLUDEPATH += ../include

unix {
  OBJECTS_DIR = .obj
  MOC_DIR = .moc
  UI_DIR = .ui

  POST_TARGETDEPS += ../lib/libqds.a
}

win32 {
  OBJECTS_DIR = obj
  MOC_DIR = moc
  UI_DIR = ui

  POST_TARGETDEPS += ../lib/qds.lib
}

macx {
  OBJECTS_DIR = .obj
  MOC_DIR = .moc
  UI_DIR = .ui

  POST_TARGETDEPS += ../lib/libqds.a
}
