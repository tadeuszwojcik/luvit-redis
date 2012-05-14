/*
*  Copyright (c) 2012 Tadeusz Wójcik <tadeuszwojcik@gmail.com>
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy of
*  this software and associated documentation files (the "Software"), to deal in
*  the Software without restriction, including without limitation the rights to
*  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
*  the Software, and to permit persons to whom the Software is furnished to do so,
*  subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in all
*  copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
*  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
*  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
*  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
*  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <lua.h>
#include <lauxlib.h>

#include "hiredis.h"
#include "luvit-hiredis-adapter.h"
#include "uv.h"
#include "utils.h"
#include "luv_handle.h"

#define LUA_REDIS_CLIENT_MT "lua.redis.client"

#define LUA_REDIS_MAX_ARGS (LUAI_MAXCSTACK)

typedef struct lua_redis_client_t
{
  redisAsyncContext *redis_async_context;
} lua_redis_client_t;

typedef struct luv_ref_s
{
  lua_State *L;
  int r;
#if USE_REF_FREELIST
  struct luv_ref_s *next;
#endif
} luv_ref_t;

#if USE_REF_FREELIST

/*
 * Reference freelist
 */

static luv_ref_t *ref_freelist = NULL;

static luv_ref_t *ref_alloc()
{
  luv_ref_t *ref;

  ref = ref_freelist;
  if (ref != NULL) {
    ref_freelist = ref->next;
  } else {
    ref = (luv_ref_t *)malloc(sizeof *ref);
  }

  return ref;
}

static void ref_free(luv_ref_t *ref)
{
  ref->next = ref_freelist;
  ref_freelist = ref;
}

#else

#define ref_alloc() ((luv_ref_t *)malloc(sizeof(luv_ref_t)))
#define ref_free(ref) free((void *)(ref))

#endif

static void luv_push_async_hiredis_error(lua_State *L,
                                         const redisAsyncContext *context,
                                         const char* source)
{
  luv_push_async_error_raw(L, NULL, context->errstr, source, NULL);
}

static int push_reply(lua_State *L, redisReply *redisReply)
{
  switch(redisReply->type)
  {
    case REDIS_REPLY_ERROR:
      luv_push_async_error_raw(L, NULL, redisReply->str, "push_reply", NULL);
      break;

    case REDIS_REPLY_STATUS:
      lua_pushlstring(L, redisReply->str, redisReply->len);
      break;

    case REDIS_REPLY_INTEGER:
      lua_pushinteger(L, redisReply->integer);
      break;

    case REDIS_REPLY_NIL:
      lua_pushnil(L);
      break;

    case REDIS_REPLY_STRING:
      lua_pushlstring(L, redisReply->str, redisReply->len);
      break;

    case REDIS_REPLY_ARRAY:
    {
      unsigned int i;
      lua_createtable(L, redisReply->elements, 0);

      for (i = 0; i < redisReply->elements; ++i)
      {
        push_reply(L, redisReply->element[i]);
        lua_rawseti(L, -2, i + 1); /* Store sub-reply */
      }

      break;
    }

    default:
      return luaL_error(L, "Unknown reply type: %d", redisReply->type);
  }

  return 1;
}

static void on_redis_response(redisAsyncContext *context, void *reply, void *privdata)
{
  redisReply *redisReply = reply;
  luv_ref_t *ref = privdata;
  int replyArgsCount = 0;

  lua_State *L = ref->L;
  lua_rawgeti(L, LUA_REGISTRYINDEX, ref->r);
  luaL_unref(L, LUA_REGISTRYINDEX, ref->r);
  ref_free(ref);

  if (redisReply == NULL)
  {
    luv_push_async_hiredis_error(L, context, "on_redis_response");
    lua_pushnil(L);
    luv_acall(L, 2, 0, "on_redis_response");
  }
  else
  {
    if (redisReply->type != REDIS_REPLY_ERROR)
    {
      lua_pushnil(L);
      ++replyArgsCount;
    }
    replyArgsCount += push_reply(L, redisReply);
    luv_acall(L, replyArgsCount, 0, "on_redis_response");
  }
}

