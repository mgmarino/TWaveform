dnl -*- mode: autoconf -*- 
dnl
dnl $Id: root.m4,v 1.3 2005/03/21 21:42:21 rdm Exp $
dnl $Author: rdm $
dnl $Date: 2005/03/21 21:42:21 $
dnl
dnl Autoconf macro to check for existence or ROOT on the system
dnl Synopsis:
dnl
dnl  ROOT_PATH([MINIMUM-VERSION, [ACTION-IF-FOUND, [ACTION-IF-NOT-FOUND]]])
dnl
dnl Some examples: 
dnl 
dnl    ROOT_PATH(3.03/05, , AC_MSG_ERROR(Your ROOT version is too old))
dnl    ROOT_PATH(, AC_DEFINE([HAVE_ROOT]))
dnl 
dnl The macro defines the following substitution variables
dnl
dnl    ROOTCONF           full path to root-config
dnl    ROOTEXEC           full path to root
dnl    ROOTCINT           full path to rootcint
dnl    ROOTLIBDIR         Where the ROOT libraries are 
dnl    ROOTINCDIR         Where the ROOT headers are 
dnl    ROOTETCDIR         Where the ROOT configuration is
dnl    ROOTCFLAGS         Extra compiler flags
dnl    ROOTLIBS           ROOT basic libraries 
dnl    ROOTGLIBS          ROOT basic + GUI libraries
dnl    ROOTAUXLIBS        Auxilary libraries and linker flags for ROOT
dnl    ROOTAUXCFLAGS      Auxilary compiler flags 
dnl    ROOTRPATH          Same as ROOTLIBDIR
dnl
dnl The macro will fail if root-config and rootcint isn't found.
dnl
dnl Christian Holm Christensen <cholm@nbi.dk>
dnl
AC_DEFUN([ROOT_PATH],
[
  AC_REQUIRE([AC_PROG_AWK])dnl
  AC_ARG_WITH([root-prefix],
              [AC_HELP_STRING([--with-root-prefix],[prefix where ROOT is installed (optional)])],
    	      [root_prefix="$withval"],[root_prefix=""])

  # be old-compartible
  if test x"$root_prefix" = x ; then
    root_prefix="$ROOTSYS"
  fi
  if test "x${ROOT_CONFIG+set}" != xset ; then
    if test x"$root_prefix" != x ; then
      ROOT_CONFIG="$root_prefix/bin/root-config"
    fi
  fi
  AC_PATH_PROG(ROOT_CONFIG, root-config , no)
  min_root_version=ifelse([$1], ,3.0.0,$1)
  if test x"$ROOT_CONFIG" = xno ; then
    no_root=yes
  else
    AC_MSG_CHECKING(whether ROOT version >= [$1])
    vers=`$ROOT_CONFIG --version | tr './' ' ' | $ac_cv_prog_AWK 'BEGIN { FS = " "; } { printf "%d", ($''1 * 1000 + $''2) * 1000 + $''3;}'`
    requ=`echo $1 | tr './' ' ' | $ac_cv_prog_AWK 'BEGIN { FS = " "; } { printf "%d", ($''1 * 1000 + $''2) * 1000 + $''3;}'`
    if test 0"$vers" -lt 0"$requ" ; then 
        AC_MSG_RESULT(no)
	no_root=yes
    else 
        AC_MSG_RESULT(yes)
    fi
    
  fi
    
  if test x"$no_root" != xyes ; then
    # be compartible
    ROOTCONF="$ROOT_CONFIG"
    AC_PATH_PROG(ROOTEXEC, root , no, `$ROOT_CONFIG --bindir`)
    AC_PATH_PROG(ROOTCINT, rootcint , no, `$ROOT_CONFIG --bindir`)
  fi
  if test ! x"$ROOTCONF" = "xno" && \
     test ! x"$ROOTCINT" = "xno" ; then 

    # define some variables 
    ROOTLIBDIR=`$ROOTCONF --libdir`
    ROOTINCDIR=`$ROOTCONF --incdir`
    ROOTETCDIR=`$ROOTCONF --etcdir`
    ROOTCFLAGS=`$ROOTCONF --noauxcflags --cflags` 
    ROOTLIBS=`$ROOTCONF --noauxlibs --noldflags --libs`
    ROOTGLIBS=`$ROOTCONF --noauxlibs --noldflags --glibs`
    ROOTAUXCFLAGS=`$ROOTCONF --auxcflags`
    ROOTAUXLIBS=`$ROOTCONF --auxlibs`
    ROOTRPATH=$ROOTLIBDIR
    ROOTVERSION=`$ROOTCONF --version`
    ROOTSOVERSION=`dirname $ROOTVERSION`
	
  else
    # otherwise, we say no_root
    no_root=yes
  fi

  AC_SUBST(ROOTLIBDIR)
  AC_SUBST(ROOTINCDIR)
  AC_SUBST(ROOTETCDIR)
  AC_SUBST(ROOTCFLAGS)
  AC_SUBST(ROOTLIBS)
  AC_SUBST(ROOTGLIBS) 
  AC_SUBST(ROOTAUXLIBS)
  AC_SUBST(ROOTAUXCFLAGS)
  AC_SUBST(ROOTRPATH)
  AC_SUBST(ROOTVERSION)
  AC_SUBST(ROOTSOVERSION)

  if test "x$no_root" = x ; then 
    ifelse([$2], , :, [$2])     
  else 
    ifelse([$3], , :, [$3])     
  fi
])

#
# Macro to check if ROOT has a specific feature:
#
#   ROOT_FEATURE(FEATURE,[ACTION_IF_HAVE,[ACTION_IF_NOT]])
#
# For example 
#
#   ROOT_FEATURE([ldap],[AC_DEFINE([HAVE_ROOT_LDAP])])
# 
AC_DEFUN([ROOT_FEATURE],
[
  AC_REQUIRE([ROOT_PATH])
  feat=$1
  AC_MSG_CHECKING([whether ROOT has $feat])
  res=`$ROOTCONF --has-$feat` 
  if test "x$res" = "xyes" ; then 
    AC_MSG_RESULT([yes])
    ifelse([$2], , :, [$2])     
  else 
    AC_MSG_RESULT([no])
    ifelse([$3], , :, [$3])     
  fi
])

#
# EOF
#
