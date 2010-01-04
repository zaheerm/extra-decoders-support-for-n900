AC_PREREQ(2.52)
AC_INIT(decoders-support, 0.2, dominikowski@gmail.com)

AM_INIT_AUTOMAKE

dnl Set PACKAGE_DATA_DIR in config.h.
AH_TEMPLATE(PACKAGE_DATA_DIR, [Package data directory])
if test "x${datadir}" = 'x${prefix}/share'; then
  if test "x${prefix}" = "xNONE"; then
    PACKAGE_DATA_DIR="${ac_default_prefix}/share/${PACKAGE}"
  else
    PACKAGE_DATA_DIR="${prefix}/share/${PACKAGE}"
  fi
else
  if test "x${datarootdir}" = 'x'; then
      PACKAGE_DATA_DIR="${datadir}/${PACKAGE}"
  else
    if test "x${datarootdir}" = 'x${prefix}/share'; then
      if test "x${prefix}" = "xNONE"; then
        PACKAGE_DATA_DIR="${ac_default_prefix}/share/${PACKAGE}"
      else
        PACKAGE_DATA_DIR="${prefix}/share/${PACKAGE}"
      fi
    else
      PACKAGE_DATA_DIR="${datarootdir}/${PACKAGE}"
    fi
  fi
fi


AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${PACKAGE_DATA_DIR}")

dnl Set PACKAGE_LOCALE_DIR in config.h.
AH_TEMPLATE(PACKAGE_LOCALE_DIR, [Package locale directory])
PACKAGE_LOCALE_DIR="${prefix}/share/locale"
AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "${PACKAGE_LOCALE_DIR}")

AC_OUTPUT(
Makefile
data/Makefile
)

