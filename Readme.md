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
## BENCHMARK (benchmark.lua)
```

          PING        1/10   3.65s total,   27397.26 ops/sec
          PING       50/10   0.52s total,  192307.69 ops/sec
          PING      200/10   0.33s total,  303030.30 ops/sec
          PING    20000/10   0.22s total,  454545.45 ops/sec
          PING    50000/10   0.23s total,  434782.61 ops/sec
          PING   100000/10   0.20s total,  500000.00 ops/sec
 SET small str        1/10   3.67s total,   27247.96 ops/sec
 SET small str       50/10   0.57s total,  175438.60 ops/sec
 SET small str      200/10   0.36s total,  277777.78 ops/sec
 SET small str    20000/10   0.26s total,  384615.38 ops/sec
 SET small str    50000/10   0.25s total,  400000.00 ops/sec
 SET small str   100000/10   0.23s total,  434782.61 ops/sec
 GET small str        1/10   3.91s total,   25575.45 ops/sec
 GET small str       50/10   0.55s total,  181818.18 ops/sec
 GET small str      200/10   0.35s total,  285714.29 ops/sec
 GET small str    20000/10   0.25s total,  400000.00 ops/sec
 GET small str    50000/10   0.24s total,  416666.67 ops/sec
 GET small str   100000/10   0.22s total,  454545.45 ops/sec
 SET large str        1/10   4.22s total,   23696.68 ops/sec
 SET large str       50/10   0.73s total,  136986.30 ops/sec
 SET large str      200/10   0.53s total,  188679.25 ops/sec
 SET large str    20000/10   0.57s total,  175438.60 ops/sec
 SET large str    50000/10   0.90s total,  111111.11 ops/sec
 SET large str   100000/10   1.30s total,   76923.08 ops/sec
 GET large str        1/10   4.68s total,   21367.52 ops/sec
 GET large str       50/10   1.46s total,   68493.15 ops/sec
 GET large str      200/10   1.52s total,   65789.47 ops/sec
 GET large str    20000/10   1.43s total,   69930.07 ops/sec
 GET large str    50000/10   1.09s total,   91743.12 ops/sec
 GET large str   100000/10   0.66s total,  151515.15 ops/sec
          INCR        1/10   3.67s total,   27247.96 ops/sec
          INCR       50/10   0.54s total,  185185.19 ops/sec
          INCR      200/10   0.34s total,  294117.65 ops/sec
          INCR    20000/10   0.25s total,  400000.00 ops/sec
          INCR    50000/10   0.22s total,  454545.45 ops/sec
          INCR   100000/10   0.23s total,  434782.61 ops/sec
         LPUSH        1/10   3.78s total,   26455.03 ops/sec
         LPUSH       50/10   0.56s total,  178571.43 ops/sec
         LPUSH      200/10   0.35s total,  285714.29 ops/sec
         LPUSH    20000/10   0.25s total,  400000.00 ops/sec
         LPUSH    50000/10   0.24s total,  416666.67 ops/sec
         LPUSH   100000/10   0.23s total,  434782.61 ops/sec
     LRANGE 10        1/10   4.10s total,   24390.24 ops/sec
     LRANGE 10       50/10   0.78s total,  128205.13 ops/sec
     LRANGE 10      200/10   0.57s total,  175438.60 ops/sec
     LRANGE 10    20000/10   0.52s total,  192307.69 ops/sec
     LRANGE 10    50000/10   0.47s total,  212765.96 ops/sec
     LRANGE 10   100000/10   0.42s total,  238095.24 ops/sec
    LRANGE 100        1/10   6.87s total,   14556.04 ops/sec
    LRANGE 100       50/10   2.69s total,   37174.72 ops/sec
    LRANGE 100      200/10   2.59s total,   38610.04 ops/sec
    LRANGE 100    20000/10   2.67s total,   37453.18 ops/sec
    LRANGE 100    50000/10   2.36s total,   42372.88 ops/sec
    LRANGE 100   100000/10   1.98s total,   50505.05 ops/sec
    
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

(The MIT License)

Copyright (c) 2012 Tadeusz Wójcik &lt;tadeuszwojcik@gmail.com&gt;

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


