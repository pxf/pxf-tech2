Import("../../PxfBuild.lua")


module = NewModule("SampleModule")
module:RequireLibrary("glfw")
module:AddIncludeDirectory("Include")
module:AddIncludeDirectory("Include")
module:AddSourceDirectory("Source/*.cpp")
