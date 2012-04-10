local redisNativeClient = require('../../build/redis')
local commands = require('./commands')
local Object = require('core').Object
local fs = require("fs")
local RedisClient = Object:extend()
RedisClient.lua={}
for index, value in ipairs(commands) do
  RedisClient[value] = function(self, ...)
    self.redisNativeClient:command(value, ...)
    end

  RedisClient[value:upper()]=function(self, ...)
    self.redisNativeClient:command(value:upper(), ...)
    end
end

function RedisClient:registerCommand(commandName, scriptFilePath, numOfKeys, callback)
  local script = fs.readFileSync(scriptFilePath, 'utf8');
  self.lua[commandName]={}
  self.lua[commandName].numOfKeys=numOfKeys;
  self.redisNativeClient:command('SCRIPT','LOAD',script, function(err,res)
    self.lua[commandName].sha1=res;
    RedisClient[commandName]= function(self,...)

    self.redisNativeClient:command('EVALSHA', self.lua[commandName].sha1,self.lua[commandName].numOfKeys, ...)
    end
    callback()
  end)

end




function RedisClient:initialize(host, port)
  self.redisNativeClient = redisNativeClient.createClient(host,port)
end

return RedisClient