/*
+----------------------------------------------------------------------+
| PHP Version 8                                                        |
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <php_config.h>
#include <php.h>
#include <php_ini.h>
#include <zend_interfaces.h>
#include <ext/standard/info.h>

#include <glib.h>

#include "php_glib.h"

#include "php_glib/list.h"


extern HashTable         classes;
extern zend_module_entry gtk_module_entry;


zend_class_entry     *php_g_list_class_entry;// struct
zend_object_handlers  php_g_list_handlers;




enum _php_g_list_properties {
    PHP_G_LIST_DATA = 1,
    PHP_G_LIST_NEXT = 2,
    PHP_G_LIST_PREV = 3
};
//typedef enum php_g_list_properties php_g_list_properties;



/*----------------------------------------------------------------------+
 | Internal                                                             |
 +----------------------------------------------------------------------*/

static const zend_function_entry
php_g_list_methods[] = {
    PHP_ME(g_list, __construct, arginfo_g_list___construct, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/*----------------------------------------------------------------------+
 | Zend Handler                                                         |
 +----------------------------------------------------------------------*/
static php_g_list *first_this = NULL;
/* {{{ php_g_list_create_object */
static zend_object*
php_g_list_create_object(zend_class_entry *class_type)
{
    php_g_list *intern = zend_object_alloc(sizeof(php_g_list), class_type);
    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);
    //php_printf("php_g_list_create_object: %p\n", &intern->std);
    //php_printf("php_g_list_create_object: %p\n", intern);

    if (first_this==NULL)
        first_this = intern;


    ZVAL_NULL(&intern->data);
    ZVAL_NULL(&intern->next);
    ZVAL_NULL(&intern->prev);


    intern->std.handlers = &php_g_list_handlers;

    return &intern->std;
}
/* }}} php_g_list_create_object */

#define ENABLE_DUMP 0

static void
php_g_list_dtor_object(zend_object *obj) {
    php_g_list *intern = ZOBJ_TO_PHP_G_LIST(obj);
    //fprintf(stderr, "php_g_list_dtor_object: %p (#%d)\n", intern, intern->std.handle);

    /*
    if (first_this!=NULL) {
        char *output_2 = php_g_list_dump(php_g_list_first(first_this), 2);
        php_printf("        %s\n", output_2);
        g_free(output_2);
    }
    */


#if 0 && ENABLE_DUMP
        char *output = php_g_list_dump(php_g_list_first(intern), 0);
        php_printf("%s\n", output);
        g_free(output);
#endif
    if (!ZVAL_IS_NULL(&intern->data)) {
        Z_TRY_DELREF_P(&intern->data);
        ZVAL_NULL(&intern->data);
    }


#if 0
    ZVAL_NULL(&intern->prev);
    ZVAL_NULL(&intern->next);
#else
    if (Z_TYPE(intern->prev)==IS_OBJECT) {
        if(intern->prev.value.obj->gc.refcount>1) {
            zend_object_release(intern->prev.value.obj);
            //GC_DELREF(intern->prev.value.obj);
        } else {
            //zend_object_release(intern->prev.value.obj);
            GC_DELREF(intern->prev.value.obj);
            ZVAL_NULL(&intern->prev);
        }
    }

    if (Z_TYPE(intern->next)==IS_OBJECT) {
        if(intern->next.value.obj->gc.refcount>1) {
            zend_object_release(intern->next.value.obj);
            //GC_DELREF(intern->next.value.obj);
        } else {
            //zend_object_release(intern->next.value.obj);
            GC_DELREF(intern->next.value.obj);
            ZVAL_NULL(&intern->next);
        }
    }


#endif

#if ENABLE_DUMP
    char *output_2 = php_g_list_dump(php_g_list_first(intern), 2);
    php_printf("        %s\n", output_2);
    g_free(output_2);

#endif
    //php_printf("<============ php_g_list_dtor_object: %p\n", intern);

}

/* {{{ php_g_list_free_object */
static void
php_g_list_free_object(zend_object *object)
{
    php_g_list *intern = ZOBJ_TO_PHP_G_LIST(object);
    //php_printf("php_g_list_free_object: %p( %d) #%d\n", intern, intern->std.gc.refcount, intern->std.handle);

    //Z_TRY_DELREF(intern->prev);
    //Z_TRY_DELREF(intern->next);

    zend_object_std_dtor(&intern->std);
}
/* }}} php_g_list_free_object */




/** TODO: implement all the types */
/** rename it by : php_gtkml_setter_[double|long|string] */
static void
php_g_list_setter_mixed(php_g_list *intern, zval *value, char *name, zval *dest) {
    ZVAL_COPY(dest, value);//"mixed"
}




/** TODO: implement all the types */
/** rename it by : php_gtkml_setter_[double|long|string] */
static void
php_g_list_setter_php_glist(php_g_list *intern, zval *value, char *name, zval *dest) {
    ZVAL_COPY(dest, value);//"php_GList"
}




struct PhpGListProperty {
  const char *name;
  int code;
  void (*setter) (php_g_list *intern, zval *rvalue, char *name, zval *dest);
};

static const struct PhpGListProperty php_g_list_properties[] = {
    {"data", PHP_G_LIST_DATA, php_g_list_setter_mixed},
    {"next", PHP_G_LIST_NEXT, php_g_list_setter_php_glist},
    {"prev", PHP_G_LIST_PREV, php_g_list_setter_php_glist}
};


const struct PhpGListProperty*
php_g_list_properties_lookup (const char *str, size_t len)
{
    if (len == 4) {
        if (str[0] == 'd' && str[1] == 'a' && str[2] == 't' && str[3] == 'a') {
            return &php_g_list_properties[0];
        } else if (str[0] == 'n' && str[1] == 'e' && str[2] == 'x' && str[3] == 't') {
            return &php_g_list_properties[1];
        } else if (str[0] == 'p' && str[1] == 'r' && str[2] == 'e' && str[3] == 'v') {
            return &php_g_list_properties[2];
        }
    }
    return 0;
}

/* {{{ php_g_list_read_property */
static zval*
php_g_list_read_property(zend_object *object, zend_string *member_str, int type, void **cache_slot, zval *rv)
{
    php_g_list *intern = ZOBJ_TO_PHP_G_LIST(object);

    const struct PhpGListProperty *cmd = php_g_list_properties_lookup(member_str->val, member_str->len);
    if (cmd) {
        switch(cmd->code) {
        case PHP_G_LIST_DATA:
            ZVAL_COPY(rv, &intern->data);
            return rv;
            break;
        case PHP_G_LIST_NEXT:
            ZVAL_COPY(rv, &intern->next);
            return rv;
            break;
        case PHP_G_LIST_PREV:
            ZVAL_COPY(rv, &intern->prev);
            return rv;
            break;
        default:
            //zend_internal_type_error(1, "Internal bug,");
            //zend_error(E_USER_NOTICE, "Internal bug,");
            break;
        }
    } else {
        // property not found
    }

    return rv;
}
/* }}} */

/* {{{ php_g_list_write_property */
static zval*
php_g_list_write_property(zend_object *object, zend_string *member_str, zval *value, void **cache_slot)
{
    php_g_list *intern = ZOBJ_TO_PHP_G_LIST(object);
    // struct
    const struct PhpGListProperty *cmd = php_g_list_properties_lookup(member_str->val, member_str->len);
    if (cmd) {
        switch(cmd->code) {
        case PHP_G_LIST_DATA:
            cmd->setter(intern, value, member_str->val, &intern->data);
        break;
        case PHP_G_LIST_NEXT:
            cmd->setter(intern, value, member_str->val, &intern->next);
        break;
        case PHP_G_LIST_PREV:
            cmd->setter(intern, value, member_str->val, &intern->prev);
        break;
        }
    } else {
        // property not found
    }
    return value;
}
/* }}} */

/* {{{ php_g_list_get_property_ptr_ptr */
static zval*
php_g_list_get_property_ptr_ptr(zend_object *object, zend_string *member_str, int type, void **cache_slot)
{
    php_g_list *intern = ZOBJ_TO_PHP_G_LIST(object);
    zval *retval = NULL;

    const struct PhpGListProperty *cmd = php_g_list_properties_lookup(member_str->val, member_str->len);
    if (cmd) {
        switch(cmd->code) {
        case PHP_G_LIST_DATA:
            return &intern->data;
        case PHP_G_LIST_NEXT:
            return &intern->next;
        case PHP_G_LIST_PREV:
            return &intern->prev;
        }
    } else {
        // Error
    }

    return retval;
}/* }}} */





/* {{{ php_g_list_get_debug_info */
static HashTable*
php_g_list_get_debug_info(zend_object *object, int *is_temp)
{
    php_g_list  *intern = ZOBJ_TO_PHP_G_LIST(object);
    HashTable   *debug_info,
    *std_props;

    *is_temp = 1;
    std_props = zend_std_get_properties(object);
    debug_info = zend_array_dup(std_props);


    //gchar *ptr = g_strdup_printf("%p", intern);
    //zval zptr; ZVAL_STRING(&zptr, ptr);
    //zend_hash_str_update(debug_info, "ptr", sizeof("ptr")-1, &zptr);
    //zval zref; ZVAL_LONG(&zref, intern->std.gc.refcount);
    //zend_hash_str_update(debug_info, "refcount", sizeof("refcount")-1, &zref);
    zval zprev; ZVAL_COPY(&zprev, &intern->prev);
    zend_hash_str_update(debug_info, "prev", sizeof("prev")-1, &zprev);
    zval zdata; ZVAL_COPY(&zdata, &intern->data);
    zend_hash_str_update(debug_info, "data", sizeof("data")-1, &zdata);
    zval znext; ZVAL_COPY(&znext, &intern->next);
    zend_hash_str_update(debug_info, "next", sizeof("next")-1, &znext);

    //g_free(ptr);
    return debug_info;
}
/* }}} */





static zend_object_handlers*
php_g_list_get_handlers()
{
    memcpy(&php_g_list_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    php_g_list_handlers.offset = PHP_G_LIST_OFFSET;
    php_g_list_handlers.dtor_obj = php_g_list_dtor_object;
    php_g_list_handlers.free_obj = php_g_list_free_object;

    php_g_list_handlers.read_property = php_g_list_read_property;
    php_g_list_handlers.write_property = php_g_list_write_property;
    php_g_list_handlers.get_property_ptr_ptr = php_g_list_get_property_ptr_ptr;

    php_g_list_handlers.get_debug_info = php_g_list_get_debug_info;


    return &php_g_list_handlers;
}

/*----------------------------------------------------------------------+
 | PHP_MINIT                                                            |
 +----------------------------------------------------------------------*/

 /*{{{ php_g_list_class_init */
zend_class_entry*
php_g_list_class_init(zend_class_entry *container_ce, zend_class_entry *parent_ce) {
    php_g_list_get_handlers();
    PHP_GTK_INIT_CLASS_ENTRY((*container_ce), "GList", php_g_list_methods);
    php_g_list_class_entry = zend_register_internal_class_ex(container_ce, parent_ce);
    php_g_list_class_entry->create_object = php_g_list_create_object;

    return php_g_list_class_entry;
}/*}}} */

/*----------------------------------------------------------------------+
 | Zend-User utils                                                      |
 +----------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib-object.h>
#include "php_gobject/object.h"
#include "php_gdk/pixbuf.h"
#include "php_gdk/rectangle.h"

GList*
php_g_list_export(php_g_list *list){
    GList *glist=0;

    php_g_list *tmp;
    php_g_list *prev=NULL;
    php_g_list *next=NULL;
    zend_long i=0;
    zend_long counter=0;

    tmp = list;
    while (tmp) {
        switch (Z_TYPE(tmp->data)) {
        case IS_OBJECT:
            /*if (php_gdk_pixbuf_class_entry==Z_OBJ(tmp->data)->ce) {
                php_gdk_pixbuf *pixbuf = ZVAL_GET_PHP_GDK_PIXBUF(&tmp->data);
                glist = g_list_append(glist, GDK_PIXBUF(pixbuf->ptr));
            } else*/ {
                php_printf("Unimplemented Z_OBJ: %s\n", Z_OBJ(tmp->data)->ce->name->val);
            }
            break;
        default:
            php_printf("Unimplemented Z_TYPE\n");
            break;
        }
        tmp = ZVAL_GET_PHP_G_LIST(&tmp->next);
    }
    return glist;
}

