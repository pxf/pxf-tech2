Import("../../PxfBuild.lua")

project = NewProject("kdtree")

project:RequireLibrary("glfw")
project:RequireModule("pri")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()
