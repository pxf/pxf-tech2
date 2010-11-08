Import("../../PxfBuild.lua")

project = NewProject("raytracer")

project:RequireLibrary("glfw")
project:RequireModule("pri")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()
