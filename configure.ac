AC_INIT([aminf_test], [1.0], [bug-automake@gnu.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CHECK_LIB([rt], [sched_get_priority_min], [], AC_MSG_ERROR([librt not found]))
AC_PROG_CC
AC_CONFIG_FILES([
 Makefile
])
AC_OUTPUT


