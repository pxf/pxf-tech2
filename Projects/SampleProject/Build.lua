Import("../../PxfBuild.lua")

project = NewProject("SampleProject")

--project.settings.debug = True
--project.settings.static = True

project:RequireModule("SampleModule")
project:RequireLibrary("angelscript")
project:RequireLibrary("glfw")
project:RequireLibrary("soil")
project:RequireLibrary("glew")
project:RequireLibrary("lua")
project:RequireLibrary("enet")
project:RequireLibrary("box2d")
project:RequireLibrary("s7")
project:RequireLibrary("rtaudio")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()