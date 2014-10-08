AC_INIT([chat], [0.01])
AC_CHECK_HEADER([ncurses.h], [], [echo "ncurses.h not found"; exit 1])
AC_CHECK_LIB([ncurses], [initscr], [:], [echo "pthread library not found" ; exit 1])
AC_CHECK_LIB([pthread], [pthread_create], [:], [ echo "pthread library not found" ; exit 1])

AC_ARG_ENABLE([verbose],
	AC_HELP_STRING([--enable-verbose],
		[Enables verbose output from server and client.]))


AC_MSG_CHECKING([build with verbose enabled])

if test "x$enable_verbose" == "xyes"; then
	AC_MSG_RESULT([yes])
	AC_DEFINE(ENABLE_VERBOSE, "TRUE")
else
	AC_MSG_RESULT([no])
fi

AM_INIT_AUTOMAKE([1.11 -Wall -Werror])
AC_CONFIG_FILES([Makefile src/Makefile])

AC_PROG_CXX
AC_OUTPUT
