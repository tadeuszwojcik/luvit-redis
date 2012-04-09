Hiredis bindings for Luvit
=====
STATUS: WORK IN PROGRESS

Example:

```lua
local redisClient = require('redis')

local client = redisClient:new()

client:sadd("myset","first","second","third", function()

  client:smembers("myset",function(err,res)
    for k,v in pairs(res) do print(k,v) end
  end)
end)
```

## TODO
* redis lua scripting api
* cleanups in redis.c
* review error handling
* callbacks for connect disconnect
* unit tests
* automatic reconnect on disconnect