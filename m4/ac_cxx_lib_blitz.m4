dnl Code changed from Patrick version 
dnl @synopsis AC_CXX_LIB_BLITZ([optional-string "required"])
dnl
dnl Check whether Blitz++ is installed.
dnl Blitz++ is available at http://sourceforge.net/projects/blitz
dnl
dnl   Set the path for Blitz++  with the option
dnl      --with-blitz[=DIR]
dnl   Blitz headers should be under DIR/includes
dnl   Blitz library should be under DIR/lib
dnl   Then try to compile and run a simple program with a Blitz Array
dnl   Optional argument `required' triggers an error if Blitz++ not installed
dnl 
dnl @version $Id: ac_cxx_lib_blitz.m4,v 1.1 2006/06/16 17:39:02 ndalal Exp $
dnl @author Patrick Guio <patrick.guio@matnat.uio.no>
dnl
AC_DEFUN([AC_MSG_ERROR_BLITZ],[
AC_MSG_ERROR([
Package requires the Blitz++ template library (available at http://sourceforge.net/projects/blitz)
  After installing, provide path to installation directory with option
    --with-blitz@<:@=DIR@:>@
])])


AC_DEFUN([AC_CXX_LIB_BLITZ],[
    AC_SUBST(BLITZ_CPPFLAGS)
    AC_SUBST(BLITZ_LDFLAGS)
    AC_SUBST(BLITZ_LIBS)

    AC_ARG_WITH(blitz,
        AC_HELP_STRING([--with-blitz@<:@=DIR@:>@],[absolute path name to blitz++ libraries. Alternatively, will use BLITZ_ROOT environment variable]),
        [   if test "$withval" != yes ; then
                BLITZ_ROOT="$withval"
            fi ],
    )

    if test "x$BLITZ_ROOT" = x ; then
        BLITZ_ROOT="/usr"
    fi

    BLITZ_CPPFLAGS="-I$BLITZ_ROOT/include"
    BLITZ_LDFLAGS="-L$BLITZ_ROOT/lib"
    BLITZ_LIBS="-lblitz"

    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

    OLD_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$BLITZ_CPPFLAGS"
    OLD_LIBS="$LIBS"
    LIBS="-lblitz"
    OLD_LDFLAGS="$LDFLAGS"
    LDFLAGS="$BLITZ_LDFLAGS"

    saveLDFLAGS=$LDFLAGS

    AC_CACHE_CHECK([whether Blitz++ is installed],ac_cxx_lib_blitz,
        [AC_LANG_SAVE
        AC_LANG_CPLUSPLUS
	AC_RUN_IFELSE(
	[AC_LANG_PROGRAM([[
#include <blitz/array.h>
]],[[
blitz::Array<int,1> x(10);
x = blitz::tensor::i;
	]])],
        [ac_cxx_lib_blitz=yes],
        [ac_cxx_lib_blitz=no])
	AC_LANG_RESTORE
    ])

    CPPFLAGS="$OLD_CPPFLAGS"
    LDFLAGS="$OLD_LDFLAGS"
    LIBS=$"$OLD_LIBS"

    if test "$ac_cxx_lib_blitz" = no ; then
        BLITZ_LDFLAGS=""
        BLITZ_CPPFLAGS=""
        BLITZ_LIBS=""
        if test "$1" = required ; then
                AC_MSG_ERROR_BLITZ
        fi
    fi

    echo "    BLITZ_CPPFLAGS  = $BLITZ_CPPFLAGS"
    echo "    BLITZ_LDFLAGS   = $BLITZ_LDFLAGS"
    echo "    BLITZ_LIBS      = $BLITZ_LIBS"
])




