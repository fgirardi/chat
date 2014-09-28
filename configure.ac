AC_INIT([chat], [0.01])
AC_CHECK_HEADER([ncurses.h], [], [echo "ncurses.h not found"; exit 1])
AC_CHECK_LIB([ncurses], [initscr], [:], [echo "pthread library not found" ; exit 1])
AC_CHECK_LIB([pthread], [pthread_create], [:], [ echo "pthread library not found" ; exit 1])
AM_INIT_AUTOMAKE([1.11 -Wall -Werror])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_PROG_CXX
AC_OUTPUT
