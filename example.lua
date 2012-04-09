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