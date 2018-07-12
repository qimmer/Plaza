--
-- Created by Kim Johannsen
-- User: Kim
-- Date: 01/06/2018
-- Time: 09.16
--

local Core = require('Core')
local Foundation = require('Foundation')
local Lua = require('Lua')
local TestGame = module('TestGame')

--
-- Subscriptions
--

local function OnAppUpdate()

end

local function OnTestEvent(c)
    print("Hello Test Event: " .. c)
end

--
-- Service Functions
--

local function TestFunction(a, b)
    TestGame.FireTestEvent(a + b)

    return a + b
end

--
-- Module Composition
--

print "Module Loaded from Lua!"

TestGame.Component("TestComponent", {
    Name = Type.StringRef,
    Age = Type.u32
})

TestGame.Event("TestEvent", {
    c = Type.float
})

TestGame.Service("TestService", {
    AppUpdate = OnAppUpdate,
    TestEvent = OnTestEvent
})

TestGame.Function("TestFunction", TestFunction, {
    a = Type.float,
    b = Type.float
})
