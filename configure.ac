#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([impc], [0.1], [tomas.wagner@gmail.com])
AC_CONFIG_SRCDIR(src/main.cpp)

AM_INIT_AUTOMAKE([impc], [0.1])
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
#AC_CHECK_LIB([mpdclient], [mpd_connection_new], [],[
#         echo "mpdclient-dev library is required for this program"
#         exit -1])


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([dup2])

# Checks for intelligent client libraries
PKG_CHECK_MODULES([libmpdclient], [libmpdclient >= 2.3]) #2.3
PKG_CHECK_MODULES([libcurl], [libcurl >= 7.20]) #7.22
PKG_CHECK_MODULES([libxslt], [libxslt >= 1.0]) #1.126
PKG_CHECK_MODULES([libxml], [libxml-2.0 >= 2.0]) #2.7.8
#PKG_CHECK_MODULES([gtkmm20], [gtkmm-2.4 >= 2.24])
PKG_CHECK_MODULES([gtkmm30], [gtkmm-3.0 >= 3.0]) #3.3.18
PKG_CHECK_MODULES([gdkmm30], [gdkmm-3.0 >= 3.0]) #3.3.18
PKG_CHECK_MODULES([jsoncpp], [jsoncpp >= 0.5])
PKG_CHECK_MODULES([imagemagick], [Magick++])

# Localization
IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=impc
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], 
                          [The domain to use with gettext])

# List of availabel language here
ALL_LINGUAS="cs"

AM_GLIB_GNU_GETTEXT

IMPC_LOCALEDIR=[${datadir}/locale]
AC_SUBST(IMPC_LOCALEDIR)
# Localization end

#DOXYGEN

#LIBTOOL
LT_INIT
LT_LANG([C++])
AC_ENABLE_SHARED
AC_DISABLE_STATIC


AC_CONFIG_FILES([Makefile po/Makefile.in])
AC_OUTPUT
