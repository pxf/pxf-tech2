Import("../../PxfBuild.lua")


module = NewModule("iphone")
module:RequireLibrary("glfw")
module:RequireLibrary("glew")
module:RequireLibrary("soil")
module:AddIncludeDirectory("Include")
module:AddSourceDirectory("Source/*.cpp")
