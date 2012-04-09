local redisClient = require('redis')


local client = assert(redisClient:new())
client:set("A","B",function(err,response)
  if(err) then
    print("error")
    print(err)
  else
    print(response)
    client:get("A",function(err,response)
      print(response)
  end)
  end
end)


