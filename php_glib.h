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
  | Author: No Name                                                      |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_GLIB_H
#define PHP_GLIB_H

#include <php.h>
#include <php_ini.h>
#include <zend_interfaces.h>
#include <ext/standard/info.h>

extern zend_module_entry glib_module_entry;
#define phpext_glib_ptr &glib_module_entry

#define PHP_GLIB_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_GLIB_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_GLIB_API __attribute__ ((visibility("default")))
#else
#	define PHP_GLIB_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif



/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(glib)
	zend_long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(glib)
*/

/* Always refer to the globals in your function as GLIB_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define GLIB_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(glib, v)

#if defined(ZTS) && defined(COMPILE_DL_GLIB)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_GLIB_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
