AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = po

CFLAGS=-Wall  -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""
AM_LDFLAGS= -lm
include_HEADERS = src/thread.h src/agentmanager.h src/agent.h src/storage.h src/clientsettings.h src/guitextwidget.h src/guisettings.h src/guisources.h src/http.h src/utils.h src/webpage.h src/intelligentclient.h \
                  src/guimainwindow.h src/mpdsong.h src/mpdclient.h src/mpdlistener.h src/ClientException.h

bin_PROGRAMS = impc
impc_SOURCES = src/thread.cpp src/agentmanager.cpp src/agent.cpp  src/storage.cpp src/clientsettings.cpp src/guitextwidget.cpp src/guisettings.cpp src/guisources.cpp src/http.cpp src/utils.cpp src/webpage.cpp src/intelligentclient.cpp \
	       src/guimainwindow.cpp src/mpdsong.cpp src/mpdclient.cpp src/main.cpp
impc_CPPFLAGS = $(libxslt_CFLAGS) $(libxml_CFLAGS) $(jsoncpp_CFLAGS) $(gtkmm30_CFLAGS) $(gdkmm30_CFLAGS) -DIMPC_LOCALEDIR=\"${IMPC_LOCALEDIR}\"
impc_LDADD = $(libmpdclient_LIBS)  $(libcurl_LIBS)  $(libxslt_LIBS) $(jsoncpp_LIBS) $(gtkmm30_LIBS) $(gdkmm30_LIBS) 

INTLTOOL_FILES = intltool-extract.in \
                 intltool-merge.in \
                 intltool-update.in

#INTLTOOL_POFILES = Makefile.in.in ChangeLog cs.po POTFILES.in
#$(INTLTOOL_POFILES) 
# data poskytovana v distribuci
dist_data_DATA = $(include_HEADERS) $(impc_SOURCES) $(INTLTOOL_FILES) config xslt ui theme test 


DISTCLEANFILES = intltool-extract \
                 intltool-merge \
                 intltool-update \
                 po/.intltool-merge-cache