GList*
php_g_list_get_ptr(php_g_list *val)
{
    GList *list = NULL;

    return list;
}

php_g_list*
php_g_list_new(GList *list) {
    zend_object *zlist;
    php_g_list *plist = NULL;
    php_g_list *prev = NULL;
    php_g_list *head = NULL;

    GList *it;
    for(it=list; it; it=it->next) {
        zlist = php_g_list_create_object(php_g_list_class_entry);
        plist = ZOBJ_TO_PHP_G_LIST(zlist);
        /*if (plist->ptr) g_list_free_1(plist->ptr);

        plist->ptr = it;*/

        if (prev) {
            ZVAL_OBJ(&prev->next, zlist);

            //GC_REFCOUNT(&prev->std)++;
            Z_ADDREF(prev);
        }
        ZVAL_OBJ(&plist->prev, &prev->std);

        if (G_IS_OBJECT(it->data)) {
            GObject *object = (GObject *)it->data;
            zend_object *z_object = g_object_get_data(object, "zend_object");
            ZVAL_OBJ(&plist->data, z_object);

            //GC_REFCOUNT(z_object)++;
            Z_ADDREF(plist->data);
        } else {
            printf("TODO: data is not GObject\n");
        }

        prev = plist;
        if (it==list) {
            head = plist;
        }
    }

    //g_print("ref: %d\n", head->std.gc.refcount);
    return head;
}

