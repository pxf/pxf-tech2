Import("../../PxfBuild.lua")

project = NewProject("fabric2")

--project:RequireLibrary("glfw")
project:RequireLibrary("lua")
project:RequireLibrary("sstat")

project:RequireModule("snd")
project:RequireModule("pri")
project:RequireModule("img")
project:RequireModule("json")

project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()
