dnl #------------------------------------------------------------------------#
dnl # Test if all needed libraries for vboxgetty are installed. We will end  #
dnl # the configure script if one is missing!                                #
dnl #------------------------------------------------------------------------#

AC_DEFUN(GND_PACKAGE_TCL,
   [
      HAVE_TCL_LIBS="n"
      LINK_TCL_LIBS=""

      gnd_use_tcl_lib=""

      AC_ARG_WITH(tcllib,
         [  --with-tcllib=LIB       use tcl library LIB to link [tcl]],
         gnd_use_tcl_lib="${withval}"
      )

      if (test "${gnd_use_tcl_lib}" = "")
      then
         gnd_1st_tcl_lib_test="tcl8.0"
         gnd_2nd_tcl_lib_test="tcl7.6"
         gnd_3rd_tcl_lib_test="tcl"
      else
         gnd_1st_tcl_lib_test="${gnd_use_tcl_lib}"
         gnd_2nd_tcl_lib_test="tcl8.0"
         gnd_3rd_tcl_lib_test="tcl7.6"
      fi

      AC_CHECK_LIB(m,
         cos,
         AC_CHECK_LIB(dl,
            dlerror,
            AC_CHECK_LIB(${gnd_1st_tcl_lib_test},
               Tcl_CreateInterp,
               LINK_TCL_LIBS="-l${gnd_1st_tcl_lib_test} -lm -ldl",
               AC_CHECK_LIB(${gnd_2nd_tcl_lib_test},
                  Tcl_CreateInterp,
                  LINK_TCL_LIBS="-l${gnd_2nd_tcl_lib_test} -lm -ldl",
                  AC_CHECK_LIB(${gnd_3rd_tcl_lib_test},
                     Tcl_CreateInterp,
                     LINK_TCL_LIBS="-l${gnd_3rd_tcl_lib_test} -lm -ldl",
                     ,
                     -lm -ldl
                  ),
                  -lm -ldl
               ),
               -lm -ldl
            ),
         ),
      )

      AC_CHECK_HEADER(tcl.h, , LINK_TCL_LIBS="")

      if (test "${LINK_TCL_LIBS}" = "")
      then
         AC_MSG_WARN(tcl package or needed components not found! Some of the)
         AC_MSG_WARN(utilities (eg vboxgetty) will not be build...)

         HAVE_TCL_LIBS="n"
      else
         HAVE_TCL_LIBS="y"
      fi

      AC_SUBST(LINK_TCL_LIBS)
      AC_SUBST(HAVE_TCL_LIBS)
   ]
)

dnl #------------------------------------------------------------------------#
dnl # Checks if the ncurses package is installed:                            #
dnl #------------------------------------------------------------------------#

AC_DEFUN(GND_PACKAGE_NCURSES,
   [
      HAVE_NCURSES_LIBS="n"
      LINK_NCURSES_LIBS=""

      AC_CHECK_HEADER(ncurses.h,
         AC_CHECK_HEADER(panel.h,
            AC_CHECK_LIB(ncurses, initscr,
               AC_CHECK_LIB(panel, update_panels,
                  HAVE_NCURSES_LIBS="y",
                  HAVE_NCURSES_LIBS="n",
                  -lncurses
               )
            )
         )
      )

      if (test "${HAVE_NCURSES_LIBS}" = "y")
      then
         LINK_NCURSES_LIBS="-lncurses -lpanel"
      else
         AC_MSG_WARN(ncurses package not found or not complete! Some of the)
         AC_MSG_WARN(utilities (eg vbox) will not be build...)
      fi

      AC_SUBST(HAVE_NCURSES_LIBS)
      AC_SUBST(LINK_NCURSES_LIBS)
   ]
)

dnl #------------------------------------------------------------------------#
dnl # Checks if we have gettext installed or buildin:                        #
dnl #------------------------------------------------------------------------#

AC_DEFUN(GND_PACKAGE_GETTEXT,
   [
      HAVE_INTL_LIBS="n"
      LINK_INTL_LIBS=""
      CATALOGS_TO_INSTALL=""

      AC_MSG_CHECKING([whether native language support is requested])

      AC_ARG_ENABLE(nls,
         [  --disable-nls           do not use native language support],
         nls_cv_use_nls="${enableval}",
         nls_cv_use_nls="yes"
      )

      AC_MSG_RESULT("${nls_cv_use_nls}")

      AC_CHECK_HEADERS(locale.h)

      if (test "${nls_cv_use_nls}" = "yes")
      then
         nls_cv_use_nls="no"

         AC_CHECK_HEADERS(libintl.h)

         if (test "${ac_cv_header_locale_h}" = "yes")
         then
            AC_MSG_CHECKING([whether locale.h defines LC_MESSAGES])

            AC_TRY_LINK([#include <locale.h>],
               [return LC_MESSAGES],
               gnd_cv_val_LC_MESSAGES="yes",
               gnd_cv_val_LC_MESSAGES="no"
            )

            AC_MSG_RESULT("${gnd_cv_val_LC_MESSAGES}")

            if (test "${gnd_cv_val_LC_MESSAGES}" = "yes")
            then
               AC_DEFINE(HAVE_LC_MESSAGES)
            fi

            if (test "${ac_cv_header_libintl_h}" = "yes")
            then
               AC_CHECK_FUNCS(gettext dcgettext)

               if (test "${ac_cv_func_gettext}" = "yes")
               then
                  nls_cv_use_nls="yes"

                  HAVE_INTL_LIBS="buildin"
               fi

               if (test "${nls_cv_use_nls}" = "no")
               then
                  AC_CHECK_LIB(intl, gettext, AC_DEFINE(HAVE_GETTEXT))
                  AC_CHECK_LIB(intl, dcgettext, AC_DEFINE(HAVE_DCGETTEXT))

                  if (test "${ac_cv_lib_intl_gettext}" = "yes")
                  then
                     HAVE_INTL_LIBS="y"
                     LINK_INTL_LIBS="-lintl"

                     nls_cv_use_nls="yes"
                  fi
               fi
            fi
         fi
      fi

      if (test "${nls_cv_use_nls}" = "yes")
      then
         AC_DEFINE(ENABLE_NLS)

         AC_MSG_CHECKING(for catalogs to be installed)

         NEW_LINGUAS=""

         for lang in ${LINGUAS=$ALL_LINGUAS}
         do
            case "${ALL_LINGUAS}" in
               *$lang*) CATALOGS_TO_INSTALL="${lang} ${CATALOGS_TO_INSTALL}";;
            esac
         done

         AC_MSG_RESULT(${CATALOGS_TO_INSTALL})
      fi

      AC_SUBST(HAVE_INTL_LIBS)
      AC_SUBST(LINK_INTL_LIBS)
      AC_SUBST(CATALOGS_TO_INSTALL)
   ]
)














