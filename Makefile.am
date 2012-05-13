AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = po src/hashlib

CFLAGS=-Wall  -DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\""
AM_LDFLAGS= -lm 

include_HEADERS = src/guilinkwidget.h \
src/stext.h \
src/simage.h \
src/sfeedback.h \
src/ssource.h \
src/sarticle.h \
src/sslide.h \
src/sartist.h \
src/salbum.h \
src/bayesindex.h \
src/bayesclassificator.h \
src/guislideshowwidget.h \
src/guiarticleswidget.h \
src/thread.h \
src/agentmanager.h \
src/agent.h src/storage.h \
src/clientsettings.h \
src/guicoverwidget.h \
src/guiartistswidget.h \
src/guisettings.h \
src/guisources.h \
src/http.h \
src/utils.h \
src/webpage.h \
src/intelligentclient.h \
src/guimainwindow.h \
src/mpdsong.h \
src/mpdclient.h \
src/mpdlistener.h \
src/ClientException.h

bin_PROGRAMS = impc

#lib_LTLIBRARIES=libhl++.la
#libhl++_la_SOURCES= src/hashlib/hashutil.cpp
#libhl++_la_CFLAGS=-fPIC
#libhl++_la_CPPFLAGS=-fPIC
#libhl++_la_CXXFLAGS=-fPIC
#libhl++_la_LDFLAGS= -shared -fPIC

impc_SOURCES = src/guilinkwidget.cpp \
src/stext.cpp \
src/simage.cpp \
src/sfeedback.cpp \
src/ssource.cpp \
src/sarticle.cpp \
src/sslide.cpp \
src/sartist.cpp \
src/salbum.cpp \
src/bayesindex.cpp \
src/bayesclassificator.cpp \
src/guislideshowwidget.cpp \
src/guiarticleswidget.cpp \
src/thread.cpp \
src/agentmanager.cpp \
src/agent.cpp \
src/storage.cpp \
src/clientsettings.cpp \
src/guicoverwidget.cpp \
src/guiartistswidget.cpp \
src/guisettings.cpp \
src/guisources.cpp \
src/http.cpp \
src/utils.cpp \
src/webpage.cpp \
src/intelligentclient.cpp \
src/guimainwindow.cpp \
src/mpdsong.cpp \
src/mpdclient.cpp \
src/main.cpp
	
impc_CPPFLAGS = $(libxslt_CFLAGS) $(libxml_CFLAGS) $(jsoncpp_CFLAGS) $(gtkmm30_CFLAGS) $(gdkmm30_CFLAGS) $(imagemagick_CFLAGS) -DIMPC_LOCALEDIR=\"${IMPC_LOCALEDIR}\"
impc_LDADD = $(libmpdclient_LIBS)  $(libcurl_LIBS)  $(libxslt_LIBS) $(jsoncpp_LIBS) $(gtkmm30_LIBS) $(gdkmm30_LIBS) $(imagemagick_LIBS) src/hashlib/libhl++.a

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