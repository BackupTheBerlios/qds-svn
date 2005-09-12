AM_CPPFLAGS = $(all_includes) -I$(top_srcdir)/..

lib_LTLIBRARIES = libqds_kde.la
libqds_kde_la_LIBADD = $(LIB_KIO)
libqds_kde_la_LDFLAGS = $(all_libraries) -version-info 1:0:1 -no-undefined
libqds_kde_la_SOURCES = kdeinit.cpp klauncherimpl.cpp knetworkprotocol.cpp

METASOURCES = AUTO