/*----------------------------------------------------------------------+
 | Zend-User API                                                        |
 +----------------------------------------------------------------------*/

php_g_list *
php_g_list_append(php_g_list *php_list, zval *zdata) {
    php_g_list *php_last;
    zend_object *z_ret;

    if (NULL==php_list) {
        z_ret = php_g_list_create_object(php_g_list_class_entry);
        php_g_list *php_ret = ZOBJ_TO_PHP_G_LIST(z_ret);
        ZVAL_COPY(&php_ret->data, zdata);

    } else {
        z_ret = php_g_list_create_object(php_g_list_class_entry);
        php_g_list *php_ret = ZOBJ_TO_PHP_G_LIST(z_ret);
        ZVAL_COPY(&php_ret->data, zdata);
        zend_gc_addref(&z_ret->gc);

        php_last = php_g_list_last(php_list);

        //link
        ZVAL_OBJ(&php_last->next, z_ret);
        ZVAL_OBJ(&php_ret->prev, &php_last->std);
        Z_TRY_ADDREF(php_ret->prev);


        z_ret = &php_list->std;
        zend_gc_addref(&z_ret->gc);
    }

    return ZOBJ_TO_PHP_G_LIST(z_ret);
}

php_g_list *
php_g_list_prepend(php_g_list *php_list, zval *zdata) {
    zend_object *z_ret;

    z_ret = php_g_list_create_object(php_g_list_class_entry);
    php_g_list *php_ret = ZOBJ_TO_PHP_G_LIST(z_ret);
    ZVAL_COPY(&php_ret->data, zdata);

    if (NULL==php_list) {
    } else {
        ZVAL_OBJ(&php_ret->next, &php_list->std);

        if (ZVAL_IS_NULL(&php_list->prev)) {
            zend_gc_addref(&php_list->std.gc);

        } else {
            ZVAL_OBJ(&php_ret->prev, Z_OBJ(php_list->prev));

            //zend_object *prev = Z_OBJ(php_list->prev);
            //php_g_list *php_prev = ZOBJ_TO_PHP_G_LIST(prev);
            php_g_list *php_prev = ZVAL_GET_PHP_G_LIST(&php_list->prev);
            ZVAL_OBJ(&php_prev->next, &php_ret->std);

            zend_gc_addref(&z_ret->gc);
        }
        ZVAL_OBJ(&php_list->prev, &php_ret->std);
        GC_ADDREF(z_ret);
    }

    return php_ret;
}
php_g_list *
php_g_list_insert(php_g_list *php_list, zval *zdata, int position) {
    php_g_list *new_list;
    php_g_list *tmp_list;

    if (position < 0)
      return php_g_list_append (php_list, zdata);
    else if (position == 0)
      return php_g_list_prepend (php_list, zdata);

    tmp_list = php_g_list_nth (php_list, position);
    if (!tmp_list)
      return php_g_list_append (php_list, zdata);

    zend_object *list = php_g_list_create_object(php_g_list_class_entry);
    new_list = ZOBJ_TO_PHP_G_LIST(list);
    ZVAL_COPY(&new_list->data, zdata);
    ZVAL_COPY(&new_list->prev, &tmp_list->prev);

    php_g_list *prev_list = ZOBJ_TO_PHP_G_LIST(&tmp_list->prev);
    ZVAL_OBJ(&prev_list->next, &new_list->std);
    ZVAL_OBJ(&new_list->next, &tmp_list->std);
    ZVAL_OBJ(&tmp_list->prev, &new_list->std);

    return php_list;
}

php_g_list *
php_g_list_insert_before(php_g_list *list, php_g_list *sibling, zval *data) {
    zend_object *new_list;
    if (!list) {
        new_list = php_g_list_create_object(php_g_list_class_entry);
        list = ZOBJ_TO_PHP_G_LIST(new_list);
        ZVAL_COPY(&list->data, data);
        return list;
    } else if (sibling) {
      php_g_list *node;

      new_list = php_g_list_create_object(php_g_list_class_entry);
      node = ZOBJ_TO_PHP_G_LIST(new_list);
      ZVAL_COPY(&node->data, data);
      ZVAL_COPY(&node->prev, &sibling->prev);
      ZVAL_OBJ(&node->next, &sibling->std);
      ZVAL_OBJ(&sibling->prev, &node->std);
      if (ZVAL_IS_NULL(&node->prev)) {
          GC_ADDREF(&sibling->std);
          GC_ADDREF(&node->std);
          return node;
      } else {
          //---- TODO check
          php_g_list *prev = ZVAL_GET_PHP_G_LIST(&node->prev);
          ZVAL_OBJ(&prev->next, &node->std);
          GC_ADDREF(&node->std);
          return list;
      }

    } else {
        php_g_list *last = list;

        while(ZVAL_IS_PHP_G_LIST(&last->next))
            last = ZVAL_GET_PHP_G_LIST(&last->next);

        new_list = php_g_list_create_object(php_g_list_class_entry);
        ZVAL_OBJ(&last->next, new_list);
        php_g_list *next = ZVAL_GET_PHP_G_LIST(&last->next);
        ZVAL_COPY(&next->data, data);
        ZVAL_OBJ(&next->prev, &last->std);
        ZVAL_NULL(&next->next);
        GC_ADDREF(&last->std);

        GC_ADDREF(&list->std);
        return list;
    }
}

