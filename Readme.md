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
```
          PING        1/10   3.70s total,   27027.03 ops/sec
          PING       50/10   0.51s total,  196078.43 ops/sec
          PING      200/10   0.31s total,  322580.65 ops/sec
          PING    20000/10   0.22s total,  454545.45 ops/sec
          PING    50000/10   0.23s total,  434782.61 ops/sec
          PING   100000/10   0.20s total,  500000.00 ops/sec
 SET small str        1/10   4.02s total,   24875.62 ops/sec
 SET small str       50/10   0.57s total,  175438.60 ops/sec
 SET small str      200/10   0.34s total,  294117.65 ops/sec
 SET small str    20000/10   0.27s total,  370370.37 ops/sec
 SET small str    50000/10   0.24s total,  416666.67 ops/sec
 SET small str   100000/10   0.24s total,  416666.67 ops/sec
 GET small str        1/10   3.76s total,   26595.74 ops/sec
 GET small str       50/10   0.54s total,  185185.19 ops/sec
 GET small str      200/10   0.33s total,  303030.30 ops/sec
 GET small str    20000/10   0.25s total,  400000.00 ops/sec
 GET small str    50000/10   0.23s total,  434782.61 ops/sec
 GET small str   100000/10   0.22s total,  454545.45 ops/sec
 SET large str        1/10   4.24s total,   23584.91 ops/sec
 SET large str       50/10   0.73s total,  136986.30 ops/sec
 SET large str      200/10   0.52s total,  192307.69 ops/sec
 SET large str    20000/10   0.58s total,  172413.79 ops/sec
 SET large str    50000/10   0.94s total,  106382.98 ops/sec
 SET large str   100000/10   1.26s total,   79365.08 ops/sec
 GET large str        1/10   4.38s total,   22831.05 ops/sec
 GET large str       50/10   1.35s total,   74074.07 ops/sec
 GET large str      200/10   1.32s total,   75757.58 ops/sec
 GET large str    20000/10   1.45s total,   68965.52 ops/sec
 GET large str    50000/10   1.01s total,   99009.90 ops/sec
 GET large str   100000/10   0.68s total,  147058.82 ops/sec
          INCR        1/10   3.73s total,   26809.65 ops/sec
          INCR       50/10   0.53s total,  188679.25 ops/sec
          INCR      200/10   0.34s total,  294117.65 ops/sec
          INCR    20000/10   0.24s total,  416666.67 ops/sec
          INCR    50000/10   0.23s total,  434782.61 ops/sec
          INCR   100000/10   0.21s total,  476190.48 ops/sec
         LPUSH        1/10   3.74s total,   26737.97 ops/sec
         LPUSH       50/10   0.54s total,  185185.19 ops/sec
         LPUSH      200/10   0.34s total,  294117.65 ops/sec
         LPUSH    20000/10   0.25s total,  400000.00 ops/sec
         LPUSH    50000/10   0.25s total,  400000.00 ops/sec
         LPUSH   100000/10   0.24s total,  416666.67 ops/sec
     LRANGE 10        1/10   4.19s total,   23866.35 ops/sec
     LRANGE 10       50/10   0.76s total,  131578.95 ops/sec
     LRANGE 10      200/10   0.54s total,  185185.19 ops/sec
     LRANGE 10    20000/10   0.49s total,  204081.63 ops/sec
     LRANGE 10    50000/10   0.47s total,  212765.96 ops/sec
     LRANGE 10   100000/10   0.41s total,  243902.44 ops/sec
    LRANGE 100        1/10   6.83s total,   14641.29 ops/sec
    LRANGE 100       50/10   2.55s total,   39215.69 ops/sec
    LRANGE 100      200/10   2.64s total,   37878.79 ops/sec
    LRANGE 100    20000/10   2.63s total,   38022.81 ops/sec
    LRANGE 100    50000/10   2.28s total,   43859.65 ops/sec
    LRANGE 100   100000/10   2.08s total,   48076.92 ops/sec
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


