local redis = require("redis")
local num_clients = 10
local num_requests = 100000
local tests = {}
local Object = require('core').Object
local small_str="1234"
local large_str=""
local table= require("table")
local os= require("os")
local math= require("math")
local string= require("string")

while(#large_str < 2049) do
    large_str=large_str .. '-'
end


function lpad(input, len, chr)

    local str = tostring(input)
    chr = chr or " ";

    while (#str < len) do
        str = chr .. str
    end
    return str
end

local Test = Object:extend()

function Test:initialize(args)


    self.args = args

    self.callback = nil
    self.clients = {}
    self.clients_ready = 0
    self.commands_sent = 0
    self.commands_completed = 0
    self.max_pipeline = args.pipeline

    self.connect_latency = {}
    self.ready_latency = {}
    self.command_latency = {}

    local commandArgs = {unpack(self.args.args)}
    table.insert(commandArgs ,function (err, res)
        if (err) then
            error(err)
        end
        self.commands_completed=self.commands_completed+1
        self:fill_pipeline()
    end)

    self.commandArgs=commandArgs

end

function Test:run(callback)

    self.callback = callback

    for i = 0, num_clients-1 do


        local new_client = redis:new()

        self.clients_ready=self.clients_ready+1
        self.clients[i] = new_client

    end
    self:on_clients_ready()
end

function Test:on_clients_ready()


    self.test_start = os.clock()

    self:fill_pipeline()
end

function Test:fill_pipeline()
    local pipeline = self.commands_sent - self.commands_completed;

    while (self.commands_sent < num_requests and pipeline < self.max_pipeline) do
        self.commands_sent=self.commands_sent+1
        pipeline=pipeline+1
        self:send_next()
    end

    if (self.commands_completed == num_requests) then
        self:print_stats()
        self:stop_clients()

    end
end

function Test:stop_clients()

    self.clients={}
    self.callback();
end

function Test:send_next()

    local cur_client = self.commands_sent % #self.clients
    local command_num = self.commands_sent

    self.clients[cur_client]:command(unpack(self.commandArgs))
end

function Test:print_stats()
    local duration = os.clock() - self.test_start;

    print(lpad(self.args.descr, 14) .. lpad(self.args.pipeline, 9) .. "/" .. self.clients_ready .. " "..lpad(string.format("%.2f",duration), 6) .. "s total, " .. lpad(string.format("%.2f",num_requests /duration), 10) .. " ops/sec")
end


table.insert(tests,Test:new({descr= "PING",  args= {"ping"}, pipeline= 1}))
table.insert(tests,Test:new({descr= "PING",  args= {"ping"}, pipeline= 50}))
table.insert(tests,Test:new({descr= "PING",  args= {"ping"}, pipeline= 200}))
table.insert(tests,Test:new({descr= "PING",  args= {"ping"}, pipeline= 20000}))
table.insert(tests,Test:new({descr= "PING",  args= {"ping"}, pipeline= 50000}))
table.insert(tests,Test:new({descr= "PING",  args= {"ping"}, pipeline= 100000}))

table.insert(tests,Test:new({descr= "SET small str", command= "set", args= {"set", "foo_rand000000000000", small_str}, pipeline= 1}))
table.insert(tests,Test:new({descr= "SET small str", command= "set", args= {"set", "foo_rand000000000000", small_str}, pipeline= 50}))
table.insert(tests,Test:new({descr= "SET small str", command= "set", args= {"set","foo_rand000000000000", small_str}, pipeline= 200}))
table.insert(tests,Test:new({descr= "SET small str", command= "set", args= {"set","foo_rand000000000000", small_str}, pipeline= 20000}))
table.insert(tests,Test:new({descr= "SET small str", command= "set", args= {"set","foo_rand000000000000", small_str}, pipeline= 50000}))
table.insert(tests,Test:new({descr= "SET small str", command= "set", args= {"set","foo_rand000000000000", small_str}, pipeline= 100000}))


table.insert(tests,Test:new({descr= "GET small str", args= {"get","foo_rand000000000000"}, pipeline= 1}))
table.insert(tests,Test:new({descr= "GET small str",args= {"get","foo_rand000000000000"}, pipeline= 50}))
table.insert(tests,Test:new({descr= "GET small str", args= {"get","foo_rand000000000000"}, pipeline= 200}))
table.insert(tests,Test:new({descr= "GET small str", args= {"get","foo_rand000000000000"}, pipeline= 20000}))
table.insert(tests,Test:new({descr= "GET small str", args= {"get","foo_rand000000000000"}, pipeline= 50000}))
table.insert(tests,Test:new({descr= "GET small str",  args= {"get","foo_rand000000000000"}, pipeline= 100000}))



table.insert(tests,Test:new({descr= "SET large str",  args= {"set","foo_rand000000000001", large_str}, pipeline= 1}))
table.insert(tests,Test:new({descr= "SET large str", args= {"set","foo_rand000000000001", large_str}, pipeline= 50}))
table.insert(tests,Test:new({descr= "SET large str", args= {"set","foo_rand000000000001", large_str}, pipeline= 200}))
table.insert(tests,Test:new({descr= "SET large str",  args= {"set","foo_rand000000000001", large_str}, pipeline= 20000}))
table.insert(tests,Test:new({descr= "SET large str", args= {"set","foo_rand000000000001", large_str}, pipeline= 50000}))
table.insert(tests,Test:new({descr= "SET large str", args= {"set","foo_rand000000000001", large_str}, pipeline= 100000}))


table.insert(tests,Test:new({descr= "GET large str", command= "get", args= {"get","foo_rand000000000001"}, pipeline= 1}))
table.insert(tests,Test:new({descr= "GET large str", command= "get", args= {"get","foo_rand000000000001"}, pipeline= 50}))
table.insert(tests,Test:new({descr= "GET large str", command= "get", args= {"get","foo_rand000000000001"}, pipeline= 200}))
table.insert(tests,Test:new({descr= "GET large str", command= "get", args= {"get","foo_rand000000000001"}, pipeline= 20000}))
table.insert(tests,Test:new({descr= "GET large str", command= "get", args= {"get","foo_rand000000000001"}, pipeline= 50000}))
table.insert(tests,Test:new({descr= "GET large str", command= "get", args= {"get","foo_rand000000000001"}, pipeline= 100000}))


table.insert(tests,Test:new({descr= "INCR", command= "incr", args= {"incr","counter_rand000000000000"}, pipeline= 1}))
table.insert(tests,Test:new({descr= "INCR", command= "incr", args= {"incr","counter_rand000000000000"}, pipeline= 50}))
table.insert(tests,Test:new({descr= "INCR", command= "incr", args= {"incr","counter_rand000000000000"}, pipeline= 200}))
table.insert(tests,Test:new({descr= "INCR", command= "incr", args= {"incr","counter_rand000000000000"}, pipeline= 20000}))
table.insert(tests,Test:new({descr= "INCR", command= "incr", args= {"incr","counter_rand000000000000"}, pipeline= 50000}))
table.insert(tests,Test:new({descr= "INCR", command= "incr", args= {"incr","counter_rand000000000000"}, pipeline= 100000}))

table.insert(tests,Test:new({descr= "LPUSH", command= "lpush", args= {"lpush","mylist", small_str}, pipeline= 1}))
table.insert(tests,Test:new({descr= "LPUSH", command= "lpush", args= {"lpush","mylist", small_str}, pipeline= 50}))
table.insert(tests,Test:new({descr= "LPUSH", command= "lpush", args= {"lpush","mylist", small_str}, pipeline= 200}))
table.insert(tests,Test:new({descr= "LPUSH", command= "lpush", args= {"lpush","mylist", small_str}, pipeline= 20000}))
table.insert(tests,Test:new({descr= "LPUSH", command= "lpush", args= {"lpush","mylist", small_str}, pipeline= 50000}))
table.insert(tests,Test:new({descr= "LPUSH", command= "lpush", args= {"lpush","mylist", small_str}, pipeline= 100000}))

table.insert(tests,Test:new({descr= "LRANGE 10", command= "lrange", args= {"lrange","mylist", "0", "9"}, pipeline= 1}))
table.insert(tests,Test:new({descr= "LRANGE 10", command= "lrange", args= {"lrange","mylist", "0", "9"}, pipeline= 50}))
table.insert(tests,Test:new({descr= "LRANGE 10", command= "lrange", args= {"lrange","mylist", "0", "9"}, pipeline= 200}))
table.insert(tests,Test:new({descr= "LRANGE 10", command= "lrange", args= {"lrange","mylist", "0", "9"}, pipeline= 20000}))
table.insert(tests,Test:new({descr= "LRANGE 10", command= "lrange", args= {"lrange","mylist", "0", "9"}, pipeline= 50000}))
table.insert(tests,Test:new({descr= "LRANGE 10", command= "lrange", args= {"lrange","mylist", "0", "9"}, pipeline= 100000}))

table.insert(tests,Test:new({descr= "LRANGE 100", command= "lrange", args= {"lrange","mylist", "0", "99"}, pipeline= 1}))
table.insert(tests,Test:new({descr= "LRANGE 100", command= "lrange", args= {"lrange","mylist", "0", "99"}, pipeline= 50}))
table.insert(tests,Test:new({descr= "LRANGE 100", command= "lrange", args= {"lrange","mylist", "0", "99"}, pipeline= 200}))
table.insert(tests,Test:new({descr= "LRANGE 100", command= "lrange", args= {"lrange","mylist", "0", "99"}, pipeline= 20000}))
table.insert(tests,Test:new({descr= "LRANGE 100", command= "lrange", args= {"lrange","mylist", "0", "99"}, pipeline= 50000}))
table.insert(tests,Test:new({descr= "LRANGE 100", command= "lrange", args= {"lrange","mylist", "0", "99"}, pipeline= 100000}))

function next()
    local test = table.remove(tests, 1)
    if (test) then
        test:run(function ()

            next()
        end)
    else
        print("End of tests.")
        process.exit(0)
    end
end

next()