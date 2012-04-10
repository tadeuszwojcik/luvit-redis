Hiredis bindings for Luvit
=====
STATUS: WORK IN PROGRESS

Example:

```lua
local redis = require('redis')

local client = redis:new()

client:sadd("myset","first","second","third", function()

  client:smembers("myset",function(err,res)
    for k,v in pairs(res) do print(k,v) end
  end)

end)

client:info(function(err,res)
  print(res)
end)
```

## TODO
* cleanups in redis.c
* review error handling
* callbacks for connect disconnect (client:on("disconnect"...),client:on("connect" ...))
* callback for error (client:on("error" ...))
* unit tests
* client auth
* docs
* more examples
* benchmarks
* automatic reconnect on disconnect (queuing commands?)

License
-------

[MIT](luvit-redis/LICENSE)
