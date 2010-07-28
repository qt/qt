AC_INIT([libconninet], patsubst(esyscmd([dpkg-parsechangelog | sed -n '/^Version: \(.*\)$/ {s//\1/;p}']), [
]), [jukka.rissanen@nokia.com])
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CXX
AC_PROG_LIBTOOL

AC_ARG_ENABLE(docs,     [ --enable-docs      Build DOXYGEN documentation (requires Doxygen)],enable_docs=$enableval,enable_docs=auto)

AC_PATH_PROG(DOXYGEN, doxygen, no)
AC_MSG_CHECKING([whether to build Doxygen documentation])

if test x$DOXYGEN = xno ; then
    have_doxygen=no
else
    have_doxygen=yes
fi
if test x$enable_docs = xauto ; then
    if test x$have_doxygen = xno ; then
        enable_docs=no
    else
        enable_docs=yes
    fi
fi
if test x$enable_docs = xyes; then
    if test x$have_doxygen = xno; then
        AC_MSG_ERROR([Building Doxygen docs explicitly required, but Doxygen not found])
    else 
	AC_MSG_RESULT(yes)
    fi
else
    AC_MSG_RESULT(no)
fi
 
AM_CONDITIONAL(DOXYGEN_DOCS_ENABLED, test x$enable_docs = xyes)
AC_SUBST(DOXYGEN)

PKG_CHECK_MODULES(GLIB, glib-2.0)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(QTCORE, QtCore)
AC_SUBST(QTCORE_CFLAGS)
AC_SUBST(QTCORE_LIBS)

PKG_CHECK_MODULES(QTNETWORK, QtNetwork)
AC_SUBST(QTNETWORK_CFLAGS)
AC_SUBST(QTNETWORK_LIBS)

PKG_CHECK_MODULES(QTDBUS, QtDBus)
AC_SUBST(QTDBUS_CFLAGS)
AC_SUBST(QTDBUS_LIBS)

PKG_CHECK_MODULES(QTTEST, QtTest)
AC_SUBST(QTTEST_CFLAGS)
AC_SUBST(QTTEST_LIBS)

PKG_CHECK_MODULES(DBUS, dbus-glib-1)
AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)

PKG_CHECK_MODULES(CONNSETTINGS, connsettings)
AC_SUBST(CONNSETTINGS_CFLAGS)
AC_SUBST(CONNSETTINGS_LIBS)

PKG_CHECK_MODULES(OSSO_IC, osso-ic)
AC_SUBST(OSSO_IC_CFLAGS)
AC_SUBST(OSSO_IC_LIBS)

PKG_CHECK_MODULES(ICD_DEV, icd2)
AC_SUBST(ICD_DEV_CFLAGS)
AC_SUBST(ICD_DEV_LIBS)

PKG_CHECK_MODULES(GCONF, gconf-2.0)
AC_SUBST(GCONF_CFLAGS)
AC_SUBST(GCONF_LIBS)

PKG_CHECK_MODULES(QGCONF, gq-gconf)
AC_SUBST(QGCONF_CFLAGS)
AC_SUBST(QGCONF_LIBS)

CONCFLAGS="-Wall -Werror -Wmissing-prototypes"
AC_SUBST(CONCFLAGS)

AC_CONFIG_FILES([Makefile \
	src/Makefile \
	tests/Makefile \
	conninet.pc \
	doxygen.cfg])
AC_OUTPUT
