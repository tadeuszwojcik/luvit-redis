-- Load our native module
local hiredis = require('./redis')

local client = assert(hiredis.createClient("127.0.0.1", 6379))
client:command("SET","A","B",function(response)
  print(response)
end)
