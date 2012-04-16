local redisNative = require('../../build/redis')
local commands = require('./commands')
local Emitter = require('core').Emitter
local fs = require("fs")
local timer = require('timer')
local RedisClient = Emitter:extend()

RedisClient.lua = {}

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

function RedisClient:command(...)
  self.redisNativeClient:command(...)
end

function RedisClient:disconnect(...)
  self.redisNativeClient:disconnect()
end

function RedisClient:initialize(host, port, autoReconnect)

  host = host or "127.0.0.1"
  port = port or 6379
  autoReconnect = autoReconnect or true

  self.hiredisVersion = redisNative.version
  self.redisNativeClient = redisNative.createClient(host,port)

  self.redisNativeClient:onError(function(err)
    if(autoReconnect and self.retryTimer == nil) then
      self.retryTimer = timer.setTimeout(250, function ()
        self:initialize(host, port, autoReconnect)
        self.retryTimer = nil
    end)

    end
    self:emit("error", err)
  end)

  self.redisNativeClient:onConnect(function()
    self:emit("connect")
  end)

  self.redisNativeClient:onDisconnect(function()
    self:emit("disconnect")
  end)

end

RedisClient.print = function(err, reply)
  if err then
    print("Error: ", err)
  else
    print("Reply: ", reply)
  end
end

return  RedisClient
