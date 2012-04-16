# luvit-redis - luvit redis client.


This is a redis client for luvit which under the hood uses offical `hiredis` c library
what makes it pretty fast (see benchmarks below).

## Installation


### from git

    git clone https://github.com/twojcik/luvit-redis
    make

## Usage:


```lua
local redis = require('redis')

local client = redis:new()

client:on("error", function (err)
    print("Error (error callback): ", err)
end)

client:set('test-key','luvit-rocks', redis.print)
client:hset("hash key", "hashtest 1", "some value", redis.print)
client:hset("hash key", "hashtest 2", "some other value", redis.print)
client:hkeys("hash key", function (err, replies)
    print(#replies.." replies:")
    for i,v in ipairs(replies) do print(i,v) end
end)


##### This will display:

```
tadek@ubuntudev:~/Projects/luvit-redis$ luvit examples/example.lua
Reply:  OK
Reply:  0
Reply:  0
2 replies:
1 hashtest 1
2 hashtest 2
```


## BENCHMARKS
#### (comparison of `node-redis` and `luvit-redis`).

Benchmark performed on single box, on `Intel I7 i920`.

#### `luvit-redis` is about 3x to 13x faster than `node-redis`.


### BENCHMARK (benchmark.lua) - `luvit`

x/yy x - pipeline size, yy - num of clients

```
          PING        1/10   3.93s total,   25445.29 ops/sec
          PING       50/10   0.54s total,  185185.19 ops/sec
          PING      200/10   0.33s total,  303030.30 ops/sec
          PING    20000/10   0.24s total,  416666.67 ops/sec
          PING    50000/10   0.22s total,  454545.45 ops/sec
          PING   100000/10   0.20s total,  500000.00 ops/sec
 SET small str        1/10   3.86s total,   25906.74 ops/sec
 SET small str       50/10   0.57s total,  175438.60 ops/sec
 SET small str      200/10   0.36s total,  277777.78 ops/sec
 SET small str    20000/10   0.25s total,  400000.00 ops/sec
 SET small str    50000/10   0.25s total,  400000.00 ops/sec
 SET small str   100000/10   0.23s total,  434782.61 ops/sec
 GET small str        1/10   3.79s total,   26385.22 ops/sec
 GET small str       50/10   0.56s total,  178571.43 ops/sec
 GET small str      200/10   0.36s total,  277777.78 ops/sec
 GET small str    20000/10   0.26s total,  384615.38 ops/sec
 GET small str    50000/10   0.23s total,  434782.61 ops/sec
 GET small str   100000/10   0.22s total,  454545.45 ops/sec
 SET large str        1/10   4.31s total,   23201.86 ops/sec
 SET large str       50/10   0.76s total,  131578.95 ops/sec
 SET large str      200/10   0.53s total,  188679.25 ops/sec
 SET large str    20000/10   0.47s total,  212765.96 ops/sec
 SET large str    50000/10   0.63s total,  158730.16 ops/sec
 SET large str   100000/10   0.91s total,  109890.11 ops/sec
 GET large str        1/10   4.11s total,   24330.90 ops/sec
 GET large str       50/10   1.53s total,   65359.48 ops/sec
 GET large str      200/10   1.40s total,   71428.57 ops/sec
 GET large str    20000/10   1.34s total,   74626.87 ops/sec
 GET large str    50000/10   1.07s total,   93457.94 ops/sec
 GET large str   100000/10   0.69s total,  144927.54 ops/sec
          INCR        1/10   3.51s total,   28490.03 ops/sec
          INCR       50/10   0.54s total,  185185.19 ops/sec
          INCR      200/10   0.33s total,  303030.30 ops/sec
          INCR    20000/10   0.25s total,  400000.00 ops/sec
          INCR    50000/10   0.22s total,  454545.45 ops/sec
          INCR   100000/10   0.22s total,  454545.45 ops/sec
         LPUSH        1/10   3.72s total,   26881.72 ops/sec
         LPUSH       50/10   0.57s total,  175438.60 ops/sec
         LPUSH      200/10   0.34s total,  294117.65 ops/sec
         LPUSH    20000/10   0.26s total,  384615.38 ops/sec
         LPUSH    50000/10   0.25s total,  400000.00 ops/sec
         LPUSH   100000/10   0.22s total,  454545.45 ops/sec
     LRANGE 10        1/10   4.33s total,   23094.69 ops/sec
     LRANGE 10       50/10   0.77s total,  129870.13 ops/sec
     LRANGE 10      200/10   0.54s total,  185185.19 ops/sec
     LRANGE 10    20000/10   0.50s total,  200000.00 ops/sec
     LRANGE 10    50000/10   0.46s total,  217391.30 ops/sec
     LRANGE 10   100000/10   0.44s total,  227272.73 ops/sec
    LRANGE 100        1/10   7.15s total,   13986.01 ops/sec
    LRANGE 100       50/10   2.57s total,   38910.51 ops/sec
    LRANGE 100      200/10   2.59s total,   38610.04 ops/sec
    LRANGE 100    20000/10   2.76s total,   36231.88 ops/sec
    LRANGE 100    50000/10   2.42s total,   41322.31 ops/sec
    LRANGE 100   100000/10   1.99s total,   50251.26 ops/sec
