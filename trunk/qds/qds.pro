TEMPLATE = subdirs

SUBDIRS = lib exampleapp

unix {
    DBUSDIR=$$(DBUSDIR)
    isEmpty(DBUSDIR) {
        error( Please set DBUSDIR to your D-BUS installation prefix )
    }

    SUBDIRS += daemon
}





