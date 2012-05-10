local Redis = require('../')

local client = Redis:new()

client:on('error', function (err)
    print('Error (error callback): ', err)
end)

client:registerCommand('myget', [[
  local x = redis.call('get', KEYS[1])
  if not x then x = '(N/A)' end
  return x
]], 1, function()
  client:myget('foo1', Redis.print) -- (N/A)
  client:myget('foo2', Redis.print) -- (N/A)
end)

client:registerCommandFromFile('mycustom', __dirname .. '/../params.lua', 3, function()
  client:mycustom('foo1', 'foo2', 'foo3', 'bar1', 'bar2', p)
end)
