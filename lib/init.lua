local Emitter = require('core').Emitter
local fs = require('fs')
local timer = require('timer')

local redisNative = require('../build/redis')

local RedisClient = Emitter:extend()

-- proxy native client commands
do
  local commands = require('./commands')
  for index, value in ipairs(commands) do
    value = value:lower()
    RedisClient[value] = function(self, ...)
      self.redisNativeClient:command(value, ...)
    end
    RedisClient[value:upper()] = RedisClient[value]
  end
end

-- client constructor
function RedisClient:initialize(host, port, autoReconnect)

  host = host or '127.0.0.1'
  port = port or 6379
  autoReconnect = autoReconnect or true

  self.hiredisVersion = redisNative.version
  self.redisNativeClient = redisNative.createClient(host, port)

  self.redisNativeClient:onError(function(err)
    if autoReconnect and self.retryTimer == nil then
      -- FIXME: should implement exponential growth of timeout
      --   e.g. 250, 500, 1000, 2000...
      self.retryTimer = timer.setTimeout(250, function()
        self:initialize(host, port, autoReconnect)
        self.retryTimer = nil
      end)
    end
    self:emit('error', err)
  end)

  self.redisNativeClient:onConnect(function()
    self:emit('connect')
  end)

  self.redisNativeClient:onDisconnect(function()
    self:emit('disconnect')
  end)

end

-- register custom command
function RedisClient:registerCommand(
    commandName,
    scriptFilePath,
    numOfKeys,
    callback
  )
  -- FIXME: in async environment it's bad to be sync beyound making require()s
  local script = fs.readFileSync(scriptFilePath)
  if not self.lua then
    self.lua = {}
  end
  self.lua[commandName] = {}
  self.lua[commandName].numOfKeys = numOfKeys
  self.redisNativeClient:command('SCRIPT', 'LOAD', script, function(err, res)
    self.lua[commandName].sha1 = res;
    RedisClient[commandName] = function(self, ...)
      self.redisNativeClient:command(
          'EVALSHA',
          self.lua[commandName].sha1,
          self.lua[commandName].numOfKeys,
          ...
        )
    end
    callback()
  end)
end

-- FIXME: shouldn't it reuse RedisClient hash we filled up before?
function RedisClient:command(...)
  self.redisNativeClient:command(...)
end

-- orderly disconnect the client
function RedisClient:disconnect(...)
  self.redisNativeClient:disconnect()
end

-- helpers
RedisClient.print = function(err, reply)
  if err then
    print('Error: ', err)
  else
    print('Reply: ', reply)
  end
end

-- exports
return RedisClient
