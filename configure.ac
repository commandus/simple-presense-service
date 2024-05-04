#
# Example:
# ./configure
#
AC_PREREQ([2.63])
AC_INIT([simple-presense-service], [1.0], [andrei.i.ivanov@gmail.com])
AM_INIT_AUTOMAKE([1.11 foreign no-define subdir-objects])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_SRCDIR([simple-presense-service.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL

LT_INIT

# determine target OS
build_linux=no
build_windows=no
build_mac=no
case "${host_os}" in
    linux*)
        build_linux=yes
        ;;
    cygwin*|mingw*)
        build_windows=yes
        ;;
    darwin*)
        build_mac=yes
        ;;
    *)
        AC_MSG_ERROR(["OS $host_os is not supported"])
        ;;
esac
# Pass the conditionals to automake
AM_CONDITIONAL([OS_LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([OS_WINDOWS], [test "$build_windows" = "yes"])
AM_CONDITIONAL([OS_MAC], [test "$build_mac" = "yes"])

build_mac=no
# Detect the target system
case "${host_os}" in
  darwin*)
    build_mac=yes
    ;;
esac
AM_CONDITIONAL([OS_MACH], [test "$build_mac" = "yes"])

# MacOS: install openssl into /usr/local/lib/opt/openssl@1.1
# brew install openssl@1.1
LDFLAGS="$LDFLAGS -L/usr/local/lib"

# Checks for header files. Add /usr/local/include for OS X.
CFLAGS="$CFLAGS -I/usr/local/include"

AC_OUTPUT
