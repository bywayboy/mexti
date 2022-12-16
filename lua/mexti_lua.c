#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_var.h"

#include "ext/standard/basic_functions.h"
#include "ext/standard/php_incomplete_class.h"
#include "ext/standard/info.h"

#include "zend_exceptions.h"
#include <zend_interfaces.h>

#include "mexti_lua.h"

#include "../lib/lua/src/lua.h"
#include "../lib/lua/src/lualib.h"
#include "../lib/lua/src/lauxlib.h"

#include <stddef.h>

PHPAPI zend_class_entry     * mexti_ce_Lua;
       zend_object_handlers   lua_ce_handlers;


#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof(type, member)))

typedef struct mexti_lua_t
{
    /* data */
    lua_State * L;
    zend_object std;
}mexti_lua_t;

static inline mexti_lua_t *mexti_lua_from_obj(zend_object * obj) /* {{{ */ {
	return (mexti_lua_t*)((char*)(obj) - XtOffsetOf(mexti_lua_t, std));
}
#define Z_LUA_P(zv)  mexti_lua_from_obj(Z_OBJ_P((zv)))

#define luaL_dobuffer(L, code, size, name) \
	(luaL_loadbuffer(L, buffer,size, name) || lua_pcall(L, 0, LUA_MULTRET, 0))



PHP_METHOD(Lua, __construct)
{
    char * buffer = NULL;
    size_t size;
    int base, status;
    mexti_lua_t * obj = Z_LUA_P(ZEND_THIS);
    lua_State * L;
    
    ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(buffer, size);
	ZEND_PARSE_PARAMETERS_END();

    obj->L = L = luaL_newstate();
    luaL_openlibs(L);
    base= lua_gettop(L);

    if (LUA_OK != (status = luaL_dobuffer(L, buffer, size, "main"))) {
        size_t msglen = 0;
		const char* msg = lua_tolstring(L, -1, &msglen);
        char * out = emalloc(msglen + 106);
        zend_sprintf(out, "\n================== LUA ERROR ==================\n%s\n===============================================\n", msg);
        zend_throw_exception(NULL, out, 1002);
        efree(out);
        lua_pop(L, 1);  /* remove message */
	}
}

static int lua_pushzval(lua_State * L, zval* val);
static int lua_pushtable_object(lua_State * L, zval * arr);
static int lua_pushtable_array(lua_State * L, zval * arr);


static int lua_pushtable_array(lua_State * L, zval * arr)
{
    zend_ulong num;
	zend_string *key;
	zval *val;

    HashTable * ht = Z_ARRVAL_P(arr);
    
    lua_createtable(L, 3,3);

    //uint32 count = zend_hash_num_elements(ht);
    ZEND_HASH_FOREACH_KEY_VAL(ht, num, key, val) {
        if (key) {
			lua_pushlstring(L, ZSTR_VAL(key), ZSTR_LEN(key));
		} else {
			lua_pushinteger(L, num);
		}
        lua_pushzval(L, val);
        lua_rawset(L, -3);
    }
    ZEND_HASH_FOREACH_END();
    
    return 1;
}

static int lua_pushtable_object(lua_State * L, zval * arr)
{
    HashTable * ht = zend_get_properties_for(arr, ZEND_PROP_PURPOSE_DEBUG);
    return 1;
}


static int lua_pushzval(lua_State * L, zval* val)
{
    switch(Z_TYPE_P(val)){
    case IS_TRUE:
        lua_pushboolean(L, 1);
        break;
    case IS_FALSE:
        lua_pushboolean(L, 0);
        break;
    case IS_LONG:
        lua_pushinteger(L, Z_LVAL_P(val));
        break;
    case IS_DOUBLE:
        lua_pushinteger(L, Z_DVAL_P(val));
        break;
    case IS_NULL:
        lua_pushnil(L);
        break;
    case IS_STRING:
        lua_pushlstring(L, Z_STRVAL_P(val),Z_STRLEN_P(val));
        break;
    case IS_ARRAY:
        lua_pushtable_array(L, val);
        break;
    case IS_OBJECT:
        lua_pushtable_object(L, val);
        break;
    default:
        lua_pushnil(L);
        break;
    }
    return 1;
}

