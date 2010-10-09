Import("../../PxfBuild.lua")

project = NewProject("GamepadMorgler")

project:RequireLibrary("gamepad")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()