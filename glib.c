/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <php_ini.h>
#include <zend_interfaces.h>
#include <ext/standard/info.h>

#include "php_glib.h"

#include "php_glib/glib.h"
#include "php_g/g-list.h"
#include "php_g/g-hash-table.h"


HashTable classes;



/* If you declare any globals in php_glib.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(glib)
*/

/* True global resources - no need for thread safety here */
static int le_glib;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("glib.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_glib_globals, glib_globals)
    STD_PHP_INI_ENTRY("glib.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_glib_globals, glib_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_glib_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_glib_compiled)
{
	char *arg = NULL;
	size_t arg_len, len;
	zend_string *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	strg = strpprintf(0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "glib", arg);

	RETURN_STR(strg);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_glib_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_glib_init_globals(zend_glib_globals *glib_globals)
{
	glib_globals->global_value = 0;
	glib_globals->global_string = NULL;
}
*/
/* }}} */


extern zend_object_handlers php_glib_object_handlers;


/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(glib)
{
    //int zm_startup_glib(int type, int module_number)

	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/

	zend_class_entry ce;
	zend_class_entry *g_hash_table_ce;
	
	zend_object_handlers *handlers= php_glib_object_get_handlers();

	zend_hash_init(&classes, 0, NULL, NULL, 1);

    //PHP_MINIT(glib)(INIT_FUNC_ARGS_PASSTHRU);
        //PHP_MINIT(g)(INIT_FUNC_ARGS_PASSTHRU);
    g_hash_table_ce = php_g_hash_table_class_init(&ce);
        //PHP_MINIT(gobject)(INIT_FUNC_ARGS_PASSTHRU);
        //PHP_MINIT(gio)(INIT_FUNC_ARGS_PASSTHRU);
    //PHP_MINIT(cairo)(INIT_FUNC_ARGS_PASSTHRU);
    //PHP_MINIT(pango)(INIT_FUNC_ARGS_PASSTHRU);
    //PHP_MINIT(glib)(INIT_FUNC_ARGS_PASSTHRU);
    //...
    php_g_list_class_init(&ce);

	return SUCCESS;
}
/* }}} */



/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(glib)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(glib)
{
#if defined(COMPILE_DL_GLIB) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(glib)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(glib)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "glib support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* {{{ glib_functions[]
 *
 * Every user visible function must have an entry in glib_functions[].
 */
const zend_function_entry glib_functions[] = {
    PHP_FE(confirm_glib_compiled,	NULL)		/* For testing, remove later. */
    /* from g-list.h */
    PHP_G_LIST_FE()
    /* from g-hash-table.h */
    PHP_FE(g_str_hash, arginfo_g_str_hash)
    PHP_FE(g_str_equal, arginfo_g_str_equal)
    PHP_FE(g_hash_table_new, arginfo_g_hash_table_new)
    PHP_FE(g_hash_table_add, arginfo_g_hash_table_add)
    PHP_FE(g_hash_table_insert, arginfo_g_hash_table_insert)
    PHP_FE_END	/* Must be the last line in glib_functions[] */
};
/* }}} */


/* {{{ glib_module_entry
 */
zend_module_entry glib_module_entry = {
	STANDARD_MODULE_HEADER,
	"glib",
	glib_functions,
    PHP_MINIT(glib),// zm_startup_glib callback
	PHP_MSHUTDOWN(glib),
	PHP_RINIT(glib),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(glib),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(glib),
	PHP_GLIB_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/*}}} */


#ifdef COMPILE_DL_GLIB
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(glib)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