#if ZEND_DEBUG
static char *php_g_list_dump_zval(zval *data, int tab);
static GList *recursions = NULL;// of php_glib_list
#define ENABLE_G_OBJECT_DUMP 0

static char *php_g_list_dump_zobj(zend_object *z_object, int tab){
    char *str;
    char *tmp_prev;
    char *tmp_data;
    char *tmp_next;
#if ENABLE_G_OBJECT_DUMP
    php_gobject_object *ptr = NULL;

    ptr = z_object==NULL ? NULL : ((char*)z_object) - z_object->handlers->offset;
#endif

    char *t = g_strdup_printf("%*.s", tab*4, "");
    GList *is_recursion = g_list_find(recursions, (gconstpointer)z_object);

    if (z_object==NULL) {
        str = g_strdup_printf("NULL");
    } else if (NULL!=is_recursion) {
        //str = g_strdup_printf("*RECURSION*");
        str = g_strdup_printf("\e[1;34m%s\e[0;m\e[2;31m#%d\e[0;m(\e[2;35m%d\e[0;m) {*RECURSION*}", z_object->ce->name->val, z_object->handle, z_object->gc.refcount);
    } else if (z_object->ce==php_g_list_class_entry) {
        php_g_list *list = ZOBJ_TO_PHP_G_LIST(z_object);
        recursions = g_list_append(recursions, z_object);
        zend_object *prev = ZVAL_IS_NULL(&list->prev) ? NULL : &ZVAL_GET_PHP_G_LIST(&list->prev)->std;
        zend_object *next = ZVAL_IS_NULL(&list->next) ? NULL : &ZVAL_GET_PHP_G_LIST(&list->next)->std;

        tmp_prev = php_g_list_dump_zobj(prev, tab+1);
        tmp_data = php_g_list_dump_zval(&list->data, tab+1);
        tmp_next = php_g_list_dump_zobj(next, tab+1);

        str = g_strdup_printf("\e[1;34m%s\e[0;m\e[1;31m#%d\e[0;m(\e[2;35m%d\e[0;m) %p {\n"
                "%s    prev: %s,\n"
                "%s    data: %s,\n"
                "%s    next: %s\n"
                "%s}",
                list->std.ce->name->val,
                list->std.handle,
                list->std.gc.refcount, list,//&list->std,
                t, tmp_prev,
                t, tmp_data,
                t, tmp_next,
                t);
        g_free(tmp_prev);
        g_free(tmp_data);
        g_free(tmp_next);
    }
#if ENABLE_G_OBJECT_DUMP
    else if (G_IS_OBJECT(ptr->ptr)) {
        str = g_strdup_printf("\e[2;34m%s\e[0;m(\e[2;35m%d\e[0;m)\e[2;31m#%d\e[0;m{ ref_count: %d}",
                              g_type_name_from_instance((GTypeInstance*)ptr->ptr),
                              z_object->gc.refcount,
                              z_object->handle,
                              ptr->ptr->ref_count);
    }
#endif
    else {// for next, prev
        str = g_strdup_printf("\e[2;34m%s\e[0;m(\e[2;35m%d\e[0;m)\e[2;31m#%d\e[0;m{ %s}",
                              z_object->ce->name->val,
                              z_object->gc.refcount,
                              z_object->handle,
                              "..."
                              );
    }
    g_free(t);


    return str;
}


