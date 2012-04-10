local redis = require('redis')

local client = redis:new()


client:registerCommand('params','./params.lua',3,function()
  client:params('hk1','hk2','hk3','abc','123',function(err,res)
    print(err)
    for k,v in pairs(res) do print(k,v) end
  end)

end)