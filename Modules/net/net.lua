Import("../../PxfBuild.lua")


module = NewModule("net")
module:RequireLibrary("enet")
module:AddIncludeDirectory("Include")
module:AddSourceDirectory("Source/*.cpp")
