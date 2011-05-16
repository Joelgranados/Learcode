# Configure path for the GNU Scientific Library
# Christopher R. Gabriel <cgabriel@linux.it>, April 2000


AC_DEFUN([AM_PATH_IMLIB2],
[
AC_ARG_WITH(imlib2-prefix,[  --with-imlib2-prefix=PFX   Prefix where ImLib2 is installed (optional)],
            imlib2_prefix="$withval", imlib2_prefix="")
AC_ARG_WITH(imlib2-exec-prefix,[  --with-imlib2-exec-prefix=PFX Exec prefix where ImLib2 is installed (optional)],
            imlib2_exec_prefix="$withval", imlib2_exec_prefix="")
AC_ARG_ENABLE(imlib2test, [  --disable-imlib2test       Do not try to compile and run a test ImLib2 program],
		    , enable_imlib2test=yes)

  if test "x${IMLIB2_CONFIG+set}" != xset ; then
     if test "x$imlib2_prefix" != x ; then
         IMLIB2_CONFIG="$imlib2_prefix/bin/imlib2-config"
     fi
     if test "x$imlib2_exec_prefix" != x ; then
        IMLIB2_CONFIG="$imlib2_exec_prefix/bin/imlib2-config"
     fi
  fi

  AC_PATH_PROG(IMLIB2_CONFIG, imlib2-config, no)
  min_imlib2_version=ifelse([$1], ,1.2,$1)
  AC_MSG_CHECKING(for ImLib2 - version >= $min_imlib2_version)
  no_imlib2=""
  if test "$IMLIB2_CONFIG" = "no" ; then
    no_imlib2=yes
  else
    IMLIB2_CFLAGS=`$IMLIB2_CONFIG --cflags`
    IMLIB2_LIBS=`$IMLIB2_CONFIG --libs`

    imlib2_major_version=`$IMLIB2_CONFIG --version | \
           sed 's/^\([[0-9]]*\).*/\1/'`
    if test "x${imlib2_major_version}" = "x" ; then
       imlib2_major_version=0
    fi

    imlib2_minor_version=`$IMLIB2_CONFIG --version | \
           sed 's/^\([[0-9]]*\)\.\{0,1\}\([[0-9]]*\).*/\2/'`
    if test "x${imlib2_minor_version}" = "x" ; then
       imlib2_minor_version=0
    fi

    imlib2_micro_version=`$IMLIB2_CONFIG --version | \
           sed 's/^\([[0-9]]*\)\.\{0,1\}\([[0-9]]*\)\.\{0,1\}\([[0-9]]*\).*/\3/'`
    if test "x${imlib2_micro_version}" = "x" ; then
       imlib2_micro_version=0
    fi

    if test "x$enable_imlib2test" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $IMLIB2_CFLAGS"
      LIBS="$LIBS $IMLIB2_LIBS"

      rm -f conf.imlib2test
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* my_strdup (const char *str);

char*
my_strdup (const char *str)
{
  char *new_str;
  
  if (str)
    {
      new_str = (char *)malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
  
  return new_str;
}

int main (void)
{
/*
  int major = 0, minor = 0, micro = 0;
  int n;
  char *tmp_version;

  system ("touch conf.imlib2test");
*/
  /* HP/UX 9 (%@#!) writes to sscanf strings */
/*
  tmp_version = my_strdup("$min_imlib2_version");

  n = sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) ;

  if (n != 2 && n != 3) {
     printf("%s, bad version string\n", "$min_imlib2_version");
     exit(1);
   }

   if (($imlib2_major_version > major) ||
      (($imlib2_major_version == major) && ($imlib2_minor_version > minor)) ||
      (($imlib2_major_version == major) && ($imlib2_minor_version == minor) && ($imlib2_micro_version >= micro)))
    {
      exit(0);
    }
  else
    {
      printf("\n*** 'imlib2-config --version' returned %d.%d.%d, but the minimum version\n", $imlib2_major_version, $imlib2_minor_version, $imlib2_micro_version);
      printf("*** of ImLib2 required is %d.%d.%d. If imlib2-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If imlib2-config was wrong, set the environment variable IMLIB2_CONFIG\n");
      printf("*** to point to the correct copy of imlib2-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      exit(1);
    }
*/
}

],, no_imlib2=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_imlib2" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$IMLIB2_CONFIG" = "no" ; then
       echo "*** The imlib2-config script installed by ImLib2 could not be found"
       echo "*** If ImLib2 was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the IMLIB2_CONFIG environment variable to the"
       echo "*** full path to imlib2-config."
       exit 1
     else
       if test -f conf.imlib2test ; then
        :
       else
          echo "*** Could not run ImLib2 test program, checking why..."
          CFLAGS="$CFLAGS $IMLIB2_CFLAGS"
          LIBS="$LIBS $IMLIB2_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding ImLib2 or finding the wrong"
          echo "*** version of ImLib2. If it is not finding ImLib2, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means ImLib2 was incorrectly installed"
          echo "*** or that you have moved ImLib2 since it was installed. In the latter case, you"
          echo "*** may want to edit the imlib2-config script: $IMLIB2_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
#     IMLIB2_CFLAGS=""
#     IMLIB2_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(IMLIB2_CFLAGS)
  AC_SUBST(IMLIB2_LIBS)
  rm -f conf.imlib2test
])


