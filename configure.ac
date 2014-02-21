#                               -*- Autoconf -*-
# TWaveform configure.ac
#
# Initial version written by M. Marino Aug 2011.
# Rewritten by V. Belov Sep 2011.
# Generates the configure script in use during the configuration phase of the
# build for TWaveform.  Should be built with autoconf 2.63 

#########################################################################
# Init stage
#########################################################################
AC_PREREQ(2.63)
AC_INIT([TWaveform], [1.0])
AC_CONFIG_SRCDIR([WaveBase/TTemplWaveform.hh])

# works for me, even in screen on Darwin
AS_IF([test x"$PS1" != x],[BOLD=`tput smso`;OFFBOLD=`tput rmso`;])dnl make life brighter

#########################################################################
# Check base system
#########################################################################
AC_CANONICAL_HOST
AC_PROG_AWK
AC_PROG_SED
AC_PROG_LN_S
AC_PROG_MKDIR_P

#########################################################################
# Check the compiler
#########################################################################
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CXXCPP
AC_LANG(C++)
AC_PROG_LIBTOOL
AC_SUBST(LD)
AS_IF([test x"${build_cpu}" = "xx86_64"],[
  case "${host_cpu}" in
    i?86)
      CFLAGS="$CFLAGS -m32"
      CXXFLAGS="$CXXFLAGS -m32"
      LDFLAGS="$LDFLAGS -m32"
      AC_MSG_NOTICE([Cross compiling on an x86_64 for i386... using -m32 flag])
      # Strictly speaking one needs to check that compiler used is not a 
      #   cross-compiler, but it's so rare in _this_ case
  esac])

#########################################################################
#Find python, this is necessary for some of the build 
#########################################################################
m4_include(python.m4)
AM_PATH_PYTHON([2.5],
               [HAVE_PYTHON=yes
                PYTHONLIBDIR=`$PYTHON -c 'from distutils.sysconfig import get_python_lib; print "%s/config" % get_python_lib(0,1)'`],
               [HAVE_PYTHON=no
                AC_MSG_ERROR([python not found, required for build])])
AC_SUBST(HAVE_PYTHON)
AC_SUBST(PYTHONLIBDIR)


#########################################################################
# Check extra dependencies
#########################################################################
#########################################################################
# Looking for ROOT
#########################################################################
AC_MSG_NOTICE([${BOLD}Checking for external dependencies: ${OFFBOLD}])
m4_include(root.m4)
ROOT_PATH([5.28], 
          [HAVE_ROOT=yes
           AC_DEFINE(HAVE_ROOT,1,[Define to 1 if you have ROOT installed.])], 
          [HAVE_ROOT=no
           AC_MSG_ERROR([ROOT not found, please install])])
AC_SUBST(HAVE_ROOT)
AC_PATH_PROG(ROOTLIBMAP, rlibmap , no, `$ROOT_CONFIG --bindir`)

AS_IF([test x"$HAVE_ROOT" = xyes],
   ROOT_FEATURE([fftw3],
   [ HAVE_ROOT_FFTW3=yes
     AC_DEFINE(HAVE_ROOT_FFTW3,1,[Define to 1 if ROOT is build with fftw3.]) 
   ])
  )
AC_SUBST(ROOT_INCLUDE,"-I$ROOTINCDIR")
AC_SUBST(ROOT_LIBDIR,"$ROOTLIBDIR")
AC_MSG_NOTICE([${BOLD}ROOT found and properly configured${OFFBOLD}])

#########################################################################
# Looking for FFTW3
#########################################################################
# We use fftw3 directly, check for it.
AC_ARG_WITH(fftw,
            [AC_HELP_STRING([--without-fftw],[disable FFTW3 support])],
            [AS_IF([test x"$withval" != xno],[with_fftw=yes],[with_fftw=no])], [with_fftw=yes])
AC_ARG_WITH([fftw-prefix],
            [AC_HELP_STRING([--with-fftw-prefix],[prefix where FFTW3 is installed (optional)])],
    	    [fftw_prefix="$withval"],[fftw_prefix=""])