//#include "php_gtk/button.h"
static char*
php_g_list_dump_zval(zval *data, int tab) {
    char *str = NULL;
    char *tmp = NULL;
    char *t = g_strdup_printf("%*.s", tab*4, "");
    if (ZVAL_IS_NULL(data)) {
        str = g_strdup_printf("\e[2;34m%s\e[0;m{ %s}", "zval", "NULL");
    } else if (Z_TYPE_P(data)==IS_STRING) {
        str = g_strdup_printf("\e[2;34m%s\e[0;m{ \e[1;31m\"%s\"\e[0;m}", "zval", data->value.str->val);
    } else if (Z_TYPE_P(data)==IS_LONG) {
        str = g_strdup_printf("\e[2;34m%s\e[0;m{ %ld}", "zval", data->value.lval);
    }
    else if (Z_TYPE_P(data)==IS_OBJECT) {
        tmp = php_g_list_dump_zobj(data->value.obj, tab+1);
        str = g_strdup_printf("\e[2;34m%s\e[0;m(\e[2;35m%d\e[0;m){ %s\n%s}",
                              "zval",
                              data->value.counted->gc.refcount,
                              tmp,
                              t);
        g_free(tmp);
    }
    else {
        str = g_strdup_printf("\e[2;34m%s\e[0;m{ %s}", "zval", "Unknow");
    }
    g_free(t);
    return str;
}
char *php_g_list_dump(php_g_list *list, int tab) {

    char *str = php_g_list_dump_zobj(&list->std, tab);
    g_list_free(recursions);
    recursions=NULL;
    return str;
}
/* {{{ proto string php_g_list_dump(GList list)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(g_list_dump)
{
    zval *list = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &list) == FAILURE) {
        return;
    }

    char *str = php_g_list_dump_zval(list, 0);
    g_print("%s\n", str);
    g_free(str);
    g_list_free(recursions);
    recursions=NULL;

    RETURN_NULL();
}
#endif

static void php_g_func(gpointer data, gpointer user_data)
{

}

static gint php_g_compare_data_func(gconstpointer a, gconstpointer b, gpointer user_data)
{

}

static gint php_g_compare_func(zval *z_func, zval *z_a, zval *z_b)
{
#if 0
    php_printf("%s %s\n", z_a->value.str->val, z_b->value.str->val);
#else
    int result;
    int param_count = 2;
    zval retval;
    zval params[2];
    ZVAL_COPY(&params[0], z_a);
    ZVAL_COPY(&params[1], z_b);

    result = call_user_function(NULL, NULL, z_func, &retval, param_count, params);
    if (result==FAILURE) {
        php_printf("Unexpected 101 : php_g_compare_func\n");
    } else if (Z_TYPE(retval)==IS_LONG) {
        return retval.value.lval;
    } else {
        php_printf("Unexpected 102 : php_g_compare_func %d\n", Z_TYPE(retval));
    }
#endif

#if 0
    zval *hash_func;
    php_printf("%s(%d)\n", "php_g_relation_hash_func", php_g_relation_this->field);
    if (php_g_relation_this->field<1) {
        hash_func = &php_g_relation_this->hash_func;
    } else {
        hash_func = &php_g_relation_this->hash_func_1;
    }

    zval *val = (zval *)v;
    if (ZVAL_IS_NULL(hash_func)) {
        if (Z_TYPE_P(val)==IS_STRING) {
            guint hash = g_str_hash(val->value.str->val);
            return hash;
        } else if (Z_TYPE_P(val)==IS_OBJECT ) {
            char *name = php_g_relation_tostring_object(val, NULL);
            guint hash = g_str_hash(name);
            g_free(name);
            return hash;
        } else {
            php_printf("Unexpected 21 : php_g_relation_hash_func\n");
        }
    } else {
        int result;
        int param_count = 1;
        zval retval;
        zval params[1];
        if (Z_TYPE_P(val)==IS_STRING) {
            ZVAL_COPY(&params[0], val);
            php_printf("  String %s\n", val->value.str->val);
        } else if (Z_TYPE_P(val)==IS_OBJECT) {
#if 1
            ZVAL_COPY(&params[0], val);
            php_printf("  Object %s\n", val->value.obj->ce->name->val);
#else
            char *name = php_g_relation_tostring_object(val, NULL);
            ZVAL_STRING(&params[0], name);
            g_free(name);
#endif
        } else if (Z_TYPE_P(val)==IS_LONG) {
            ZVAL_COPY(&params[0], val);
            php_printf("  Int %d\n", val->value.lval);
        } else {
            php_printf("=> %d == %d\n", Z_TYPE_P(val), IS_UNDEF);// IS_LONG

        }
        //php_printf("%d\n", Z_TYPE_P(hash_func));
        result = call_user_function(NULL, NULL, hash_func, &retval, param_count, params);
        if (result==FAILURE) {
            php_printf("Unexpected 22 : php_g_relation_hash_func\n");
        } else if (Z_TYPE(retval)==IS_LONG) {
            return retval.value.lval;
        } else {
            php_printf("Unexpected 23 : php_g_relation_hash_func: %d\n", Z_TYPE(retval));
        }
    }
    php_printf("Unexpected 22 : php_g_relation_hash_func\n");
    // zend call function
#endif
    return 0;
}

static php_g_list*
php_g_list_insert_sorted_real (php_g_list *php_list,
                               zval *z_data,
                               zval *z_func,
                               zval *z_user_data)
{
  zend_object *z_tmp_list = php_list ? &php_list->std : NULL;
  php_g_list *tmp_list = php_list;

  zend_object *z_new_list = NULL;
  php_g_list *new_list = NULL;

  gint cmp;

#if 1
  if (!php_list)
    {
      z_new_list = php_g_list_create_object(php_g_list_class_entry);
      new_list = ZOBJ_TO_PHP_G_LIST(z_new_list);
      ZVAL_COPY(&new_list->data, z_data);
      g_print("return new_list\n");
      return new_list;
    }

  //cmp = ((GCompareDataFunc) func) (data, tmp_list->data, user_data);
  cmp = php_g_compare_func(z_func, z_data, &tmp_list->data);

  while ((ZVAL_GET_PHP_G_LIST(&tmp_list->next)) && (cmp > 0))
    {
      tmp_list = ZVAL_GET_PHP_G_LIST(&tmp_list->next);

      //cmp = ((GCompareDataFunc) func) (data, tmp_list->data, user_data);
      cmp = php_g_compare_func(z_func, z_data, &tmp_list->data);
    }

  z_new_list = php_g_list_create_object(php_g_list_class_entry);
  new_list = ZOBJ_TO_PHP_G_LIST(z_new_list);
  ZVAL_COPY(&new_list->data, z_data);

  if ((!ZVAL_GET_PHP_G_LIST(&tmp_list->next)) && (cmp > 0))
    {
      ZVAL_OBJ(&tmp_list->next, z_new_list);
      ZVAL_OBJ(&new_list->prev, &tmp_list->std);
      Z_TRY_ADDREF(new_list->prev);//--------------

      zend_gc_addref(&php_list->std.gc);
      g_print("return php_list\n");
      return php_list;
    }








  php_g_list *prev_list = ZVAL_GET_PHP_G_LIST(&tmp_list->prev);
  if (prev_list)
    {
      ZVAL_OBJ(&prev_list->next, z_new_list);
      prev_list = ZVAL_GET_PHP_G_LIST(&tmp_list->prev);
      ZVAL_OBJ(&new_list->prev, &prev_list->std);
    }
  ZVAL_OBJ(&new_list->next, &tmp_list->std);
  ZVAL_OBJ(&tmp_list->prev, &new_list->std);
  //Z_TRY_ADDREF(tmp_list->prev);

  if (tmp_list == php_list) {
      //zend_gc_addref(&new_list->std.gc);
      g_print("return new_list[2]\n");
      return new_list;
  } else {
      zend_gc_addref(&php_list->std.gc);
      g_print("return php_list[2]\n");
      return php_list;
  }

#else
  GList *tmp_list = list;
  GList *new_list;
  gint cmp;

  g_return_val_if_fail (func != NULL, list);

  if (!list)
    {
      new_list = _g_list_alloc0 ();
      new_list->data = data;
      return new_list;
    }

  cmp = ((GCompareDataFunc) func) (data, tmp_list->data, user_data);

  while ((tmp_list->next) && (cmp > 0))
    {
      tmp_list = tmp_list->next;

      cmp = ((GCompareDataFunc) func) (data, tmp_list->data, user_data);
    }

  new_list = _g_list_alloc0 ();
  new_list->data = data;

  if ((!tmp_list->next) && (cmp > 0))
    {
      tmp_list->next = new_list;
      new_list->prev = tmp_list;
      return list;
    }

  if (tmp_list->prev)
    {
      tmp_list->prev->next = new_list;
      new_list->prev = tmp_list->prev;
    }
  new_list->next = tmp_list;
  tmp_list->prev = new_list;

  if (tmp_list == list)
    return new_list;
  else
    return list;
#endif
}

php_g_list*
php_g_list_insert_sorted(php_g_list *php_list, zval *z_data, zval *z_func) {
    php_g_list *ret = php_g_list_insert_sorted_real(php_list, z_data, z_func, NULL);
    return ret;
}

php_g_list*
php_g_list_remove (php_g_list *list, zval *data)
{
    php_g_list *tmp;
    php_g_list *prev=NULL;
    php_g_list *next=NULL;
    zend_long i=0;
    zend_long counter=0;

    tmp = list;
    while (tmp) {
        if (! zend_is_identical(&tmp->data, data))
          tmp = ZVAL_GET_PHP_G_LIST(&tmp->next);
        else {

            if (list == tmp)
              list = ZVAL_GET_PHP_G_LIST(&list->next);

            prev = ZVAL_GET_PHP_G_LIST(&tmp->prev);
            next = ZVAL_GET_PHP_G_LIST(&tmp->next);

            // 1) detach
            if (prev) {
                if(next) {
                    ZVAL_OBJ(&prev->next, &next->std);
                    GC_ADDREF(&next->std);
                } else {
                    ZVAL_NULL(&prev->next);
                }
                GC_ADDREF(&prev->std);
                counter++;// release(tmp); GC_REFCOUNT(&tmp->std)--;
            } else if (next) {
                GC_ADDREF(&next->std);
            }

            if (next) {
                if(prev) {
                    ZVAL_OBJ(&next->prev, &prev->std);
                    GC_ADDREF(&prev->std);
                } else {
                    ZVAL_NULL(&next->prev);
                }
                GC_ADDREF(&next->std);
                counter++;// release(tmp); GC_REFCOUNT(&tmp->std)--;
            }

            // 2) attach
            php_g_list_dtor_object(&tmp->std);
            for(i=0; i<counter; i++) {
                zend_object_release(&tmp->std);
            }

          break;
        }

    }
    return list;
}

static php_g_list*
php_g_list_remove_link_real(php_g_list *list, php_g_list *link) {
    php_g_list *prev;
    php_g_list *next;
    zend_long counter=0;

    if (link == NULL)
        return list;

    prev = ZVAL_GET_PHP_G_LIST(&link->prev);
    next = ZVAL_GET_PHP_G_LIST(&link->next);

    if (prev) {
        if (ZVAL_GET_PHP_G_LIST(&prev->next) == link) {
            if (next) {
                ZVAL_OBJ(&prev->next, &next->std);
                GC_ADDREF(&next->std);
            } else {
                ZVAL_NULL(&prev->next);
            }
            GC_ADDREF(&prev->std);
            counter++;
        }
        else
            g_warning ("1]corrupted double-linked list detected");
    }
    if (next) {
        if (ZVAL_GET_PHP_G_LIST(&next->prev) == link) {
            if (prev) {
                ZVAL_OBJ(&next->prev, &prev->std);
                GC_ADDREF(&prev->std);
            } else {
                ZVAL_NULL(&next->prev);
            }
            GC_ADDREF(&next->std);
            counter++;
        }
        else
            g_warning ("2]corrupted double-linked list detected");
    }

    if (link == list)
        list = ZVAL_GET_PHP_G_LIST(&list->next);

    // 2) dettach
    php_g_list_dtor_object(&link->std);
    //  ZVAL_NULL(&link->next);
    //  ZVAL_NULL(&link->prev);
    for(; counter>1; counter--) {
        zend_object_release(&link->std);
    }

    return list;
}
php_g_list *
php_g_list_remove_link(php_g_list *list, php_g_list *llink) {
    return php_g_list_remove_link_real(list, llink);
}
php_g_list *php_g_list_delete_link(php_g_list *list, php_g_list *link_) {
    list = php_g_list_remove_link_real(list, link_);
    // TODO: use DEREF
    //zend_object_release(&link_->std);
    return list;
}


php_g_list*
php_g_list_remove_all(php_g_list *list, zval *data) {
    php_g_list *tmp = list;
    zend_long i = 0;

    while (tmp) {
        if (!zend_is_identical(&tmp->data, data))
            tmp = ZVAL_GET_PHP_G_LIST(&tmp->next);
        else {
            php_g_list *next = ZVAL_GET_PHP_G_LIST(&tmp->next);
            php_g_list *prev = ZVAL_GET_PHP_G_LIST(&tmp->prev);
            zend_long counter = 0;

            // 2) attach
            if (prev) {
                if (next) {
                    ZVAL_OBJ(&prev->next, &next->std);
                    GC_ADDREF(&next->std);
                } else {
                    ZVAL_NULL(&prev->next);
                }
                GC_ADDREF(&prev->std);
                counter++;
            } else {
                list = next;
            }
            if (next) {
                if (prev) {
                    ZVAL_OBJ(&next->prev, &prev->std);
                    GC_ADDREF(&prev->std);
                } else {
                    ZVAL_NULL(&next->prev);
                }
                GC_ADDREF(&next->std);
                counter++;
            }

            // 2) dettach
            php_g_list_dtor_object(&tmp->std);
            for(i=0; i<counter; i++) {
                zend_object_release(&tmp->std);
            }

            tmp = next;
        }
    }
    return list;
}

void
php_g_list_free(php_g_list *list) {
    if (list==NULL) {
        return;
    }

    php_g_list *tmp = list;
    php_g_list *last = php_g_list_last(tmp);
    while(last) {
        tmp = ZVAL_GET_PHP_G_LIST(&last->prev);
        zend_object_release(&last->std);
        if (tmp)
            ZVAL_NULL(&tmp->next);
        last = tmp;
    }
}

zend_long
php_g_list_length(php_g_list *list) {
    zend_long length = 0;

    while(list) {
        length++;
        list = ZVAL_IS_PHP_G_LIST(&list->next) ? ZVAL_GET_PHP_G_LIST(&list->next) : NULL;
    }

    return length;
}

php_g_list*
php_g_list_first(php_g_list *list) {
    php_g_list *first;
    if (list==NULL) {
        return NULL;
    }

    while(list) {
        first = list;
        if (Z_TYPE(list->prev)==IS_OBJECT) {
            list = ZOBJ_TO_PHP_G_LIST(Z_OBJ(list->prev));
        } else {
            list = NULL;
        }
    }

    return first;
}

php_g_list *
php_g_list_last(php_g_list *list) {
    php_g_list *last;

    if (list==NULL) {
        return NULL;
    }

    while(list) {
        last = list;
        if (Z_TYPE(list->next)==IS_OBJECT) {
            list = ZOBJ_TO_PHP_G_LIST(Z_OBJ(list->next));
        } else {
            list = NULL;
        }
    }

    return last;
}
php_g_list *
php_g_list_nth(php_g_list *php_list, zend_long n){
    php_g_list *list = php_list;
    while(list && (n-- > 0))
        if (Z_TYPE(list->next)==IS_OBJECT) {
            list = ZOBJ_TO_PHP_G_LIST(Z_OBJ(list->next));
        } else {
            list = NULL;
        }

    return list;
}


/*----------------------------------------------------------------------+
 | PHP_METHOD                                                           |
 +----------------------------------------------------------------------*/

 /* {{{ g_list::__construct() */
