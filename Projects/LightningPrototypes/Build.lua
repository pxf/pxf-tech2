Import("../../PxfBuild.lua")

project = NewProject("LightningPrototypes")

--project.settings.debug = True
--project.settings.static = True
project:RequireLibrary("zeromq")
project:RequireLibrary("protobuffers")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Client/Source/*.cpp")
project:AddSourceDirectory("Client/Source/*.cc")
project:Build()
