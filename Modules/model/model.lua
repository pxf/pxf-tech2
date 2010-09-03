Import("../../PxfBuild.lua")

module = NewModule("model")
module:RequireLibrary("openctm")
module:AddIncludeDirectory("Include")
module:AddSourceDirectory("Source/*.cpp")
