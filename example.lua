local redis = require('redis')

local client = assert(redis.createClient())
client:command("SET","A","B",function(response)
  print(response)
end)

client:command("GET","A",function(response)
  print(response)
end)

