Import("../../PxfBuild.lua")

project = NewProject("NetMorgler")

project:RequireModule("net")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()