```

### BENCHMARK (multi_bench.js) - `node.js`
x/yy x- pipeline size, yy - num of clients

```
Client count: 10, node version: 0.7.6, server version: 2.9.6
parser: hiredis

          PING        1/10  5549ms total, 18021.27 ops/sec
          PING       50/10  3385ms total, 29542.10 ops/sec
          PING      200/10  3304ms total, 30266.34 ops/sec
          PING    20000/10  3254ms total, 30731.41 ops/sec
          PING    50000/10  3313ms total, 30184.12 ops/sec
          PING   100000/10  3023ms total, 33079.72 ops/sec
 SET small str        1/10  6709ms total, 14905.35 ops/sec
 SET small str       50/10  3550ms total, 28169.01 ops/sec
 SET small str      200/10  3272ms total, 30562.35 ops/sec
 SET small str    20000/10  3387ms total, 29524.65 ops/sec
 SET small str    50000/10  3356ms total, 29797.38 ops/sec
 SET small str   100000/10  2974ms total, 33624.75 ops/sec
 GET small str        1/10  6967ms total, 14353.38 ops/sec
 GET small str       50/10  3286ms total, 30432.14 ops/sec
 GET small str      200/10  3350ms total, 29850.75 ops/sec
 GET small str    20000/10  3620ms total, 27624.31 ops/sec
 GET small str    50000/10  3361ms total, 29753.05 ops/sec
 GET small str   100000/10  2793ms total, 35803.80 ops/sec
 SET large str        1/10  8355ms total, 11968.88 ops/sec
 SET large str       50/10  3723ms total, 26860.06 ops/sec
 SET large str      200/10  3558ms total, 28105.68 ops/sec
 SET large str    20000/10  3761ms total, 26588.67 ops/sec
 SET large str    50000/10  3776ms total, 26483.05 ops/sec
 SET large str   100000/10  3575ms total, 27972.03 ops/sec
 GET large str        1/10  8859ms total, 11287.96 ops/sec
 GET large str       50/10  3858ms total, 25920.17 ops/sec
 GET large str      200/10  3776ms total, 26483.05 ops/sec
 GET large str    20000/10  4191ms total, 23860.65 ops/sec
 GET large str    50000/10  4013ms total, 24919.01 ops/sec
 GET large str   100000/10  3441ms total, 29061.32 ops/sec
          INCR        1/10  6517ms total, 15344.48 ops/sec
          INCR       50/10  3398ms total, 29429.08 ops/sec
          INCR      200/10  3183ms total, 31416.90 ops/sec
          INCR    20000/10  3160ms total, 31645.57 ops/sec
          INCR    50000/10  2992ms total, 33422.46 ops/sec
          INCR   100000/10  2989ms total, 33456.01 ops/sec
         LPUSH        1/10  6105ms total, 16380.02 ops/sec
         LPUSH       50/10  3268ms total, 30599.76 ops/sec
         LPUSH      200/10  3073ms total, 32541.49 ops/sec
         LPUSH    20000/10  3280ms total, 30487.80 ops/sec
         LPUSH    50000/10  3299ms total, 30312.22 ops/sec
         LPUSH   100000/10  2903ms total, 34447.12 ops/sec
     LRANGE 10        1/10  8596ms total, 11633.32 ops/sec
     LRANGE 10       50/10  3643ms total, 27449.90 ops/sec
     LRANGE 10      200/10  3427ms total, 29180.04 ops/sec
     LRANGE 10    20000/10  3734ms total, 26780.93 ops/sec
     LRANGE 10    50000/10  3720ms total, 26881.72 ops/sec
     LRANGE 10   100000/10  3899ms total, 25647.60 ops/sec
    LRANGE 100        1/10 14431ms total,  6929.53 ops/sec
    LRANGE 100       50/10  5416ms total, 18463.81 ops/sec
    LRANGE 100      200/10  5310ms total, 18832.39 ops/sec
    LRANGE 100    20000/10  5499ms total, 18185.12 ops/sec
    LRANGE 100    50000/10  5728ms total, 17458.10 ops/sec
    LRANGE 100   100000/10  5392ms total, 18545.99 ops/sec
```


## Sending Commands

Each Redis command is exposed as a function on the `client` object.
All functions take variable number of individual arguments followed by an **optional callback**.

Example:

    ```lua
    client:mset("test keys 1", "test val 1", "test keys 2", "test val 2", function (err, res) end)
    ```


For a list of Redis commands, see [Redis Command Reference](http://redis.io/commands)

The commands can be specified in uppercase or lowercase for convenience.  `client:get()` is the same as `client:GET()`.

## API

### Connection events

`client` object will emit some events about the state of the connection to the Redis server.

### "connect"

`client` will emit `connect` event when first write event is received (stream is connected to redis).

## "registerCommand"
  todo

## "command"
  todo

### "disconnect"

`client` will emit `disconnect` event when connection is disconnected (either because of an error or per user request).

### "error"

`client` will emit `error` when encountering an error with connection to the Redis server.

Note that "error" is a special event type in luvit.  If there are no listeners for an "error" event, luvit will exit.

## redis:new(port, host, autoReconnect)
Create a new client connection.  `port` defaults to `6379` and `host` defaults
to `127.0.0.1`.  If you have `redis-server` running on the same computer as node, then the defaults for
port and host are probably fine.

### autoReconnect
luvi-redis has autoReconnect to `redis-sever` build in. You can turn it of by setting 'autoReconnect' to false.
 TODO: add more explanation how it works.


## client:disconnect()

When this function is called, the connection is not immediately terminated. Instead, new commands are no longer accepted and the connection is only terminated when all pending commands have been written to the socket, their respective replies have been read and their respective callbacks have been executed.


`new()` returns a `RedisClient` object that is named `client` in all of the examples here.


Credits
-------
Thanks to Salvatore Sanfilippo for creating Redis in first place and hiredis driver,
Matt Ranney for creating node-redis,
Alexander Gladysh for creating lua hiredis,
Tim Caswell for creating luvit,
and Vladimir Dronnikov for all help as without him that module would never happen.
Thank you all!




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