PHP_METHOD(g_list, __construct)
{
    zval *data=NULL;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(data)
    ZEND_PARSE_PARAMETERS_END();

    zend_object *zobj = Z_OBJ_P(getThis());
    php_g_list *self = ZOBJ_TO_PHP_G_LIST(zobj);

    if (data!=NULL) {
        ZVAL_COPY(&self->data, data);
    }

}
/* }}} */

/*----------------------------------------------------------------------+
 | PHP_FUNCTION                                                         |
 +----------------------------------------------------------------------*/
/* {{{ proto php_GList g_list_append(php_GList list, mixed data)
   Adds a new element on to the end of the list. */
PHP_FUNCTION(g_list_append)
{
    zval *zlist;
    zval *zdata;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS_EX(zlist, php_g_list_class_entry, 1, 0);
        Z_PARAM_ZVAL(zdata);
    ZEND_PARSE_PARAMETERS_END();

    php_g_list *php_list = ZVAL_IS_PHP_G_LIST(zlist)? ZVAL_GET_PHP_G_LIST(zlist): NULL;

    php_g_list *php_ret = php_g_list_append(php_list, zdata);
    zend_object *z_ret = &php_ret->std;

    RETURN_OBJ(z_ret);
}/* }}} */
/* {{{ proto php_GList g_list_prepend(php_GList list, mixed data)
   Prepends a new element on to the start of the list. */
