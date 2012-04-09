local redisNativeClient = require('../../build/redis')
local commands = require('./commands')
local Object = require('core').Object

local RedisClient = Object:extend()

for index, value in ipairs(commands) do
  RedisClient[value] = function(self, ...)
    self.redisNativeClient:command(value, ...)
    end

  RedisClient[value:upper()]=function(self, ...)
    self.redisNativeClient:command(value:upper(), ...)
    end
end

function RedisClient:initialize(host, port)
  self.redisNativeClient = redisNativeClient.createClient(host,port)
end

return RedisClient