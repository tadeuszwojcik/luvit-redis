local redis = require('redis')

local client = redis:new()

client:on("connect", function()
  print("conect callback")
end)

client:on("disconnect",function()
  print("disconnect callback")
end)

client:on("error",function()
  print("error callback")
end)

client:set('A','luvit rocks')

local http = require("http")

http.createServer(function (req, res)
  client:get('A', function(err, response)
    local body = nil
    if err then
      body = "error: ".. err.message
    else
      body = "response: ".. response
    end
    res:writeHead(200,
      {
      ["Content-Type"] = "text/plain",
      ["Content-Length"] = #body
      })

  res:finish(body)
  end)
end):listen(8080)

print("Server listening at http://localhost:8080/")