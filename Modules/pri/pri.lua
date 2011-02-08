Import("../../PxfBuild.lua")


module = NewModule("pri")
module:RequireLibrary("glfw")
module:RequireLibrary("glew")
module:RequireLibrary("soil")
module:AddIncludeDirectory("Include")
module:AddSourceDirectory("Source/*.cpp")
--module:AddSourceDirectory("Source/Shader/*.cpp")
