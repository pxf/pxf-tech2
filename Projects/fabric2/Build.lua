Import("../../PxfBuild.lua")

project = NewProject("fabric2")

project:RequireLibrary("glfw")
project:RequireLibrary("lua")
project:RequireLibrary("sstat")

project:RequireModule("pri")
project:RequireModule("img")

project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()
