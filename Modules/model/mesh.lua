Import("../../PxfBuild.lua")

module = NewModule("mesh")
module:RequireLibrary("openctm")
module:AddIncludeDirectory("Include")
module:AddSourceDirectory("Source/*.cpp")
