AC_INIT([chat], [0.01], [], [], [http://github.com/marcosps/chat])

AC_CHECK_LIB([ncurses], [initscr], [:],
	[AC_MSG_ERROR(ncurses library not found. Install ncurses-devel package)])
AC_CHECK_LIB([pthread], [pthread_create], [:],
	[AC_MSG_ERROR(pthread library not found. Install glibc-devel package)])

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

AM_INIT_AUTOMAKE([1.11])
AC_CONFIG_FILES([Makefile src/Makefile])

AC_PROG_CXX([g++ clang++])
AC_OUTPUT
