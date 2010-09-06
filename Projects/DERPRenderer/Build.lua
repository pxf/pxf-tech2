Import("../../PxfBuild.lua")

project = NewProject("DERPRenderer")

--project.settings.debug = True
--project.settings.static = True
project:RequireModule("pri")
project:RequireModule("img")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()