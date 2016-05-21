#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT(snap, 2.4, [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([snap-core/alg.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE(-Wall subdir-objects foreign color-tests dist-bzip2 parallel-tests)

#magic for conditional check in Makefile:
MK=''; AC_SUBST(MK)
SED=sed

# Checks for programs.
AM_PROG_AR
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for libraries.
LT_PREREQ([2.2])
#LT_INIT([shared static])
LT_INIT([disable-static])


# FIXME: Replace `main' with a function in `-lgtest':
AC_CHECK_LIB([gtest], [main])

# debug
AC_ARG_ENABLE([debug],
	AS_HELP_STRING([--enable-debug],[Compile the debug version (default: disabled)]),
	[enable_debug=$enableval],
	[enable_debug=no])
AM_CONDITIONAL([DEBUG], [test $enable_debug = "yes"])
if test "x$enable_debug" = "xyes"; then
  changequote({,})
  CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9s]*//g'`
  CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-O[0-9s]*//g'`
  changequote([,])
  dnl add -O0 only if GCC or ICC is used
  if test "$GCC" = "yes" || test "$ICC" = "yes"; then
    CFLAGS="$CFLAGS -O0"
    CXXFLAGS="$CXXFLAGS -O0"
  fi
else
  changequote({,})
  CFLAGS=`echo "$CFLAGS" | $SED -e 's/-g//g'`
  CXXFLAGS=`echo "$CXXFLAGS" | $SED -e 's/-g//g'`
  changequote([,])
fi

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h float.h inttypes.h limits.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/file.h sys/ioctl.h sys/mount.h sys/param.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_FUNC_STRTOD
AC_CHECK_FUNCS([clock_gettime dup2 fdatasync floor ftruncate getcwd gethostbyaddr gethostbyname gettimeofday localtime_r memchr memmove memset mkdir munmap pow putenv socket sqrt strchr strdup strerror strrchr strstr strtol])

AC_CONFIG_FILES([Makefile
                 libsnap.pc \
                 examples/Makefile
                 glib-core/Makefile
                 glib-adv/Makefile
                 snap-core/Makefile
                 snap-adv/Makefile
                 snap-exp/Makefile
                ])
AC_OUTPUT
