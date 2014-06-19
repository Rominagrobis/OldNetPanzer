AC_PREREQ([2.54])

AC_INIT([masterserver], 0.2.9)
AC_CONFIG_SRCDIR([src/MasterServer.hpp])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL

# check if our c++ compiler is gcc3.2 or newer
AC_MSG_CHECKING([whether g++ is new enough])
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([], [
#ifndef __GNUC__
#error no gcc compiler found (__GNUC__ not defined)
#endif
#if __GNUC__ < 3 || (__GNUC__ == 3 && __GNUC__ < 2)
#error gcc compiler too old. Need at least version 3.2.0
#endif
], [AC_MSG_RESULT([yes])], [AC_MSG_RESULT([no])
   AC_MSG_ERROR([
Error: Your gcc version is too old. You need at least gcc 3.2.0.
])])
AC_LANG_RESTORE

AC_CHECK_FUNCS([gethostbyname inet_ntoa memset socket strerror])
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h unistd.h])
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_VOLATILE
AC_TYPE_SIZE_T

AC_MSG_CHECKING(for debug mode)
AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug], [enable debugging mode]),
              enable_debug="$enableval", enable_debug="no")
if test "x${enable_debug}" != "xno"; then
    CXXFLAGS="$CXXFLAGS -Wall -W -Werror -DDEBUG -g -O0"
    CFLAGS="$CFLAGS -Wall -W -Werror -DDEBUG -g -O0"
    AC_MSG_RESULT([enabled])
else
    AC_MSG_RESULT([disabled])
fi

AC_CONFIG_FILES([Makefile docs/Makefile src/Makefile exampleclient/Makefile])
AC_OUTPUT