static void bind_lua_val(zval* return_value, lua_State * L, int i)
{
    size_t l;
    const char * s;
    zend_array * sub_array;
    int t = lua_type(L, i), c, j;
    switch(t){
    case LUA_TNUMBER:
        lua_isinteger(L, i) ? RETURN_LONG(lua_tointeger(L, i)):RETURN_DOUBLE(lua_tonumber(L, i));
        break;
    case LUA_TBOOLEAN:
        RETURN_BOOL(lua_toboolean(L, i));
        break;
    case LUA_TSTRING:
        s = lua_tolstring(L,i, &l);
        RETURN_STRINGL(s, l);
        break;
    case LUA_TTABLE:
        array_init(return_value);
        c = lua_rawlen(L, i);
        if(c > 0){
            for(j = 0; j < c; j++){
                lua_rawgeti(L, j);
                switch(lua_type(L, -1)){
                case LUA_TNUMBER:
                    lua_isinteger(L, i) ? add_index_long(return_value, j, lua_tointeger(L, -1)) : add_index_double(return_value, j, lua_tonumber(L, -1));
                    break;
                case LUA_TBOOLEAN:
                    add_index_bool(return_value, j, lua_toboolean(L, -1));
                    break;
                case LUA_TTABLE:
                    array_init(&sub_array);
                    bind_lua_val(&sub_array, lua_absindex(L, -1));
                    add_index_array(return_value, j, &sub_array);
                }
                lua_pop(L, 1);
            }
        }
    }
}

PHP_METHOD(Lua, call)
{
    char * func = NULL;
    size_t size_func;
    zval * params;
    uint32_t argc, i;
    zval *args = NULL;
    mexti_lua_t * obj = Z_LUA_P(ZEND_THIS);
    lua_State * L = obj->L;
    int base = lua_gettop(L);

    ZEND_PARSE_PARAMETERS_START(1, -1)
        Z_PARAM_STRING(func, size_func);
        #if PHP_VERSION_ID >= 80000
		    //Z_PARAM_VARIADIC_WITH_NAMED(params, param_count, named_params)
            Z_PARAM_VARIADIC('*', args, argc)
        #else
            //TODO Throw Eror.
            zend_error("xzend error.")
        #endif
    ZEND_PARSE_PARAMETERS_END();

    lua_pushlstring(L, func, size_func);
    if(LUA_TFUNCTION == lua_getglobal(L, func)){
        zend_printf("arg count = %d\n", argc);
        if(NULL != args){
            for(i=0;i< argc; i++){
                lua_pushzval(L, &args[i]);
            }
        }
        if(LUA_OK == lua_pcall(L, argc, LUA_MULTRET,0)){
            int retc = lua_gettop(L) - base;
            if(retc == 1){
                bind_lua_val(return_value, L, -1);
                return;
            }
        }
    }
    lua_settop(L, base);
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Lua___construct, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, code, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_class_Lua_call, 0)
    ZEND_ARG_TYPE_INFO(0, func, IS_STRING, 0)
    ZEND_ARG_VARIADIC_INFO(0, parameters)
ZEND_END_ARG_INFO()


static const zend_function_entry class_Lua_methods[] = {
    ZEND_ME(Lua, __construct, arginfo_class_Lua___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    ZEND_ME(Lua, call, arginfo_class_Lua_call,   ZEND_ACC_PUBLIC)
    ZEND_FE_END
};




// 释放对象
static void mexti_lua_free_object(zend_object *object)
{
    mexti_lua_t * obj = mexti_lua_from_obj(object);
    unsigned int i;

    // 释放所有标准属性.
    zend_object_std_dtor(&obj->std);

    lua_close(obj->L);
    //zend_printf("\\mexti\\MinHeap::free\n");
}

// 创建对象
static zend_object * mexti_lua_object_new(zend_class_entry *class_type)
{
    mexti_lua_t * obj = zend_object_alloc(sizeof(mexti_lua_t), class_type);
    zend_object_std_init(&obj->std, class_type);
	object_properties_init(&obj->std, class_type);

    obj->std.handlers = &lua_ce_handlers;
    obj->L = luaL_newstate();
    luaL_openlibs(obj->L);
    return &obj->std;
}


zend_class_entry * register_class_Lua()
{
	 zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "mexti\\Lua", class_Lua_methods);
	//class_entry = zend_register_internal_interface(&ce);
    class_entry = zend_register_internal_class_ex(&ce, NULL);
	//class_entry->ce_flags |= ZEND_ACC_ABSTRACT;
	//zend_class_implements(class_entry, 2, class_entry_Iterator, class_entry_Countable);
    class_entry->create_object          = mexti_lua_object_new;

    memcpy(&lua_ce_handlers, zend_get_std_object_handlers(), sizeof(lua_ce_handlers));

    lua_ce_handlers.offset          = XtOffsetOf(mexti_lua_t, std);
    //minheap_ce_handlers.dtor_obj        = mexti_minheap_dtor_object;        /* This is the destroy handler */
    lua_ce_handlers.free_obj        = mexti_lua_free_object;        /* This is the free handler */
    

	return class_entry;
}