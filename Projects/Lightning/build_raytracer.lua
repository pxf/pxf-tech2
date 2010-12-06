Import("../../PxfBuild.lua")

project = NewProject("raytracer")

project:RequireLibrary("glfw")
project:RequireLibrary("zthread")
project:RequireLibrary("lua")
project:RequireLibrary("rtaudio")
project:RequireLibrary("stb_vorbis")
project:RequireLibrary("sstat")

project:RequireModule("pri")
project:RequireModule("snd")
project:RequireModule("img")
project:RequireModule("mesh")

project:AddIncludeDirectory("Include/Raytracer/")
project:AddSourceDirectory("Source/Raytracer/*.cpp")
project:Build()