FFTW_INCLUDE=""
FFTW_LIBS=""
FFTW_LDFLAGS=""
AS_IF([test x"$with_fftw" = xyes], 
 [
    # First check to see if symbols are in the ROOT libraries, this can happen
    AS_IF([test x"$HAVE_ROOT_FFTW3" = xyes],
           [
             save_LIBS="$LIBS"
             save_LDFLAGS="$LDFLAGS"
             LDFLAGS="$LDFLAGS -L$ROOTLIBDIR"
             AC_MSG_NOTICE([Checking if ROOT FFTW library contains the correct symbols])
             AC_SEARCH_LIBS([fftw_destroy_plan],[FFTW], 
               AC_SEARCH_LIBS([fftw_plan_dft_r2c],[FFTW],
                 AC_SEARCH_LIBS([fftw_plan_dft_c2r],[FFTW],
                   AC_SEARCH_LIBS([fftw_execute],[FFTW], 
                     [HAVE_FFTW=yes
                      USE_ROOT_FFTW=yes
                      FFTW_LIBS="FFTW"
                      LIBS="$LIBS -lFFTW"
                      AC_MSG_NOTICE([Using ROOTs FFTW library])
                      ], [], [-lCore -lCint -ldl])
                   , [], [-lCore -lCint -ldl])
                 , [], [-lCore -lCint -ldl])
               , [], [-lCore -lCint -ldl])
             AS_IF([test x"$USE_ROOT_FFTW" != xyes],
               [
                 LIBS="$save_LIBS"
                 LDFLAGS="$save_LDFLAGS"
               ]
             )
           ] 
         ) 
    
    AS_IF([test x"$USE_ROOT_FFTW" = x],
      [
      AC_MSG_NOTICE([Cannot use ROOT FFTW library, trying others.])
      save_CPPFLAGS="$CPPFLAGS"
      save_LIBS="$LIBS"
      save_LDFLAGS="$LDFLAGS"
      FFTW_LIBS="fftw3"
      AS_IF([test x"$fftw_prefix" != x],
            [FFTW_INCLUDE="-I$fftw_prefix/include"
             FFTW_LDFLAGS="-L$fftw_prefix/lib"])
     
      CPPFLAGS="$CPPFLAGS $FFTW_INCLUDE" 
      LIBS="$LIBS -l$FFTW_LIBS" 
      LDFLAGS="$LDFLAGS $FFTW_LDFLAGS" 
      AC_CHECK_HEADER([fftw3.h],[HAVE_FFTW=yes],[HAVE_FFTW=no])
      AS_IF([test x"$HAVE_FFTW" = xyes],
        [ 
        AC_MSG_CHECKING([if a program with fftw runs])
        AC_LINK_IFELSE(
      	[AC_LANG_PROGRAM(
            [#include "fftw3.h"],
            [fftw_complex temp;])],
      	[AC_MSG_RESULT(yes)],
      	[AC_MSG_RESULT(no)
      	 HAVE_FFTW=no])
        CPPFLAGS=$save_CPPFLAGS
        LDFLAGS=$save_LDFLAGS
       ])
    ])
 ], [HAVE_FFTW=no])
AS_IF([test x"$HAVE_FFTW" != xno],
  [AC_DEFINE(HAVE_FFTW,1,[Define to 1 if you have FFTW3 installed.])])
AS_IF([test x"$USE_ROOT_FFTW" = xyes],
  [AC_DEFINE(USE_ROOT_FFTW,1,[Define to 1 if you will use the ROOT FFTW installation.])])
AC_SUBST(FFTW_INCLUDE)
AC_SUBST(FFTW_LIBS)
AC_SUBST(FFTW_LDFLAGS)

#########################################################################
# Make build flags
#########################################################################
# handle dynamic libraries
case "$host_os" in
  darwin*)