PHP_FUNCTION(g_list_prepend)
{
    zval *zlist;
    zval *zdata;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS_EX(zlist, php_g_list_class_entry, 1, 0);
        Z_PARAM_ZVAL(zdata);
    ZEND_PARSE_PARAMETERS_END();

    php_g_list *php_list = ZVAL_IS_PHP_G_LIST(zlist)? ZVAL_GET_PHP_G_LIST(zlist): NULL;

    php_g_list *php_ret = php_g_list_prepend(php_list, zdata);
    zend_object *z_ret = &php_ret->std;

    RETURN_OBJ(z_ret);
}/* }}} */
PHP_FUNCTION(g_list_insert)
{
    zval *zlist;
    zval *zdata;
    zend_long position;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_OBJECT_OF_CLASS_EX(zlist, php_g_list_class_entry, 1, 0);
        Z_PARAM_ZVAL(zdata);
        Z_PARAM_LONG(position);
    ZEND_PARSE_PARAMETERS_END();

    php_g_list *php_list = ZVAL_IS_PHP_G_LIST(zlist)? ZVAL_GET_PHP_G_LIST(zlist): NULL;

    php_g_list *php_ret = php_g_list_insert(php_list, zdata, position);

    RETURN_OBJ(&php_ret->std);
}/* }}} */
/* {{{ proto php_GList g_list_insert_before(php_GList list, php_GList sibling, mixed data)
   Inserts a new element into the list before the given position. */
PHP_FUNCTION(g_list_insert_before)
{
    zval *zlist;
    zval *zsibling;
    zval *zdata;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_OBJECT_OF_CLASS_EX(zlist, php_g_list_class_entry, 1, 0);
        Z_PARAM_OBJECT_OF_CLASS_EX(zsibling, php_g_list_class_entry, 1, 0);
        Z_PARAM_ZVAL(zdata);
    ZEND_PARSE_PARAMETERS_END();

    php_g_list *php_list = ZVAL_IS_PHP_G_LIST(zlist)? ZVAL_GET_PHP_G_LIST(zlist): NULL;
    php_g_list *php_sibling = ZVAL_IS_PHP_G_LIST(zsibling)? ZVAL_GET_PHP_G_LIST(zsibling): NULL;

    php_g_list *php_ret = php_g_list_insert_before(php_list, php_sibling, zdata);
    zend_object *z_ret = &php_ret->std;

    RETURN_OBJ(z_ret);
}/* }}} */

PHP_FUNCTION(g_list_insert_sorted) /* {{{ */
{
    zval *z_list = NULL;
    zval *z_data;
    zval *z_func;


    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_OBJECT_OF_CLASS_EX(z_list, php_g_list_class_entry, ZEND_SEND_NOTNULL, ZEND_SEND_BY_VAL)
        Z_PARAM_ZVAL(z_data)
        Z_PARAM_ZVAL(z_func)
    ZEND_PARSE_PARAMETERS_END();

    php_g_list *p_list = ZVAL_IS_PHP_G_LIST(z_list)? ZVAL_GET_PHP_G_LIST(z_list): NULL;

    php_g_list *ret_php_list = php_g_list_insert_sorted(p_list, z_data, z_func);

    /*
    GList *list = p_list ? PHP_G_LIST_GET_PTR(p_list) : NULL;
    GList *ret_value = g_list_insert_sorted(list, data);

    zend_object *ret_z_value = php_g_list_create_object(php_g_list_class_entry);
    php_g_list *ret_p_value = ZOBJ_TO_PHP_G_LIST(ret_z_value);
    ret_p_value->ptr = ret_value;
    RETURN_OBJ(ret_z_value);
    */
    RETURN_OBJ(&ret_php_list->std);

}/* }}} */

/* {{{ proto php_GList g_list_remove(php_GList list, gconstpointer data)
   Removes an element from a GList. */
PHP_FUNCTION(g_list_remove)
{
    zval *zlist;
    zval *zdata;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS_EX(zlist, php_g_list_class_entry, 0, 0);
        Z_PARAM_ZVAL(zdata);
    ZEND_PARSE_PARAMETERS_END();

    php_g_list *php_list = ZVAL_IS_PHP_G_LIST(zlist)? ZVAL_GET_PHP_G_LIST(zlist): NULL;

    php_g_list *php_ret = php_g_list_remove(php_list, zdata);

    if (php_ret) {
        GC_ADDREF(&php_ret->std);
        RETURN_OBJ(&php_ret->std);
    } else {
        RETURN_NULL();
    }
}/* }}} */
/* {{{ proto php_GList g_list_remove_link(php_GList list, php_GList llink)
   Removes an element from a GList, without freeing the element. */
