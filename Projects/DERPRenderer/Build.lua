Import("../../PxfBuild.lua")

project = NewProject("DERPRenderer")

--project.settings.debug = True
--project.settings.static = True
project:RequireLibrary("jsoncpp")
project:RequireModule("pri")
project:RequireModule("img")
project:RequireModule("mesh")
project:RequireModule("snd")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()