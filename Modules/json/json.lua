Import("../../PxfBuild.lua")

module = NewModule("json")
module:RequireLibrary("jsoncpp")
module:AddIncludeDirectory("Include")
module:AddSourceDirectory("Source/*.cpp")
