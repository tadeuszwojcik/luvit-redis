#include <lua.h>
#include <lauxlib.h>
#include "uv.h"
#include "hiredis.h"
#include "async.h"
#include "libev.h"
#include "utils.h"

#define LUA_REDIS_CLIENT_MT "lua.redis.client"
#define LUA_REDIS_MAX_ARGS (256)
#define LUAHIREDIS_KEY_NIL "NIL"

typedef struct lua_redis_client_t
{
  redisAsyncContext * redis_async_context;
} lua_redis_client_t;

static int push_error(lua_State * L, redisAsyncContext * pContext)
{
  /* TODO: Use errno if err is REDIS_ERR_IO */
  lua_pushnil(L);
  lua_pushstring(
      L,
      (pContext->errstr != NULL)
        ? pContext->errstr
        : "(lua-hiredis: no error message)" /* TODO: ?! */
    );
  lua_pushnumber(L, pContext->err);

  return 3;
}

static int push_reply(lua_State * L, redisReply * pReply)
{
  /* int base = lua_gettop(L); */

  switch(pReply->type)
  {
    case REDIS_REPLY_STATUS:
    case REDIS_REPLY_ERROR:

      lua_pushlstring(L, pReply->str, pReply->len); /* status */

      break;

    case REDIS_REPLY_INTEGER:
      lua_pushinteger(L, pReply->integer);
      break;

    case REDIS_REPLY_NIL:
      lua_pushnil(L);
      break;

    case REDIS_REPLY_STRING:
      lua_pushlstring(L, pReply->str, pReply->len);
      break;

    case REDIS_REPLY_ARRAY:
    {
      unsigned int i = 0;

      lua_createtable(L, pReply->elements, 0);

      for (i = 0; i < pReply->elements; ++i)
      {
        /*
        * Not controlling recursion depth:
        * if we parsed the reply somehow,
        * we hope to be able to push it.
        */

        push_reply(L, pReply->element[i]);
        lua_rawseti(L, -2, i + 1); /* Store sub-reply */
      }
      break;
    }

    default: /* should not happen */
      return luaL_error(L, "command: unknown reply type: %d", pReply->type);
  }

  /*
  if (lua_gettop(L) != base + 1)
  {
    return luaL_error(L, "pushreplystackerror: actual %d expected %d base %d type %d", lua_gettop(L), base + 1, base, pReply->type);
  }
  */

  /*
  * Always returning a single value.
  * If changed, change REDIS_REPLY_ARRAY above.
  */
  return 1;
}

//todo can it be static?
void on_response(redisAsyncContext *c, void *r, void *privdata)
{
    redisReply *reply = r;
    luv_ref_t *ref =privdata;
    int i;

    if(ref == NULL)
    {
      // no callback?
      //memory leak
      return;
    }
    lua_State* L = ref->L;
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref->r);
    i=push_reply(L,reply);
    luaL_unref(L, LUA_REGISTRYINDEX,ref-> r);
    luv_acall(L, i, 0, "redis_on_response");
    free(ref);
}

static int load_args(lua_State * L, int idx, const char ** argv, size_t * argvlen)
{
  int nargs = lua_gettop(L) - idx + 1;
  int i = 0;

  if (nargs <= 0)
  {
    return luaL_error(L, "missing command name");
  }

  if (nargs > LUA_REDIS_MAX_ARGS)
  {
    return luaL_error(L, "too many arguments");
  }

  for (i = 0; i < nargs; ++i)
  {
    size_t len = 0;
    const char * str = lua_tolstring(L, idx + i, &len);

    if (str == NULL)
    {
      return luaL_argerror(L, idx + i, "expected a string or number value");
    }

    argv[i] = str;
    argvlen[i] = len;
  }

  return nargs;
}

static int lua_client_command(lua_State * L)
{
  lua_redis_client_t * lua_redis_client = (lua_redis_client_t *)luaL_checkudata(
                                                    L, 1, LUA_REDIS_CLIENT_MT);
  if (lua_redis_client == NULL)
  {
    luaL_error(L, "lua-redis error: connection is null");

    return 0; /* Unreachable */
  }

  if (lua_redis_client->redis_async_context == NULL)
  {
    luaL_error(L, "lua-redis error: attempted to use closed connection");

    return 0; /* Unreachable */
  }


  const char * argv[LUA_REDIS_MAX_ARGS];
  size_t argvlen[LUA_REDIS_MAX_ARGS];

  luv_ref_t* ref = NULL;

  if(lua_isfunction(L,-1))
  {
    ref = (luv_ref_t*)malloc(sizeof(luv_ref_t));
    ref->L = L;
    lua_pushvalue(L, -1);
    ref->r = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pop(L,1);
  }

  int nargs = load_args(L, 2, argv, argvlen);

  redisAsyncCommandArgv(
    lua_redis_client->redis_async_context,on_response,ref,nargs,argv,argvlen);

  //TODO: should we return something here? maybe only error from redisAsyncCommand?
  return 0;
}


static int lua_create_client(lua_State * L)
{
  lua_redis_client_t * lua_redis_client = NULL;
  redisAsyncContext * redis_async_context = NULL;

  const char * host = luaL_checkstring(L, 1);
  int port = luaL_checkint(L, 2);

  redis_async_context = redisAsyncConnect(host, port);
  if (!redis_async_context)
  {
    lua_pushnil(L);
    lua_pushliteral(L, "failed to create hiredis context");

    return 2;
  }

  if (redis_async_context->err)
  {
    int result = push_error(L, redis_async_context);

    redisAsyncFree(redis_async_context);
    redis_async_context = NULL;

    return result;
  }

  redisLibevAttach(EV_DEFAULT_ redis_async_context);

  lua_redis_client = (lua_redis_client_t *)lua_newuserdata(
                                          L, sizeof(lua_redis_client_t));
  lua_redis_client->redis_async_context = redis_async_context;

  luaL_getmetatable(L, LUA_REDIS_CLIENT_MT);
  lua_setmetatable(L, -2);

  return 1;
}

static const luaL_reg redis_client_methods []=
{
  {"command", lua_client_command},
  {NULL, NULL}
};

static const luaL_reg exports[] =
{
  { "createClient", lua_create_client },
  { NULL, NULL }
};

LUALIB_API int luaopen_redis(lua_State * L)
{
  luaL_newmetatable(L, LUA_REDIS_CLIENT_MT);
  lua_pushvalue(L,-1);
  lua_setfield(L, -2, "__index");
  luaL_register(L, NULL, redis_client_methods);


  lua_newtable(L);
  luaL_register(L, NULL, exports);

  return 1;
}