#    EXTRALIBFLAGS="-bind_at_load" 
#    SOFLAGS="-dynamiclib"
    SHREXT=dylib
    SOFLAGS="-dynamiclib -install_name @rpath/"
    ORIGINFLAGS="@loader_path/../lib"
    LIBRARY_SEARCH_PATH=DYLD_LIBRARY_PATH
  ;;
  linux*)
    SOFLAGS="-shared -Wl,-soname,"
    SHREXT=so
    LDFLAGS="$LDFLAGS -Wl,-rpath-link,\$(prefix)/lib -Wl,-z,defs" 
    ORIGINFLAGS=\'\$\$ORIGIN/../lib\',--enable-new-dtags
    LIBRARY_SEARCH_PATH=LD_LIBRARY_PATH
  ;;
  *)
  AC_MSG_ERROR([Don't know how to build dynamic libraries on your system])
  ;;
esac
AC_SUBST(SOFLAGS)
AC_SUBST(SHREXT)
AC_SUBST(ORIGINFLAGS)
AC_SUBST(LIBRARY_SEARCH_PATH)

# check x86_64
case "${host_cpu}" in
    x86_64*)
        CXXFLAGS="${CXXFLAGS} -fPIC"
        AC_MSG_NOTICE([Compiling on an x86_64 chipset... setting -fPIC flag.])
  # only on amd64 should one compile static libs with this flag
  # Other tested architectures currently compile PIC libraries
  # and so it is no problem to link a dynamic library using the PIC
  # libs
esac

# Setting default warning flags.
WARNINGFLAGS="-W -Wall -Wextra"
case "${CXX}" in
  icc*)
  # If using icc, remove warning flags.
  # icc output is otherwise too verbose.
  WARNINGFLAGS=""
esac

AC_SUBST(WARNINGFLAGS)

AS_IF([test x"$as_echo" = x],
 [AC_SUBST(AX_ECHO,[echo])
  AC_SUBST(AX_ECHO_N,[echo $ECHO_N " $ECHO_C"])],
 [AC_SUBST(AX_ECHO,$as_echo)
  AC_SUBST(AX_ECHO_N,$as_echo_n)])
#---------------------------------------
# Get directories to compile
#_AS_ECHO_N([looking for all compilable directories... ])
#DIRECTORIES=`find $EXOANALYSISROOT -type d -exec sh -c 'if test -d {}/src; then printf "{} "; fi' \;`
#DIRECTORIES=`echo $DIRECTORIES | $SED "s|$EXOANALYSISROOT/||g"`
#AC_MSG_RESULT(done)
#AC_SUBST(DIRECTORIES)

#EXTRACXXFLAGS="${EXTRACXXFLAGS} -O2 -ggdb"
#AC_SUBST(EXTRACXXFLAGS)

dnl AC_SUBST(CPPFLAGS)
dnl LDFLAGS=`$ROOTCONF --ldflags --glibs` 
dnl AC_SUBST(LDFLAGS)

#########################################################################
# Make output
#########################################################################

# deliver all DEFINES via command line as well
AC_OUTPUT_MAKE_DEFS
AX_DEFS=$DEFS
AC_SUBST(AX_DEFS)
# rootcint has issues with some of the defines, in particular those that define
# the arguments of the select function.  Therefore, we remove those defines and
# add the rest to ROOTCINTDEFS
ROOTCINTDEFS=`echo $AX_DEFS | $SED "s/-DSELECT_TYPE_ARG[[1-5]]*=[[^-D]]*//g"`
AC_SUBST(ROOTCINTDEFS)
#
AC_CONFIG_FILES([
                 buildTools/config.mk
                ])
AC_OUTPUT

AC_MSG_NOTICE
AC_MSG_NOTICE([**********************************************************************************])
AC_MSG_NOTICE([Run])
AC_MSG_NOTICE
AC_MSG_NOTICE([  ./configure --help])
AC_MSG_NOTICE
AC_MSG_NOTICE([for a list of options and configuration flags to adjust the configuration.])
AC_MSG_NOTICE([**********************************************************************************])

AC_MSG_NOTICE([You have successfully configured TWaveform.])
AC_MSG_NOTICE([Please now compile by issuing the command (where the @%:@ in -j@%:@ refers to the CPUs ]) 
AC_MSG_NOTICE([across which you'd like to split the build process @<:@optional@:>@):])
AC_MSG_NOTICE
AC_MSG_NOTICE([  make clean ])
AC_MSG_NOTICE([  make @<:@-j@%:@@:>@ ])
AC_MSG_NOTICE


