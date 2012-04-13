local redis = require('redis')

local client = redis:new()


client:sadd('hk1','hk2','hk3','abc','123',function(err,res)
  if err then
    print("error: "..err.message)
  else
    print("response: "..res)
  end

client:on("connect",function()
  print("conect callback")
end)
client:on("disconnect",function()
  print("disconnect callback")
end)

  client:on("error",function()

  print("dupaa22222 errror")
end)


local http = require("http")

http.createServer(function (req, res)
  local body = "Hello world\n"
  res:writeHead(200, {
    ["Content-Type"] = "text/plain",
    ["Content-Length"] = #body
  })
  res:finish(body)
end):listen(8080)

print("Server listening at http://localhost:8080/")