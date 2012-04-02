#include <lua.h>
#include <lauxlib.h>
#include "uv.h"
#include "hiredis.h"
#include "async.h"
#include "libuv.h"
#include "utils.h"

#define LUVIT_HIREDIS_CONNECTION_MT "luvit-hiredis.connection"
#define LUAHIREDIS_MAXARGS (256)
#define LUAHIREDIS_KEY_NIL "NIL"

typedef struct luvitredis_Connection
{
  redisAsyncContext * redisContext;
} luvitredis_Connection;

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

void getCallback(redisAsyncContext *c, void *r, void *privdata) {
    redisReply *reply = r;
    luv_ref_t *ff =privdata;
    int i;

    if(ff==NULL)
    {
      return;
    }

    lua_rawgeti(ff->L, LUA_REGISTRYINDEX, ff->r);
    i=push_reply(ff->L,reply);

    luv_acall(ff->L, i, 0, "hiredis_after");
}
static redisAsyncContext * check_connection(lua_State * L, int idx)
{
  luvitredis_Connection * pConn = (luvitredis_Connection *)luaL_checkudata(
      L, idx, LUVIT_HIREDIS_CONNECTION_MT
    );
  if (pConn == NULL)
  {
    luaL_error(L, "lua-hiredis error: connection is null");
    return NULL; /* Unreachable */
  }

  if (pConn->redisContext == NULL)
  {
    luaL_error(
        L, "lua-hiredis error: attempted to use closed connection"
      );
    return NULL; /* Unreachable */
  }

  return pConn->redisContext;
}
static int load_args(
    lua_State * L,
    redisAsyncContext * redisContext,
    int idx, /* index of first argument */
    const char ** argv,
    size_t * argvlen
  )
{
  int nargs = lua_gettop(L) - idx + 1;
  int i = 0;

  if (nargs <= 0)
  {
    return luaL_error(L, "missing command name");
  }

  if (nargs > LUAHIREDIS_MAXARGS)
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

static int lconn_command(lua_State * L)
{
  redisAsyncContext * redisContext = check_connection(L, 1);

  const char * argv[LUAHIREDIS_MAXARGS];
  size_t argvlen[LUAHIREDIS_MAXARGS];

  luv_ref_t* ref=NULL;
  if(lua_isfunction(L,-1))
  {
    ref = (luv_ref_t*)malloc(sizeof(luv_ref_t));
    ref->L = L;
    lua_pushvalue(L, -1); /* duplicate so we can _ref it */
    ref->r = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pop(L,1);
  }

  int nargs = load_args(L, redisContext, 2, argv, argvlen);

  redisAsyncCommandArgv(redisContext,getCallback,ref,nargs,argv,argvlen);

  return 0;
}


static int luv_hiredis_connect(lua_State * L)
{
  luvitredis_Connection * luvitConnection = NULL;
  redisAsyncContext * redisContext = NULL;

  const char * host = luaL_checkstring(L, 1);
  int port = luaL_checkint(L, 2);

  redisContext = redisConnect(host, port);
  if (!redisContext)
  {
    lua_pushnil(L);
    lua_pushliteral(L, "failed to create hiredis context");

    return 2;
  }

  if (redisContext->err)
  {
    int result = push_error(L, redisContext);

    redisFree(redisContext);
    redisContext = NULL;

    return result;
  }

  redisLibuvAttach(luv_get_loop(L), redisContext);

  luvitConnection = (luvitredis_Connection *)lua_newuserdata(
      L, sizeof(luvitredis_Connection)
    );

  luvitConnection->redisContext = redisContext;

  luaL_getmetatable(L, LUVIT_HIREDIS_CONNECTION_MT);
  lua_setmetatable(L, -2);

  return 1;
}

static const luaL_reg connectionMethods []=
{
  {"command",lconn_command},
  {NULL, NULL}
};

static const luaL_reg exports[] =
{
  { "connect", luv_hiredis_connect },
  { NULL, NULL }
};

LUALIB_API int luaopen_hiredis(lua_State * L)
{
  luaL_newmetatable(L, LUVIT_HIREDIS_CONNECTION_MT);
  lua_pushvalue(L,-1);
  lua_setfield(L, -2, "__index");
  luaL_register(L,NULL,connectionMethods);


  lua_newtable(L);
  luaL_register(L,NULL,exports);

  return 1;
}