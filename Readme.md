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
#BENCHMARKS (comparison of node-redis and luvit-redis).

Benchmark performed on single box, on Intel I7 i920 .


## BENCHMARK (benchmark.lua)
x/yy x - pipeline size, yy - num of clients

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

## BENCHMARK (multi_bench.js)
x/yy x- pipeline size, yy - num of clients

```
Client count: 10, node version: 0.7.6, server version: 2.9.6, parser: hiredis
         PING,     1/10 min/max/avg/p95:    0/   4/   0.05/   1.00   5581ms total, 17917.94 ops/sec
         PING,    50/10 min/max/avg/p95:    0/  11/   1.65/   3.00   3314ms total, 30175.02 ops/sec
         PING,   200/10 min/max/avg/p95:    0/  17/   6.15/  11.00   3077ms total, 32499.19 ops/sec
         PING, 20000/10 min/max/avg/p95:   17/1351/ 572.94/1114.10   3109ms total, 32164.68 ops/sec
         PING, 50000/10 min/max/avg/p95:   16/3042/1242.46/2700.55   3108ms total, 32175.03 ops/sec
         PING, 100000/10 min/max/avg/p95:  161/2837/1539.29/2636.10   2857ms total, 35001.75 ops/sec
SET small str,     1/10 min/max/avg/p95:    0/   6/   0.06/   1.00   6682ms total, 14965.58 ops/sec
SET small str,    50/10 min/max/avg/p95:    0/   7/   1.68/   3.00   3375ms total, 29629.63 ops/sec
SET small str,   200/10 min/max/avg/p95:    0/  19/   6.44/  11.00   3224ms total, 31017.37 ops/sec
SET small str, 20000/10 min/max/avg/p95:   13/1435/ 595.94/1140.20   3220ms total, 31055.90 ops/sec
SET small str, 50000/10 min/max/avg/p95:   39/3248/1341.40/2872.05   3315ms total, 30165.91 ops/sec
SET small str, 100000/10 min/max/avg/p95:  190/2917/1595.05/2664.00   2945ms total, 33955.86 ops/sec
GET small str,     1/10 min/max/avg/p95:    0/  10/   0.05/   1.00   5771ms total, 17328.02 ops/sec
GET small str,    50/10 min/max/avg/p95:    0/   5/   1.60/   3.00   3209ms total, 31162.36 ops/sec
GET small str,   200/10 min/max/avg/p95:    0/  21/   6.03/  10.00   3019ms total, 33123.55 ops/sec
GET small str, 20000/10 min/max/avg/p95:   13/1348/ 559.75/1069.00   3036ms total, 32938.08 ops/sec
GET small str, 50000/10 min/max/avg/p95:   26/3020/1229.13/2647.55   3078ms total, 32488.63 ops/sec
GET small str, 100000/10 min/max/avg/p95:  173/2802/1541.18/2613.10   2823ms total, 35423.31 ops/sec
SET large str,     1/10 min/max/avg/p95:    0/  11/   0.06/   1.00   6682ms total, 14965.58 ops/sec
SET large str,    50/10 min/max/avg/p95:    0/  14/   1.84/   3.00   3692ms total, 27085.59 ops/sec
SET large str,   200/10 min/max/avg/p95:    1/  28/   6.89/  12.00   3447ms total, 29010.73 ops/sec
SET large str, 20000/10 min/max/avg/p95:   26/1575/ 659.98/1269.55   3592ms total, 27839.64 ops/sec
SET large str, 50000/10 min/max/avg/p95:   53/3750/1521.82/3225.00   3837ms total, 26062.03 ops/sec
SET large str, 100000/10 min/max/avg/p95:  177/3451/1922.32/3234.55   3471ms total, 28810.14 ops/sec
SET large buf,     1/10 min/max/avg/p95:    0/  40/   0.11/   1.00  11510ms total,  8688.10 ops/sec
SET large buf,    50/10 min/max/avg/p95:    0/  16/   4.82/   8.00   9657ms total, 10355.18 ops/sec
SET large buf,   200/10 min/max/avg/p95:    0/  53/  18.58/  32.00   9300ms total, 10752.69 ops/sec
SET large buf, 20000/10 min/max/avg/p95:   25/4815/1841.53/3472.80  10183ms total,  9820.29 ops/sec
SET large buf, 50000/10 min/max/avg/p95:   58/10000/3881.28/8560.30  10077ms total,  9923.59 ops/sec
SET large buf, 100000/10 min/max/avg/p95:  735/9948/5223.01/9424.40   9970ms total, 10030.09 ops/sec
GET large str,     1/10 min/max/avg/p95:    0/  31/   0.06/   1.00   6689ms total, 14949.92 ops/sec
GET large str,    50/10 min/max/avg/p95:    0/   9/   1.93/   4.00   3862ms total, 25893.32 ops/sec
GET large str,   200/10 min/max/avg/p95:    0/  23/   7.55/  14.00   3779ms total, 26462.03 ops/sec
GET large str, 20000/10 min/max/avg/p95:    0/1718/ 716.94/1473.55   3901ms total, 25634.45 ops/sec
GET large str, 50000/10 min/max/avg/p95:    0/3717/1574.86/3297.75   3945ms total, 25348.54 ops/sec
GET large str, 100000/10 min/max/avg/p95:  280/3312/1821.58/2937.20   3409ms total, 29334.12 ops/sec
         INCR,     1/10 min/max/avg/p95:    0/  11/   0.06/   1.00   6081ms total, 16444.66 ops/sec
         INCR,    50/10 min/max/avg/p95:    0/   9/   1.64/   3.00   3287ms total, 30422.88 ops/sec
         INCR,   200/10 min/max/avg/p95:    0/  19/   6.12/  11.00   3065ms total, 32626.43 ops/sec
         INCR, 20000/10 min/max/avg/p95:   17/1461/ 591.67/1120.65   3210ms total, 31152.65 ops/sec
         INCR, 50000/10 min/max/avg/p95:   45/3094/1249.21/2677.00   3152ms total, 31725.89 ops/sec
         INCR, 100000/10 min/max/avg/p95:  177/2895/1594.01/2727.10   2915ms total, 34305.32 ops/sec
        LPUSH,     1/10 min/max/avg/p95:    0/   1/   0.06/   1.00   6157ms total, 16241.68 ops/sec
        LPUSH,    50/10 min/max/avg/p95:    0/   7/   1.68/   3.00   3356ms total, 29797.38 ops/sec
        LPUSH,   200/10 min/max/avg/p95:    0/  17/   6.42/  11.00   3215ms total, 31104.20 ops/sec
        LPUSH, 20000/10 min/max/avg/p95:   12/1418/ 576.38/1117.55   3122ms total, 32030.75 ops/sec
        LPUSH, 50000/10 min/max/avg/p95:   33/2865/1163.68/2498.15   2938ms total, 34036.76 ops/sec
        LPUSH, 100000/10 min/max/avg/p95:  187/2838/1559.24/2635.20   2857ms total, 35001.75 ops/sec
    LRANGE 10,     1/10 min/max/avg/p95:    0/  11/   0.07/   1.00   7151ms total, 13984.06 ops/sec
    LRANGE 10,    50/10 min/max/avg/p95:    0/  10/   1.77/   3.00   3547ms total, 28192.84 ops/sec
    LRANGE 10,   200/10 min/max/avg/p95:    0/  20/   6.76/  12.00   3382ms total, 29568.30 ops/sec
    LRANGE 10, 20000/10 min/max/avg/p95:    4/1642/ 647.35/1362.00   3509ms total, 28498.15 ops/sec
    LRANGE 10, 50000/10 min/max/avg/p95:    3/3413/1409.65/2998.65   3521ms total, 28401.02 ops/sec
    LRANGE 10, 100000/10 min/max/avg/p95:  196/3730/2184.62/3497.65   3771ms total, 26518.16 ops/sec
   LRANGE 100,     1/10 min/max/avg/p95:    0/  13/   0.14/   1.00  14683ms total,  6810.60 ops/sec
   LRANGE 100,    50/10 min/max/avg/p95:    0/  14/   2.86/   6.00   5720ms total, 17482.52 ops/sec
   LRANGE 100,   200/10 min/max/avg/p95:    1/  23/  10.44/  18.00   5226ms total, 19135.09 ops/sec
   LRANGE 100, 20000/10 min/max/avg/p95:    1/2635/1002.76/2084.65   5354ms total, 18677.62 ops/sec
   LRANGE 100, 50000/10 min/max/avg/p95:    1/4722/2214.09/4375.85   5312ms total, 18825.30 ops/sec
   LRANGE 100, 100000/10 min/max/avg/p95: 1588/5047/3470.49/4641.20   5425ms total, 18433.18 ops/sec

```
## TODO

* unit tests
* client auth
* docs
* more examples
* c optimizations ? (-O3 ?)


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


