AUTOMAKE_OPTIONS = foreign 1.5

EXTRA_DIST = admin LICENCE

dist-hook:
	cd $(top_distdir) && perl $(top_srcdir)/admin/am_edit -padmin

include admin/Doxyfile.am
include admin/deps.am

SUBDIRS=$(TOPSUBDIRS)
