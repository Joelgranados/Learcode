
dnl Compiler flags preset
AC_DEFUN([AC_CXX_FLAGS_PRESET],[

dnl Declare variables which we want substituted in the Makefile.in's

dnl AC_SUBST(CXXFLAGS)
AC_SUBST(CXX_OPTIMIZE_FLAGS)
AC_SUBST(CXX_DEBUG_FLAGS)
AC_SUBST(CXX_PROFILE_FLAGS)
AC_SUBST(USE_SSE_FLAG)
AC_SUBST(CXX_LIBS)
AC_SUBST(AR)
AC_SUBST(AR_FLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(RANLIB)

dnl Set default values
AR="ar"
AR_FLAGS="-cru"
LDFLAGS=

AC_MSG_CHECKING([whether using $CXX preset flags])
AC_ARG_ENABLE(cxx-flags-preset,
AS_HELP_STRING([--enable-cxx-flags-preset],
[Enable C++ compiler flags preset @<:@default yes@:>@]),[],[enableval='yes'])

if test "$enableval" = yes ; then

	ac_cxx_flags_preset=yes

	case "$CXX" in
	g++) dnl GNU C++  http://gcc.gnu.org/
		CXX_VENDOR="GNU" 
		GCC_V=`$CXX --version`
		gcc_version=`expr "$GCC_V" : '.* \(@<:@0-9@:>@\)\..*'`
		gcc_release=`expr "$GCC_V" : '.* @<:@0-9@:>@\.\(@<:@0-9@:>@\).*'`

                COMMON_FLAGS=" -O3 -funroll-loops -fomit-frame-pointer -Wall -W -pipe -ftree-vectorize "

                echo $gcc_version $gcc_release
		if test $gcc_version -lt "3" ; then
			CXXFLAGS="-ftemplate-depth-40"
			CXX_OPTIMIZE_FLAGS="-O2 -funroll-loops -fstrict-aliasing -fno-gcse"
		else
			CXXFLAGS=""
			CXX_OPTIMIZE_FLAGS="$COMMON_FLAGS" 
		fi
		C_OPTIMIZE_FLAGS="$COMMON_FLAGS" 
                dnl -freassociative-math can be used instead of -ffast-math to enable vectorization of reductions of floats
                dnl -ffast-math -funsafe-math-optimizations -fforce-addr -fmove-all-movables -fkeep-inline-functions
		C_DEBUG_FLAGS="-g"
		CXX_DEBUG_FLAGS="-g"
		CXX_PROFILE_FLAGS="-pg "
		C_PROFILE_FLAGS="-pg "
                dnl USE_SSE_FLAG="-DUSE_SSE"

                dnl CpuArch=`check_cpu`
                dnl echo CpuArch=$CpuArch
                dnl if test ! -z $CpuArch; then
                dnl     C_DEBUG_FLAGS="$C_DEBUG_FLAGS -mtune=$CpuArch "
                dnl     CXX_DEBUG_FLAGS="$CXX_DEBUG_FLAGS -mtune=$CpuArch "
                dnl     C_OPTIMIZE_FLAGS="$C_OPTIMIZE_FLAGS  -mtune=$CpuArch "
                dnl     CXX_OPTIMIZE_FLAGS="$CXX_OPTIMIZE_FLAGS  -mtune=$CpuArch "
                dnl
	;;
	icpc|icc) dnl Intel icc http://www.intel.com/
dnl remark #983: operands are evaluated in unspecified order
dnl remark #1418: external definition with no prior declaration
dnl remark #383: value copied to temporary, reference to temporary used
dnl remark #444: destructor for base class "A<T>" is not virtual, if u move to Intel 8.1, remove this
dnl remark #810: conversion from "double" to "float" may lose significant bits
                CC="icc"
                CXX="icpc"
		CXX_VENDOR="Intel"
		CXXFLAGS="" dnl -strict_ansi

                WARNING_FLAGS=" -Wall -wd981 -wd383  -wd810 -wd444 "
		C_OPTIMIZE_FLAGS="-O3 -Zp16 -ip -ansi_alias -xN -axN -sox $WARNING_FLAGS -vec_report0 "
		C_DEBUG_FLAGS="-g -O0 -C $WARNING_FLAGS "

		CXX_OPTIMIZE_FLAGS="-restrict -O3 -Zp16 -ip -ansi_alias -xN -axN -sox $WARNING_FLAGS -cxxlib-icc -vec_report0 "
		CXX_DEBUG_FLAGS="-g -O0 -C $WARNING_FLAGS "
		CXX_PROFILE_FLAGS="-pg --fprofile-generate "
		C_PROFILE_FLAGS="-pg --fprofile-generate "

                CpuArch=`check_cpu`
                if test -n $CpuArch; then
                    C_DEBUG_FLAGS="$C_DEBUG_FLAGS -march=$CpuArch "
                    CXX_DEBUG_FLAGS="$CXX_DEBUG_FLAGS -march=$CpuArch "
                    C_OPTIMIZE_FLAGS="$C_OPTIMIZE_FLAGS  -march=$CpuArch "
                    CXX_OPTIMIZE_FLAGS="$CXX_OPTIMIZE_FLAGS  -march=$CpuArch "
                    USE_SSE_FLAG="-DUSE_SSE"
                fi
		SPECIAL_LDFALGS="-static-libcxa -i-static " 
	;;
	*) 
		ac_cxx_flags_preset=no
	;;
	esac
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
fi

if test "$ac_cxx_flags_preset" = yes ; then
	if test "$CXX_VENDOR" = Intel ; then
		AC_MSG_NOTICE([Setting compiler flags for $CXX_VENDOR $CXX (WAHOO!!)])
	else
		AC_MSG_NOTICE([Setting compiler flags for $CXX_VENDOR $CXX])
	fi
else

	AC_MSG_NOTICE([No flags preset found for $CXX])
fi

])


