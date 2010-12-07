Import("../../PxfBuild.lua")

project = NewProject("client")

--project.settings.debug = True
--project.settings.static = True
--project:RequireLibrary("zeromq")
project:RequireLibrary("zthread")
project:RequireLibrary("protobuffers")

project:AddIncludeDirectory("Include/Client/")
project:AddIncludeDirectory("Include/ClientLib/")

project:AddSourceDirectory("Source/Client/*.cpp")
project:AddSourceDirectory("Source/ClientLib/*.cpp")
project:AddSourceDirectory("Source/ClientLib/*.cc")
project:Build()
