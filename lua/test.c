#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <math.h>

static int l_sin (lua_State *L) {
  double d = lua_tonumber(L, 1);  /* get argument */
  lua_pushnumber(L, sin(d));  /* push result */
  return 1;  /* number of results */
}

static int l_cos (lua_State *L) {
  double d = lua_tonumber(L, 1);  /* get argument */
  lua_pushnumber(L, cos(d));  /* push result */
  return 1;  /* number of results */
}

int main (void) {
  char buff[256];
  int error;

  lua_State *L = luaL_newstate(); /* Create new Lua state */
  if(L == NULL) {
    printf("luaL_newstate == NULL!\n");
    return(1);
  }

  luaL_openlibs(L);
  lua_pushcfunction(L, l_sin);
  lua_setglobal(L, "sin");

  lua_pushcfunction(L, l_cos);
  lua_setglobal(L, "cos");

  error = luaL_loadfile(L, "test.cfg") || lua_pcall(L, 0, 0, 0);
  if (error) {
    fprintf(stderr, "%s", lua_tostring(L, -1));
    lua_pop(L, 1);  /* pop error message from the stack */
  }

  // ***********************************************************

  // Call lua init function if it exists
  error = lua_getglobal (L, "init");
  if(error) {
  	if (lua_isfunction (L, 0) == 1) {
    if (lua_pcall (L, 0, 1, 0) == 0) {
      printf("Nu har vi kört!\n");
    } else {
      fprintf(stderr, "%s", lua_tostring(L, -1));
      lua_pop(L, 1);  /* pop error message from the stack */
    }
  	}
  } else {
    printf("Kan inte ladda test.cfg!\n");
    fprintf(stderr, "%s", lua_tostring(L, -1));
    lua_pop(L, 1);  /* pop error message from the stack */
  }

  lua_pop(L, 1); // Pop value pushed by lua_getglobal
  
  //lua_pushnumber(L, x);   /* push 1st argument */
  //lua_pushnumber(L, y);   /* push 2nd argument */

  /* do the call (2 arguments, 1 result) */
  //if (lua_pcall(L, 2, 1, 0) != 0)
  //  error(L, "error running function `f': %s",
  //      lua_tostring(L, -1));

  /* retrieve result */
  //if (!lua_isnumber(L, -1))
  //  error(L, "function `f' must return a number");
  //z = lua_tonumber(L, -1);
  //lua_pop(L, 1);  /* pop returned value */


  // ***********************************************************
  lua_close(L);
  return 0;
}
