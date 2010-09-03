Import("../../PxfBuild.lua")

module = NewModule("model")
module:AddIncludeDirectory("Include")
module:AddSourceDirectory("Source/*.cpp")
