Import("../../PxfBuild.lua")

project = NewProject("DERPEditor")

--project.settings.debug = True
--project.settings.static = True
project:RequireLibrary("openctm");
project:RequireLibrary("enet")
project:RequireLibrary("glfw")
project:RequireLibrary("lua")
project:RequireLibrary("openctm")
project:RequireModule("pri")
project:RequireModule("snd")
project:RequireModule("img")
project:RequireModule("mesh")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()