PHP_ARG_WITH([glib],
  [for glib support],
  [AS_HELP_STRING([--with-glib],
    [Include glib support])],
  [no])

if test "$PHP_GLIB" != "no"; then
    PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.56.4])
    PHP_EVAL_INCLINE($GLIB_CFLAGS)
    PHP_EVAL_LIBLINE($GLIB_LIBS, GLIB_SHARED_LIBADD)

    glib_glib_sources="php_glib/list.c "

    glib_sources = $glib_glib_sources

    ext_ns = "Gnome"

    PHP_NEW_EXTENSION(glib, glib.c $glib_sources, $ext_shared, -DGLIB_NS="$ext_ns")
    PHP_ADD_BUILD_DIR($ext_builddir/php_glib, 1)
    PHP_SUBST(GLIB_SHARED_LIBADD)

fi
