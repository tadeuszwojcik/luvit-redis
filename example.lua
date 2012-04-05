local redis = require('redis')

local client = assert(redis.createClient("127.0.0.1", 6379))
client:command("SET","A","B",function(response)
  print(response)
end)

client:command("GET","A",function(response)
  print(response)
end)
