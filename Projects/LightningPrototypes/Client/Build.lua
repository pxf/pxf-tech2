Import("../../PxfBuild.lua")

project = NewProject("LighningPrototypes")

--project.settings.debug = True
--project.settings.static = True
project:RequireLibrary("zeromq")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()
