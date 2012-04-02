-- Load our native module
local hiredis = require('./hiredis')

local conn = assert(hiredis.connect("127.0.0.1", 6379))
conn:command("SET","A","B")
conn:command("GET","A",function(response)
  print(response)
end)
