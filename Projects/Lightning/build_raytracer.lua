Import("../../PxfBuild.lua")

project = NewProject("raytracer")

project:RequireLibrary("glfw")
project:RequireLibrary("zthread")
project:RequireLibrary("lua")
project:RequireLibrary("sstat")
project:RequireLibrary("protobuffers")

project:RequireModule("pri")
project:RequireModule("img")
project:RequireModule("mesh")

project:AddIncludeDirectory("Include/Raytracer/")
project:AddIncludeDirectory("Include/ClientLib/")
project:AddSourceDirectory("Source/Raytracer/*.cpp")
project:AddSourceDirectory("Source/ClientLib/*.cpp")
project:AddSourceDirectory("Source/ClientLib/*.cc")
project:Build()
