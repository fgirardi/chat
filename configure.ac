AC_INIT([chat], [0.01])
AC_CHECK_HEADER([ncurses.h], [], [AC_MSG_RESULT([ncurses.h not found])], [])
AC_CHECK_LIB([ncurses], [initscr], [:], [AC_MSG_RESULT([Library ncurses not found])], [])
AM_INIT_AUTOMAKE([1.11 -Wall -Werror])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_PROG_CXX
AC_OUTPUT