static int lua_client_command(lua_State *L)
{
  lua_redis_client_t *lua_redis_client = (lua_redis_client_t *)
                                    luaL_checkudata(L, 1, LUA_REDIS_CLIENT_MT);

  static const char *argv[LUA_REDIS_MAX_ARGS];
  static size_t argvlen[LUA_REDIS_MAX_ARGS];

  int nargs, ltop, i;
  luv_ref_t *ref = NULL;
  int commandStatus;
  redisCallbackFn *redisCallback = NULL;

  if (lua_redis_client->redis_async_context == NULL)
  {
    return luaL_error(L, "RedisAsyncContext is null");
  }

  /* consume callback, if any */
  if (lua_isfunction(L, -1))
  {
    ref = ref_alloc();
    ref->L = L;
    ref->r = luaL_ref(L, LUA_REGISTRYINDEX);
    redisCallback = on_redis_response;
  }

  /* get arguments */
  ltop = lua_gettop(L);
  nargs = 0;
  for (i = 2; i <= ltop; ++i) {
    /* unpack tables of arguments */
    if (lua_istable(L, i)) {
      int j;
      for (j = 1; j <= lua_objlen(L, i); ++j) {
        lua_rawgeti(L, i, j);
        argv[nargs] = lua_tolstring(L, -1, &argvlen[nargs]);
        lua_pop(L, 1);
        if (argv[nargs] == NULL) {
          return luaL_argerror(L, i,
              "expected an array table of string or number values"
            );
        }
        if (++nargs >= LUA_REDIS_MAX_ARGS) {
          return luaL_error(L, "too many arguments");
        }
      }
    } else {
      argv[nargs] = lua_tolstring(L, i, &argvlen[nargs]);
      if (argv[nargs] == NULL) {
        return luaL_argerror(L, i, "expected a string or number value");
      }
      if (++nargs >= LUA_REDIS_MAX_ARGS) {
        return luaL_error(L, "too many arguments");
      }
    }
  }
  if (nargs <= 0) {
    return luaL_error(L, "missing command name");
  }

  commandStatus = redisAsyncCommandArgv(lua_redis_client->redis_async_context,
                                        redisCallback,
                                        ref,
                                        nargs,
                                        argv,
                                        argvlen);

  if (commandStatus != REDIS_OK)
  {
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref->r);
    luaL_unref(L, LUA_REGISTRYINDEX, ref->r);
    ref_free(ref);
    luv_push_async_error_raw(L, NULL, "Redis connection problem", "client_command", NULL);
    lua_pushnil(L);
    lua_call(L, 2, 0);
  }

  return 0;
}

static int lua_client_disconnect(lua_State *L)
{
  lua_redis_client_t *lua_redis_client = (lua_redis_client_t *)
                                    luaL_checkudata(L, 1, LUA_REDIS_CLIENT_MT);

  if (lua_redis_client->redis_async_context == NULL)
  {
    return luaL_error(L, "RedisAsyncContext is null");
  }

  redisAsyncDisconnect(lua_redis_client->redis_async_context);

  return 0;
}

static void connectCallback(const redisAsyncContext *redisAsyncContext, int status)
{
  luv_ref_t *ref = redisAsyncContext->data;
  lua_State *L = ref->L;
  lua_rawgeti(L, LUA_REGISTRYINDEX, ref->r);

  if (status != REDIS_OK)
  {
    luv_push_async_hiredis_error(L, redisAsyncContext, "connectCallback");
    luv_emit_event(L, "error", 1);
  }
  else
  {
    luv_emit_event(L, "connect", 0);
  }
}

