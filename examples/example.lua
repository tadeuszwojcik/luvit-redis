local redis = require('../')

local client = redis:new()

client:on("error", function (err)
    print("Error (error callback): ", err)
end)

client:set('test-key','luvit-rocks', redis.print)
client:hset("hash key", "hashtest 1", "some value", redis.print)
client:hset("hash key", "hashtest 2", "some other value", redis.print)
client:hkeys("hash key", function (err, replies)
    print(#replies.." replies:")
    for i,v in ipairs(replies) do print(i,v) end
end)

client:mset("test keys 1", "test val 1", "test keys 2", "test val 2", function (err, res) end)