PHP_FUNCTION(g_list_remove_link)
{
    zval *zlist;
    zval *zllink;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS_EX(zlist, php_g_list_class_entry, 0, 0);
        Z_PARAM_OBJECT_OF_CLASS_EX(zllink, php_g_list_class_entry, 0, 0);
    ZEND_PARSE_PARAMETERS_END();

    php_g_list *php_list = ZVAL_IS_PHP_G_LIST(zlist)? ZVAL_GET_PHP_G_LIST(zlist): NULL;
    php_g_list *php_llink = ZVAL_IS_PHP_G_LIST(zllink)? ZVAL_GET_PHP_G_LIST(zllink): NULL;

    php_g_list *php_ret = php_g_list_remove_link(php_list, php_llink);

    if (php_ret) {
        GC_ADDREF(&php_ret->std);
        RETURN_OBJ(&php_ret->std);
    } else {
        RETURN_NULL();
    }
}/* }}} */
/* {{{ proto php_GList g_list_delete_link(php_GList list, php_GList link_)
   Removes the node link_ from the list and frees it. */
PHP_FUNCTION(g_list_delete_link)
{
    zval *zlist;
    zval *zlink_;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS_EX(zlist, php_g_list_class_entry, 0, 0);
        Z_PARAM_OBJECT_OF_CLASS_EX(zlink_, php_g_list_class_entry, 0, 0);
    ZEND_PARSE_PARAMETERS_END();

    php_g_list *php_list = ZVAL_IS_PHP_G_LIST(zlist)? ZVAL_GET_PHP_G_LIST(zlist): NULL;
    php_g_list *php_link_ = ZVAL_IS_PHP_G_LIST(zlink_)? ZVAL_GET_PHP_G_LIST(zlink_): NULL;

    php_g_list *php_ret = php_g_list_delete_link(php_list, php_link_);

    if (php_ret) {
        GC_ADDREF(&php_ret->std);
        RETURN_OBJ(&php_ret->std);
    } else {
        RETURN_NULL();
    }
}/* }}} */

/* {{{ proto GList g_list_remove_all(GList list, mixed data) */
PHP_FUNCTION(g_list_remove_all)
{
    zval *list = NULL;
    zval *data = NULL;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(list)
        Z_PARAM_ZVAL(data)
    ZEND_PARSE_PARAMETERS_END();

    php_g_list *php_list = ZVAL_IS_PHP_G_LIST(list)? ZVAL_GET_PHP_G_LIST(list): NULL;
    php_g_list *php_ret = php_g_list_remove_all(php_list, data);

    if (php_ret) {
        GC_ADDREF(&php_ret->std);
        RETURN_OBJ(&php_ret->std);
    } else {
        RETURN_NULL();
    }
}/* }}} */
/* {{{ proto void g_list_free(php_GList list)
   Frees all of the memory used by a GList. */
PHP_FUNCTION(g_list_free)
{
    zval *zlist;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        //Z_PARAM_OBJECT_OF_CLASS_EX(zlist, php_g_list_class_entry, 0, 0);
        Z_PARAM_OBJECT_OF_CLASS_EX2(zlist, php_g_list_class_entry, 1, 1, 0);
    ZEND_PARSE_PARAMETERS_END();

    php_g_list *php_list = ZVAL_IS_PHP_G_LIST(zlist)? ZVAL_GET_PHP_G_LIST(zlist): NULL;

    php_g_list_free(php_list);
    ZVAL_NULL(zlist);

}/* }}} */
/* {{{ proto int g_list_length(php_GList list)
   Gets the number of elements in a GList. */
PHP_FUNCTION(g_list_length)
{
    zval *zlist;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS_EX(zlist, php_g_list_class_entry, 0, 0);
    ZEND_PARSE_PARAMETERS_END();

    php_g_list *php_list = ZVAL_IS_PHP_G_LIST(zlist)? ZVAL_GET_PHP_G_LIST(zlist): NULL;

    int ret = php_g_list_length(php_list);

    RETURN_LONG(ret);
}/* }}} */

/* {{{ proto php_GList g_list_first(php_GList list)
   Gets the first element in a GList. */
PHP_FUNCTION(g_list_first)
{
    zval *zlist;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS_EX(zlist, php_g_list_class_entry, 0, 0);
    ZEND_PARSE_PARAMETERS_END();

    php_g_list *php_list = ZVAL_IS_PHP_G_LIST(zlist)? ZVAL_GET_PHP_G_LIST(zlist): NULL;

    php_g_list *php_ret = php_g_list_first(php_list);
    zend_object *z_ret = &php_ret->std;
    zend_gc_addref(&z_ret->gc);

    RETURN_OBJ(z_ret);
}/* }}} */
/* {{{ proto php_GList g_list_last(php_GList list)
   Gets the last element in a GList. */
PHP_FUNCTION(g_list_last)
{
    zval *zlist;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS_EX(zlist, php_g_list_class_entry, 0, 0);
    ZEND_PARSE_PARAMETERS_END();

    php_g_list *php_list = ZVAL_IS_PHP_G_LIST(zlist)? ZVAL_GET_PHP_G_LIST(zlist): NULL;

    php_g_list *php_ret = php_g_list_last(php_list);
    zend_object *z_ret = &php_ret->std;

    RETURN_OBJ(z_ret);
}/* }}} */
/* {{{ proto php_GList g_list_nth(php_GList list, int n)
   Gets the element at the given position in a GList. */
PHP_FUNCTION(g_list_nth)
{
    zval *zlist;
    zend_long n;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS_EX(zlist, php_g_list_class_entry, 0, 0);
        Z_PARAM_LONG(n);
    ZEND_PARSE_PARAMETERS_END();

    php_g_list *php_list = ZVAL_IS_PHP_G_LIST(zlist)? ZVAL_GET_PHP_G_LIST(zlist): NULL;

    php_g_list *php_ret = php_g_list_nth(php_list, n);
    zend_object *z_ret = &php_ret->std;

    RETURN_OBJ(z_ret);
}/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
