local redisNative = require('../../build/redis')
local commands = require('./commands')
local Emitter = require('core').Emitter
local fs = require("fs")

local RedisClient = Emitter:extend()

RedisClient.lua = {}

for index, value in ipairs(commands) do

  RedisClient[value] = function(self, ...)
    self.redisNative:command(value, ...)
    end

  RedisClient[value:upper()]=function(self, ...)
    self.redisNative:command(value:upper(), ...)
    end

end

function RedisClient:registerCommand(commandName, scriptFilePath, numOfKeys, callback)
  local script = fs.readFileSync(scriptFilePath, 'utf8');
  self.lua[commandName]={}
  self.lua[commandName].numOfKeys=numOfKeys;
  self.redisNative:command('SCRIPT','LOAD',script, function(err,res)
    self.lua[commandName].sha1=res;
    RedisClient[commandName]= function(self,...)

    self.redisNative:command('EVALSHA', self.lua[commandName].sha1,self.lua[commandName].numOfKeys, ...)
    end
    callback()
  end)

end

function RedisClient:command(...)
  self.redisNative:command(...)
end

function RedisClient:disconnect(...)
  self.redisNative:disconnect()
end

function RedisClient:initialize(host, port)

  host = host or "127.0.0.1"
  port = port or 6379

  self.hiredisVersion = redisNative.version
  self.redisNative = redisNative.createClient(host,port)

  self.redisNative:onError(function()
    self:emit("error")
  end)

  self.redisNative:onConnect(function()
    self:emit("connect")
  end)

  self.redisNative:onDisconnect(function()
    self:emit("disconnect")
  end)

end

return RedisClient