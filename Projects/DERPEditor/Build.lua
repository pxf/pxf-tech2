Import("../../PxfBuild.lua")

project = NewProject("DERPEditor")

--project.settings.debug = True
--project.settings.static = True
project:RequireLibrary("enet")
project:RequireModule("pri")
project:RequireModule("img")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()