static int lua_client_on_connect(lua_State *L)
{
  lua_redis_client_t *lua_redis_client = (lua_redis_client_t *)
                                    luaL_checkudata(L, 1, LUA_REDIS_CLIENT_MT);

  if (lua_redis_client->redis_async_context == NULL)
  {
    return luaL_error(L, "RedisAsyncContext is null");
  }

  luv_register_event(L, 1, "connect", 2);
  redisAsyncSetConnectCallback(lua_redis_client->redis_async_context,
                               connectCallback);

  return 0;
}

static void disconnectCallback(const redisAsyncContext *redisAsyncContext, int status)
{
  luv_ref_t *ref = redisAsyncContext->data;
  lua_State *L = ref->L;
  lua_rawgeti(L, LUA_REGISTRYINDEX, ref->r);

  if (status != REDIS_OK)
  {
    luv_push_async_hiredis_error(L, redisAsyncContext, "disconnectCallback");
    luv_emit_event(L, "error", 1);
  }
  else
  {
    luv_emit_event(L, "disconnect", 0);
  }
}

static int lua_client_on_disconnect(lua_State *L)
{
  lua_redis_client_t *lua_redis_client = (lua_redis_client_t *)
                                    luaL_checkudata(L, 1, LUA_REDIS_CLIENT_MT);

  if (lua_redis_client->redis_async_context == NULL)
  {
    return luaL_error(L, "RedisAsyncContext is null");
  }

  luv_register_event(L, 1, "disconnect", 2);
  redisAsyncSetDisconnectCallback(lua_redis_client->redis_async_context,
                                  disconnectCallback);
  return 0;
}

static int lua_client_on_error(lua_State *L)
{
  lua_redis_client_t *lua_redis_client = (lua_redis_client_t *)
                                    luaL_checkudata(L, 1, LUA_REDIS_CLIENT_MT);

  if (lua_redis_client->redis_async_context == NULL)
  {
    return luaL_error(L, "RedisAsyncContext is null");
  }

  luv_register_event(L, 1, "error", 2);

  return 0;
}

static int lua_create_client(lua_State *L)
{
  lua_redis_client_t *lua_redis_client = NULL;
  redisAsyncContext *redis_async_context = NULL;
  luv_ref_t *ref;
  const char *host = luaL_checkstring(L, 1);
  int port = luaL_checkint(L, 2);

  redis_async_context = redisAsyncConnect(host, port);

  if(redis_async_context->err)
  {
    redisAsyncFree(redis_async_context);
    return luaL_error(L, redis_async_context->errstr);
  }

  redisLibevAttach(EV_DEFAULT_ redis_async_context);

  lua_redis_client = (lua_redis_client_t *)lua_newuserdata(
                                          L, sizeof(lua_redis_client_t));


  lua_redis_client->redis_async_context = redis_async_context;

  luaL_getmetatable(L, LUA_REDIS_CLIENT_MT);
  lua_setmetatable(L, -2);

  lua_newtable(L);
  lua_setfenv(L, -2);

  ref = ref_alloc();
  ref->L = L;
  lua_pushvalue(L, -1);
  ref->r = luaL_ref(L, LUA_REGISTRYINDEX);
  redis_async_context->data = ref;

  return 1;
}

static const luaL_reg redis_client_methods[] =
{
  { "command", lua_client_command },
  { "disconnect", lua_client_disconnect },
  { "onConnect", lua_client_on_connect },
  { "onDisconnect", lua_client_on_disconnect },
  { "onError", lua_client_on_error },
  { NULL, NULL}
};

static const luaL_reg exports[] =
{
  { "createClient", lua_create_client },
  { NULL, NULL }
};

LUALIB_API int luaopen_redis(lua_State *L)
{
  luaL_newmetatable(L, LUA_REDIS_CLIENT_MT);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  luaL_register(L, NULL, redis_client_methods);

  lua_newtable(L);

  lua_pushfstring(L, "%d.%d.%d", HIREDIS_MAJOR, HIREDIS_MINOR, HIREDIS_PATCH);
  lua_setfield(L, -2, "version");

  luaL_register(L, NULL, exports);

  return 1;
}
