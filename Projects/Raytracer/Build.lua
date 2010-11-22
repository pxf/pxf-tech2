Import("../../PxfBuild.lua")

project = NewProject("raytracer")

project:RequireLibrary("glfw")
project:RequireLibrary("zthread")
project:RequireModule("pri")
project:RequireModule("img")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()
